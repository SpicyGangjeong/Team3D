struct KeyFrame
{
    float3 vScale;
    float4 vRotation;
    float3 vTranslation;
    float fTrackPosition;
};

struct Channel
{
    uint StartIndex;
    uint KeyCount;
    uint BoneIndex;
};

struct LocalPos
{
    row_major float4x4 Local;
    row_major float4x4 Combined;
    row_major float4x4 LocalCombined;
};

struct BoneLocal
{
    row_major float4x4 BoneLocal;
};

StructuredBuffer<KeyFrame> g_KeyFrameBuffer : register(t0);
StructuredBuffer<Channel> g_ChannelBuffer : register(t1);
StructuredBuffer<KeyFrame> g_PrevKeyFrameBuffer : register(t2);
StructuredBuffer<Channel> g_PrevChannelBuffer : register(t3);
StructuredBuffer<int> g_ParentBuffer : register(t4);
StructuredBuffer<BoneLocal> g_BoneLocalBuffer : register(t5);
StructuredBuffer<uint> g_BoneRemap : register(t6);

RWStructuredBuffer<LocalPos> g_LocalPosOutput : register(u0);

cbuffer AnimCB : register(b0)
{
    float CurrentTime;
    float Duration;
    int MeshBoneCount;
    int BoneCount;

    int RootBoneIndex;
    int padding1;
    float PrevTime;
    float BlendRatio;
    
    row_major float4x4 PreTransformMatrix;
    float4 RootInitRot;
};

static const float EPS = 1e-6f;

float4 QuatNormalize(float4 q)
{
    return q * rsqrt(max(dot(q, q), EPS));
}

float4 QuatSlerp(float4 a, float4 b, float t)
{
    a = QuatNormalize(a);
    b = QuatNormalize(b);

    float cosTheta = dot(a, b);

    if (cosTheta < 0.0f)
    {
        b = -b;
        cosTheta = -cosTheta;
    }

    if (cosTheta > 0.9995f)
        return QuatNormalize(lerp(a, b, t));

    float theta = acos(saturate(cosTheta));
    float sinTheta = max(sin(theta), EPS);

    float w0 = sin((1.0f - t) * theta) / sinTheta;
    float w1 = sin(t * theta) / sinTheta;

    return a * w0 + b * w1;
}

float4x4 MakeAffine(float3 scale, float4 rot, float3 trans)
{
    rot = QuatNormalize(rot);

    float xx = rot.x * rot.x;
    float yy = rot.y * rot.y;
    float zz = rot.z * rot.z;
    float xy = rot.x * rot.y;
    float xz = rot.x * rot.z;
    float yz = rot.y * rot.z;
    float wx = rot.w * rot.x;
    float wy = rot.w * rot.y;
    float wz = rot.w * rot.z;

    float4x4 M;

    M._11 = (1 - 2 * (yy + zz)) * scale.x;
    M._12 = (2 * (xy + wz)) * scale.x;
    M._13 = (2 * (xz - wy)) * scale.x;
    M._14 = 0;

    M._21 = (2 * (xy - wz)) * scale.y;
    M._22 = (1 - 2 * (xx + zz)) * scale.y;
    M._23 = (2 * (yz + wx)) * scale.y;
    M._24 = 0;

    M._31 = (2 * (xz + wy)) * scale.z;
    M._32 = (2 * (yz - wx)) * scale.z;
    M._33 = (1 - 2 * (xx + yy)) * scale.z;
    M._34 = 0;

    M._41 = trans.x;
    M._42 = trans.y;
    M._43 = trans.z;
    M._44 = 1;

    return M;
}

uint FindKeyIndexCur(uint start, uint count, float t)
{
    uint lo = 0;
    uint hi = count - 1;

    float t0 = g_KeyFrameBuffer[start + 0].fTrackPosition;
    if (t <= t0)
        return 0;

    float tLast = g_KeyFrameBuffer[start + hi].fTrackPosition;
    if (t >= tLast)
        return hi - 1;

    while (hi - lo > 1)
    {
        uint mid = (lo + hi) >> 1;
        float tm = g_KeyFrameBuffer[start + mid].fTrackPosition;
        if (t < tm)
            hi = mid;
        else
            lo = mid;
    }
    return lo;
}

