struct LocalPos
{
    row_major float4x4 Combined;
    row_major float4x4 Local;
    row_major float4x4 LocalCombined;
};

struct BoneCombined
{
    row_major float4x4 BoneCombinedTransformation;
};

StructuredBuffer<BoneCombined> g_CombinedMatIn : register(t0);

RWStructuredBuffer<LocalPos> g_LocalPosOutput : register(u0);

cbuffer BoneInsertionCB : register(b0)
{
    uint iStartOffset;
    uint iMatrixCount;
    uint iPad0;
    uint iPad1;
};

[numthreads(256, 1, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iBoneIndex = DTid.x;

    if (iBoneIndex >= iMatrixCount)
    {
        return;
    }
    
    float4x4 CombinedMatrix = g_CombinedMatIn[iBoneIndex].BoneCombinedTransformation;
    
    g_LocalPosOutput[iBoneIndex+iStartOffset].LocalCombined = CombinedMatrix;
    g_LocalPosOutput[iBoneIndex+iStartOffset].Combined = CombinedMatrix;

}
