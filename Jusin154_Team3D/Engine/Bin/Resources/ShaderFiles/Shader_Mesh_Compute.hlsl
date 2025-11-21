float PI = 3.141592;

struct KeyFrame
{
    float3 vScale;
    float4 vRotation;
    float3 vTranslation;
    float fTrackPosition;
};

struct BoneInfo
{
    int ParentIndex;
};

struct BoneMatrix
{
    row_major float4x4 LocalMatrix;
    row_major float4x4 CombinedMatrix;

};

StructuredBuffer<KeyFrame> g_KeyFrameBufferInput : register(t0);
StructuredBuffer<BoneInfo> g_BoneInoBufferInput : register(t1);

RWStructuredBuffer<BoneMatrix> gBoneMatrixOutput : register(u0);

cbuffer g_ConstantBuffer : register(b0)
{
    float CurrentTime;
    float Duration;
    float Speed;
    int BoneCount;
    
    float4x4 PreTransformMatrix;

};

float4x4 QuatToMatrix(float4 q)
{
    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;

    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    float4x4 m;

    m[0][0] = 1.0 - 2.0 * (yy + zz);
    m[0][1] = 2.0 * (xy + wz);
    m[0][2] = 2.0 * (xz - wy);
    m[0][3] = 0.0;

    m[1][0] = 2.0 * (xy - wz);
    m[1][1] = 1.0 - 2.0 * (xx + zz);
    m[1][2] = 2.0 * (yz + wx);
    m[1][3] = 0.0;

    m[2][0] = 2.0 * (xz + wy);
    m[2][1] = 2.0 * (yz - wx);
    m[2][2] = 1.0 - 2.0 * (xx + yy);
    m[2][3] = 0.0;

    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;

    return m;
}

float4x4 MakeTRS(float3 scale, float4 rot, float3 trans)
{
    float4x4 S =
    {
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0, 1
    };

    float4x4 R = QuatToMatrix(normalize(rot));

    float4x4 T =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        trans.x, trans.y, trans.z, 1
    };

    return mul(S, mul(R, T));
}

[numthreads(256, 1, 1)]
void CS_MAIN(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint boneIndex = DispatchThreadID.x;
    if (boneIndex >= BoneCount)
        return;

    KeyFrame k = g_KeyFrameBufferInput[boneIndex];
    BoneInfo bi = g_BoneInoBufferInput[boneIndex];

    float3 s = k.vScale;
    float4 r = k.vRotation;
    float3 t = k.vTranslation;

    float4x4 local = MakeTRS(s, r, t);

    float4x4 combined;

    if (bi.ParentIndex < 0)
    {
        combined = mul(local, PreTransformMatrix);
    }
    else
    {
        combined = mul(local, gBoneMatrixOutput[bi.ParentIndex].CombinedMatrix);
    }

    gBoneMatrixOutput[boneIndex].LocalMatrix = local;
    gBoneMatrixOutput[boneIndex].CombinedMatrix = combined;
}
