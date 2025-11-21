float PI = 3.141592;

struct Mesh
{
    float3 vPosition;
    float3 vNormal;
    float3 vTangent;
    float3 vBinormal;
    float2 vTexcoord;

    uint4 vBlendIndex;
    float4 vBlendWeight;
};

struct SkinngMesh
{
    float3 vPosition;
    float3 vNormal;
    float3 vTangent;
    float3 vBinormal;
    float2 vTexcoord;

};

StructuredBuffer<Mesh> g_MeshBufferInput : register(t0);
RWStructuredBuffer<SkinngMesh> gSkinngMeshOutput : register(u0);

cbuffer g_ConstantBuffer : register(b0)
{
    row_major float4x4 BoneMatrix[512];
};

//[numthreads(256, 1, 1)]
[numthreads(256, 1, 1)]
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint iIndex = DispatchThreadID.x;

    Mesh mesh = g_MeshBufferInput[iIndex];

    float fWeightW = 1.f -
        (mesh.vBlendWeight.x + mesh.vBlendWeight.y + mesh.vBlendWeight.z);

    matrix SkinMatrix =
        BoneMatrix[mesh.vBlendIndex.x] * mesh.vBlendWeight.x +
        BoneMatrix[mesh.vBlendIndex.y] * mesh.vBlendWeight.y +
        BoneMatrix[mesh.vBlendIndex.z] * mesh.vBlendWeight.z +
        BoneMatrix[mesh.vBlendIndex.w] * fWeightW;

    float4 vPosition = mul(float4(mesh.vPosition, 1), SkinMatrix);
    float4 vNormal = mul(float4(mesh.vNormal, 0), SkinMatrix);
    float4 vBinormal = mul(float4(mesh.vBinormal, 0), SkinMatrix);
    float4 vTangent = mul(float4(mesh.vTangent, 0), SkinMatrix);


    SkinngMesh outv;
    outv.vPosition = vPosition.xyz;
    outv.vNormal = vNormal.xyz;
    outv.vTangent = vTangent.xyz;
    outv.vBinormal = vBinormal.xyz;
    outv.vTexcoord = mesh.vTexcoord;

    gSkinngMeshOutput[iIndex] = outv;
}
