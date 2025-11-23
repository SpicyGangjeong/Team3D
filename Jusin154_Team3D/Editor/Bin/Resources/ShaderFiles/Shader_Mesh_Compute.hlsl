float PI = 3.141592;

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
    float3 Scale;
    float pad0;

    float4 Rotation;

    float3 Translation;
    float pad1;
};


StructuredBuffer<KeyFrame> g_KeyFrameBuffer : register(t0);
StructuredBuffer<Channel> g_ChannelBuffer : register(t1);

RWStructuredBuffer<LocalPos> g_LocalPosOutput : register(u0);


cbuffer AnimCB : register(b0)
{
    float CurrentTime;
    float Duration;
    float Speed;
    int BoneCount;
    
    row_major float4x4 PreTransformMatrix;
};

float4 quatSlerp(float4 q1, float4 q2, float t)
{
    float cosTheta = dot(q1, q2);

    if (cosTheta < 0.0f)
    {
        q2 = -q2;
        cosTheta = -cosTheta;
    }

    if (cosTheta > 0.9995f)
        return normalize(lerp(q1, q2, t));

    float theta = acos(cosTheta);
    float sinTheta = sin(theta);

    float w1 = sin((1.0f - t) * theta) / sinTheta;
    float w2 = sin(t * theta) / sinTheta;

    return q1 * w1 + q2 * w2;
}


[numthreads(256, 1, 1)]
void CS_LocalSRT(uint3 DTID : SV_DispatchThreadID)
{
    uint boneIndex = DTID.x;
    if (boneIndex >= (uint) BoneCount)
        return;

    float time = CurrentTime;
    if (Duration > 0)
        time = min(time, Duration);

    Channel channel = g_ChannelBuffer[boneIndex];

    LocalPos result;

    if (channel.KeyCount == 0)
    {
        result.Scale = float3(1, 1, 1);
        result.Rotation = float4(0, 0, 0, 1);
        result.Translation = float3(0, 0, 0);
        result.pad0 = result.pad1 = 0.0f;

        g_LocalPosOutput[boneIndex] = result;
        return;
    }

    uint firstIndex = channel.StartIndex;
    uint lastIndex = channel.StartIndex + channel.KeyCount - 1;

    KeyFrame keyframe1 = g_KeyFrameBuffer[firstIndex];
    KeyFrame keyframe2 = g_KeyFrameBuffer[lastIndex];

    if (time <= keyframe1.fTrackPosition)
    {
        result.Scale = keyframe1.vScale;
        result.Rotation = keyframe1.vRotation;
        result.Translation = keyframe1.vTranslation;
        result.pad0 = result.pad1 = 0.0f;

        g_LocalPosOutput[boneIndex] = result;
        return;
    }

    if (time >= keyframe2.fTrackPosition)
    {
        result.Scale = keyframe2.vScale;
        result.Rotation = keyframe2.vRotation;
        result.Translation = keyframe2.vTranslation;
        result.pad0 = result.pad1 = 0.0f;

        g_LocalPosOutput[boneIndex] = result;
        return;
    }

    for (uint i = 0; i < channel.KeyCount - 1; ++i)
    {
        uint A = channel.StartIndex + i;
        uint B = channel.StartIndex + i + 1;

        KeyFrame ka = g_KeyFrameBuffer[A];
        KeyFrame kb = g_KeyFrameBuffer[B];

        if (time >= ka.fTrackPosition && time <= kb.fTrackPosition)
        {
            float t = (time - ka.fTrackPosition) / (kb.fTrackPosition - ka.fTrackPosition);

            result.Scale = lerp(ka.vScale, kb.vScale, t);
            result.Rotation = quatSlerp(ka.vRotation, kb.vRotation, t);
            result.Translation = lerp(ka.vTranslation, kb.vTranslation, t);
            result.pad0 = result.pad1 = 0.0f;

            g_LocalPosOutput[boneIndex] = result;
            return;
        }
    }

    result.Scale = keyframe2.vScale;
    result.Rotation = keyframe2.vRotation;
    result.Translation = keyframe2.vTranslation;
    result.pad0 = result.pad1 = 0.0f;

    g_LocalPosOutput[boneIndex] = result;
}
