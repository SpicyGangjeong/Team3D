#ifndef ENGINE_SHADER_SAMPLERS_HLSLI
#define ENGINE_SHADER_SAMPLERS_HLSLI

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};
sampler AnisoTropy_BLUR_Sampler = sampler_state
{
    Filter = AnisoTropic;
    MaxAnisotropy = 16;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
    MipLodBias = 1.f; // 밉맵 레벨 +1시킴
    MinLOD = 0;
    MaxLOD = 0xFFFFFFFF;
    ComparisonFunc = ALWAYS;
};

sampler ClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

sampler MirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = mirror;
    AddressV = mirror;
};

sampler BorderZeroSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0, 0, 0, 0);
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

#endif // ENGINE_SHADER_SAMPLERS_HLSLI
