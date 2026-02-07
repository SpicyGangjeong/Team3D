#ifndef ENGINE_SHADER_SAMPLERS_HLSLI
#define ENGINE_SHADER_SAMPLERS_HLSLI

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};
sampler AnisoTropy_BLUR_Sampler = sampler_state // 밉맵이 있어야 의미가 있음
{
    Filter = AnisoTropic;
    MaxAnisotropy = 16;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
    MipLodBias = 0.5f; // 밉맵 레벨 +1시켜서 의도적으로 뭉갬
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

sampler ClampLinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
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
    BorderColor = float4(0.f, 0.f, 0.f, 0.f);
};

sampler BorderHalfSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0.5f, 0.5f, 0.5f, 0.5f);
};

sampler BorderZeroLinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0.f, 0.f, 0.f, 0.f);
};

sampler BorderOneSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(1.f, 1.f, 1.f, 1.f);
};

sampler SSAODataSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

#endif // ENGINE_SHADER_SAMPLERS_HLSLI
