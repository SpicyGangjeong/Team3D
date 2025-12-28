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


RWStructuredBuffer<float4x4> g_LocalMatrixOut : register(u0);

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

    float theta = acos(clamp(cosTheta, -1.0f, 1.0f));
    float sinTheta = max(sin(theta), EPS);

    float w0 = sin((1.0f - t) * theta) / sinTheta;
    float w1 = sin(t * theta) / sinTheta;

    return a * w0 + b * w1;
}

row_major float4x4 MakeAffine(float3 scale, float4 rot, float3 trans)
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

    float3 r0 = float3(1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy));
    float3 r1 = float3(2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx));
    float3 r2 = float3(2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy));

    r0 *= scale.x;
    r1 *= scale.y;
    r2 *= scale.z;

    row_major float4x4 M;
    M[0] = float4(r0, 0.0f);
    M[1] = float4(r1, 0.0f);
    M[2] = float4(r2, 0.0f);
    M[3] = float4(trans, 1.0f);
    return M;
}

void DecomposeAffine_RowMajor(row_major float4x4 M, out float3 S, out float4 R, out float3 T)
{
    T = float3(M._41, M._42, M._43);

    float3 r0 = float3(M._11, M._12, M._13);
    float3 r1 = float3(M._21, M._22, M._23);
    float3 r2 = float3(M._31, M._32, M._33);

    S = float3(length(r0), length(r1), length(r2));
    float3 invS = 1.0f / max(S, float3(EPS, EPS, EPS));

    r0 *= invS.x;
    r1 *= invS.y;
    r2 *= invS.z;

    float trace = r0.x + r1.y + r2.z;
    float4 q;

    if (trace > 0.0f)
    {
        float s = sqrt(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (r1.z - r2.y) / s;
        q.y = (r2.x - r0.z) / s;
        q.z = (r0.y - r1.x) / s;
    }
    else if (r0.x > r1.y && r0.x > r2.z)
    {
        float s = sqrt(1.0f + r0.x - r1.y - r2.z) * 2.0f;
        q.w = (r1.z - r2.y) / s;
        q.x = 0.25f * s;
        q.y = (r1.x + r0.y) / s;
        q.z = (r2.x + r0.z) / s;
    }
    else if (r1.y > r2.z)
    {
        float s = sqrt(1.0f + r1.y - r0.x - r2.z) * 2.0f;
        q.w = (r2.x - r0.z) / s;
        q.x = (r1.x + r0.y) / s;
        q.y = 0.25f * s;
        q.z = (r2.y + r1.z) / s;
    }
    else
    {
        float s = sqrt(1.0f + r2.z - r0.x - r1.y) * 2.0f;
        q.w = (r0.y - r1.x) / s;
        q.x = (r2.x + r0.z) / s;
        q.y = (r2.y + r1.z) / s;
        q.z = 0.25f * s;
    }

    R = QuatNormalize(q);
}

uint FindKeyIndexCur(uint start, uint count, float t)
{
    uint lo = 0;
    uint hi = count - 1;

    float tLast = g_KeyFrameBuffer[start + hi].fTrackPosition;
    if (t >= tLast)
        return hi - 1;

    while (lo + 1 < hi)
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

    float tLast = g_PrevKeyFrameBuffer[start + hi].fTrackPosition;
    if (t >= tLast)
        return hi - 1;

    while (lo + 1 < hi)
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

void SampleLocalTRS_Cur(uint bone, float t, out float3 S, out float4 R, out float3 T)
{
    Channel ch = g_ChannelBuffer[bone];

    if (ch.KeyCount == 0)
    {
        DecomposeAffine_RowMajor(g_BoneLocalBuffer[bone].BoneLocal, S, R, T);
        return;
    }
    
    if (ch.KeyCount < 2)
    {
        KeyFrame k = g_KeyFrameBuffer[ch.StartIndex];
        S = k.vScale;
        T = k.vTranslation;
        R = QuatNormalize(k.vRotation);
        return;
    }

    uint idx0 = FindKeyIndexCur(ch.StartIndex, ch.KeyCount, t);
    uint idx1 = idx0 + 1;

    KeyFrame k0 = g_KeyFrameBuffer[ch.StartIndex + idx0];
    KeyFrame k1 = g_KeyFrameBuffer[ch.StartIndex + idx1];

    float denom = max(k1.fTrackPosition - k0.fTrackPosition, EPS);
    float a = saturate((t - k0.fTrackPosition) / denom);

    S = lerp(k0.vScale, k1.vScale, a);
    T = lerp(k0.vTranslation, k1.vTranslation, a);
    R = QuatSlerp(k0.vRotation, k1.vRotation, a);
}

void SampleLocalTRS_Prev(uint bone, float t, out float3 S, out float4 R, out float3 T)
{
    Channel ch = g_PrevChannelBuffer[bone];

    if (ch.KeyCount == 0)
    {
        DecomposeAffine_RowMajor(g_BoneLocalBuffer[bone].BoneLocal, S, R, T);
        return;
    }
    
    if (ch.KeyCount < 2)
    {
        KeyFrame k = g_PrevKeyFrameBuffer[ch.StartIndex];
        S = k.vScale;
        T = k.vTranslation;
        R = QuatNormalize(k.vRotation);
        return;
    }

    uint idx0 = FindKeyIndexPrev(ch.StartIndex, ch.KeyCount, t);
    uint idx1 = idx0 + 1;

    KeyFrame k0 = g_PrevKeyFrameBuffer[ch.StartIndex + idx0];
    KeyFrame k1 = g_PrevKeyFrameBuffer[ch.StartIndex + idx1];

    float denom = max(k1.fTrackPosition - k0.fTrackPosition, EPS);
    float a = saturate((t - k0.fTrackPosition) / denom);

    S = lerp(k0.vScale, k1.vScale, a);
    T = lerp(k0.vTranslation, k1.vTranslation, a);
    R = QuatSlerp(k0.vRotation, k1.vRotation, a);
}

row_major float4x4 SampleBlendedLocal(uint bone)
{
    if (PrevAnimIndex == -1)
    {
        float3 S;
        float4 R;
        float3 T;
        SampleLocalTRS_Cur(bone, CurrentTime, S, R, T);
        if (bone == RootBoneIndex)
        {
            T = 0;
            R = RootInitRot;
        }
        return MakeAffine(S, R, T);
    }

    if (CurrentAnimIndex == PrevAnimIndex)
    {
        float3 S;
        float4 R;
        float3 T;
        SampleLocalTRS_Cur(bone, CurrentTime, S, R, T);

        if (bone == RootBoneIndex)
        {
            T = 0;
            R = RootInitRot;
        }
        return MakeAffine(S, R, T);
    }

    float br = saturate(BlendRatio);

    float3 sA, sB, tA, tB;
    float4 rA, rB;
    
    float prevT = PrevTime;

    if (BlendRatio < 1e-4f)
    {
        prevT = CurrentTime;
    }

    SampleLocalTRS_Prev(bone, prevT, sA, rA, tA);
    SampleLocalTRS_Cur(bone, CurrentTime, sB, rB, tB);

    float3 S = lerp(sA, sB, br);
    float3 T = lerp(tA, tB, br);
    float4 R = QuatSlerp(rA, rB, br);

    if (bone == RootBoneIndex)
    {
        T = 0;
        R = RootInitRot;
    }
    
    

    return MakeAffine(S, R, T);
}

[numthreads(256, 1, 1)]
void CS_LOCAL(uint3 DTid : SV_DispatchThreadID)
{
    uint bone = DTid.x;
    if (bone >= BoneCount)
        return;

    g_LocalMatrixOut[bone] = SampleBlendedLocal(bone);
}