uint FindKeyIndexPrev(uint start, uint count, float t)
{
    uint lo = 0;
    uint hi = count - 1;

    float t0 = g_PrevKeyFrameBuffer[start + 0].fTrackPosition;
    if (t <= t0)
        return 0;

    float tLast = g_PrevKeyFrameBuffer[start + hi].fTrackPosition;
    if (t >= tLast)
        return hi - 1;

    while (hi - lo > 1)
    {
        uint mid = (lo + hi) >> 1;
        float tm = g_PrevKeyFrameBuffer[start + mid].fTrackPosition;
        if (t < tm)
            hi = mid;
        else
            lo = mid;
    }
    return lo;
}


float3 GetRow3(float4 v)
{
    return v.xyz;
}

float4x4 SampleLocalPrev(uint bone, float t)
{
    Channel ch = g_PrevChannelBuffer[bone];
    if (ch.KeyCount == 0)
        return g_BoneLocalBuffer[bone].BoneLocal;

    uint start = ch.StartIndex;
    uint count = ch.KeyCount;

    if (count == 1)
    {
        KeyFrame k = g_PrevKeyFrameBuffer[start];
        float3 trans = k.vTranslation;
        float4 rot = k.vRotation;

        if (bone == RootBoneIndex)
        {
            trans = 0;
            rot = RootInitRot;
        }
        return MakeAffine(k.vScale, rot, trans);
    }

    uint i0 = FindKeyIndexPrev(start, count, t);
    uint i1 = i0 + 1;

    KeyFrame k0 = g_PrevKeyFrameBuffer[start + i0];
    KeyFrame k1 = g_PrevKeyFrameBuffer[start + i1];

    float denom = max(EPS, (k1.fTrackPosition - k0.fTrackPosition));
    float ratio = saturate((t - k0.fTrackPosition) / denom);

    float3 scale = lerp(k0.vScale, k1.vScale, ratio);
    float3 trans = lerp(k0.vTranslation, k1.vTranslation, ratio);
    float4 rot = QuatSlerp(k0.vRotation, k1.vRotation, ratio);

    if (bone == RootBoneIndex)
    {
        trans = 0;
        rot = RootInitRot;
    }
    return MakeAffine(scale, rot, trans);
}

float4x4 SampleLocal(uint bone, float t)
{
    Channel ch = g_ChannelBuffer[bone];
    if (ch.KeyCount == 0)
        return g_BoneLocalBuffer[bone].BoneLocal;

    uint start = ch.StartIndex;
    uint count = ch.KeyCount;

    if (count == 1)
    {
        KeyFrame k = g_KeyFrameBuffer[start];
        float3 trans = k.vTranslation;
        float4 rot = k.vRotation;

        if (bone == RootBoneIndex)
        {
            trans = 0;
            rot = RootInitRot;
        }
        return MakeAffine(k.vScale, rot, trans);
    }

    uint i0 = FindKeyIndexCur(start, count, t);
    uint i1 = i0 + 1;

    KeyFrame k0 = g_KeyFrameBuffer[start + i0];
    KeyFrame k1 = g_KeyFrameBuffer[start + i1];

    float denom = max(EPS, (k1.fTrackPosition - k0.fTrackPosition));
    float ratio = saturate((t - k0.fTrackPosition) / denom);

    float3 scale = lerp(k0.vScale, k1.vScale, ratio);
    float3 trans = lerp(k0.vTranslation, k1.vTranslation, ratio);
    float4 rot = QuatSlerp(k0.vRotation, k1.vRotation, ratio);

    if (bone == RootBoneIndex)
    {
        trans = 0;
        rot = RootInitRot;
    }
    return MakeAffine(scale, rot, trans);
}
void SampleLocalTRS_Cur(uint bone, float t, out float3 S, out float4 R, out float3 T)
{
    Channel ch = g_ChannelBuffer[bone];

    if (ch.KeyCount == 0)
    {
        S = float3(1, 1, 1);
        R = float4(0, 0, 0, 1);
        T = float3(0, 0, 0);
        return;
    }

    uint start = ch.StartIndex;
    uint count = ch.KeyCount;

    if (count == 1)
    {
        KeyFrame k = g_KeyFrameBuffer[start];
        S = k.vScale;
        R = QuatNormalize(k.vRotation);
        T = k.vTranslation;
    }
    else
    {
        uint i0 = FindKeyIndexCur(start, count, t);
        uint i1 = i0 + 1;

        KeyFrame k0 = g_KeyFrameBuffer[start + i0];
        KeyFrame k1 = g_KeyFrameBuffer[start + i1];

        float denom = max(EPS, (k1.fTrackPosition - k0.fTrackPosition));
        float ratio = saturate((t - k0.fTrackPosition) / denom);

        S = lerp(k0.vScale, k1.vScale, ratio);
        T = lerp(k0.vTranslation, k1.vTranslation, ratio);
        R = QuatSlerp(k0.vRotation, k1.vRotation, ratio);
    }

    if (bone == RootBoneIndex)
    {
        T = 0;
        R = RootInitRot;
    }
}

