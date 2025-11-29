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
   
    Channel channel = g_ChannelBuffer[boneIndex];

    LocalPos result;
   
    uint firstIndex = channel.StartIndex;
    uint keyCount = channel.KeyCount;
    uint lastIndex = firstIndex + keyCount - 1;

    KeyFrame keyLast = g_KeyFrameBuffer[lastIndex];

    if (time >= keyLast.fTrackPosition)
    {
        result.Scale = keyLast.vScale;
        result.Rotation = keyLast.vRotation;
        result.Translation = keyLast.vTranslation;
        result.pad0 = result.pad1 = 0.0f;
    }
    else
    {
        for (uint i = 0; i < keyCount - 1; ++i)
        {
            KeyFrame Sour = g_KeyFrameBuffer[firstIndex + i];
            KeyFrame Dest = g_KeyFrameBuffer[firstIndex + i + 1];

            if (time >= Sour.fTrackPosition && time <= Dest.fTrackPosition)
            {
                float ratio = (time - Sour.fTrackPosition) /
                          (Dest.fTrackPosition - Sour.fTrackPosition);

                result.Scale = lerp(Sour.vScale, Dest.vScale, ratio);
                result.Rotation = quatSlerp(Sour.vRotation, Dest.vRotation, ratio);
                result.Translation = lerp(Sour.vTranslation, Dest.vTranslation, ratio);
                result.pad0 = result.pad1 = 0.0f;
                break;
            }
        }
    }

    g_LocalPosOutput[channel.BoneIndex] = result;
    return;


}
