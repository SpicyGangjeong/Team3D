struct LocalPos
{
    row_major float4x4 Combined;
    row_major float4x4 Local;
    row_major float4x4 LocalCombined;
};

StructuredBuffer<float4x4> g_LocalMatIn : register(t0);
StructuredBuffer<int> g_ParentBuffer : register(t1);
StructuredBuffer<uint> g_BoneRemap : register(t2);

RWStructuredBuffer<LocalPos> g_LocalPosOutput : register(u0);

cbuffer AnimCB : register(b0)
{
    float CurrentTime;
    float Duration;
    uint MeshBoneCount;
    uint BoneCount;

    uint CurrentAnimIndex;
    uint PrevAnimIndex;
    float PrevTime;
    float BlendRatio;

    int RootBoneIndex;
    int HeadBoneIndex;
    float HeadAimWeight;
    float paddding1;

    float3 TargetDir_Local;
    float padding;

    row_major float4x4 PreTransformMatrix;
    float4 RootInitRot;
};

[numthreads(256, 1, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint local = DTid.x;
    if (local >= MeshBoneCount)
        return;

    uint global = g_BoneRemap[local];

   float4x4 combined = g_LocalMatIn[global];

    int p = g_ParentBuffer[global];
    while (p != -1)
    {
        combined = mul(combined,g_LocalMatIn[p]);
        p = g_ParentBuffer[p];
    }

    combined = mul(combined, PreTransformMatrix);
    
    g_LocalPosOutput[local].Local = g_LocalMatIn[global];
    g_LocalPosOutput[local].LocalCombined = combined;
    g_LocalPosOutput[local].Combined = combined;

}