void SampleLocalTRS_Prev(uint bone, float t, out float3 S, out float4 R, out float3 T)
{
    Channel ch = g_PrevChannelBuffer[bone];

    if (ch.KeyCount == 0)
    {
        S = float3(1, 1, 1);
        R = float4(0, 0, 0, 1);
        T = float3(0, 0, 0);
        return;
    }

    uint start = ch.StartIndex;
    uint count = ch.KeyCount;

    if (count == 1)
    {
        KeyFrame k = g_PrevKeyFrameBuffer[start];
        S = k.vScale;
        R = QuatNormalize(k.vRotation);
        T = k.vTranslation;
    }
    else
    {
        uint i0 = FindKeyIndexPrev(start, count, t);
        uint i1 = i0 + 1;

        KeyFrame k0 = g_PrevKeyFrameBuffer[start + i0];
        KeyFrame k1 = g_PrevKeyFrameBuffer[start + i1];

        float denom = max(EPS, (k1.fTrackPosition - k0.fTrackPosition));
        float ratio = saturate((t - k0.fTrackPosition) / denom);

        S = lerp(k0.vScale, k1.vScale, ratio);
        T = lerp(k0.vTranslation, k1.vTranslation, ratio);
        R = QuatSlerp(k0.vRotation, k1.vRotation, ratio);
    }

    if (bone == RootBoneIndex)
    {
        T = 0;
        R = RootInitRot;
    }
}


float4x4 SampleBlendedLocal(uint bone)
{
    float br = saturate(BlendRatio);

    if (br <= 0.0f)
        return SampleLocalPrev(bone, PrevTime);
    if (br >= 1.0f)
        return SampleLocal(bone, CurrentTime);

    float3 sA, sB, tA, tB;
    float4 rA, rB;

    SampleLocalTRS_Prev(bone, PrevTime, sA, rA, tA);
    SampleLocalTRS_Cur(bone, CurrentTime, sB, rB, tB);

    float3 s = lerp(sA, sB, br);
    float3 t = lerp(tA, tB, br);
    float4 r = QuatSlerp(rA, rB, br);

    if (bone == RootBoneIndex)
    {
        t = 0;
        r = RootInitRot;
    }

    return MakeAffine(s, r, t);
}

[numthreads(256, 1, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint globalBone = DTid.x;
    if (globalBone >= (uint) BoneCount)
        return;

    float4x4 localMat = SampleBlendedLocal(globalBone);
    g_LocalPosOutput[globalBone].Local = localMat;

    float4x4 combined = localMat;
    int p = g_ParentBuffer[globalBone];
    while (p != -1)
    {
        combined = mul(combined, SampleBlendedLocal((uint) p));
        p = g_ParentBuffer[p];
    }

    combined = mul(combined, PreTransformMatrix);
    g_LocalPosOutput[globalBone].Combined = combined;

    for (uint local = 0; local < (uint) MeshBoneCount; ++local)
    {
        if (g_BoneRemap[local] == globalBone)
        {
            g_LocalPosOutput[local].LocalCombined = combined;
        }
    }
}

