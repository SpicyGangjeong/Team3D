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
    int PlayHeadBone;
    int HeadBoneIndex;
    float HeadAimWeight;

    int SkipCount;
    int IsSkip;
    int _pad1;
    int _pad2;

    float3 TargetDir_Local;
    float padding3;

    row_major float4x4 PreTransformMatrix;
    float4 RootInitRot;
    
    int UseUpperBody;
    float UpperBlend;
    int SecondAnimIndex;
    float SecondAnimTime;
    
    row_major float4x4 WorldMatrix;

};

[numthreads(256, 1, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint local = DTid.x;
    if (local >= MeshBoneCount)
        return;

    uint global = g_BoneRemap[local];
    
    
    float4x4 localMat = g_LocalMatIn[global];
    float4x4 modelCombined = localMat;

    int p = g_ParentBuffer[global];
    while (p != -1)
    {
        modelCombined = mul(modelCombined, g_LocalMatIn[p]);
        p = g_ParentBuffer[p];
    }

    modelCombined = mul(modelCombined, PreTransformMatrix);

    float4x4 worldCombined = mul(modelCombined, WorldMatrix);

    g_LocalPosOutput[local].Local = localMat;
    g_LocalPosOutput[local].LocalCombined = modelCombined;
    g_LocalPosOutput[local].Combined = worldCombined;
}
