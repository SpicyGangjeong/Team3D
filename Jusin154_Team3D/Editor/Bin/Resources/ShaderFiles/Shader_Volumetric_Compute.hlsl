#include "Engine_Shader_Functions.hlsli"
#include "Engine_Shader_Defines.hlsli"

static const float EPSILON = 1e-6f;

struct LightData
{
    float3 vDirection;
    float fRange;
    float4 vPosition;
    float4 vDiffuse;
};

StructuredBuffer<LightData> g_LightBuffer : register(t0);
Texture2D<float4> g_ShadowMap : register(t1);
Texture2D<float4> NoiseTexture : register(t2);

SamplerState g_Sampler : register(s0);

RWTexture3D<float4> g_VolumeUAV : register(u0);

cbuffer g_ConstantBuffer : register(b0) // b0 << 이 숫자와 컨스턴트 쉐이더 바인딩할때 인덱스가 동일해야함
{
    uint3 VolumeSize;
    float fDepthPackExponent;
    
    float fTemporalAccum;
    float fFrameIndex;
    float fNear;
    float fFar;
    
    float3 vCameraPosition;
    float fDensity;
    
    float fLightIntensity;
    float3 vLightDirection;
   
    float4 vLightColor;
    
    row_major matrix InvProjMatrix;
    row_major matrix InvViewMatrix;
    
    row_major matrix ShadowViewMatrix;
    row_major matrix ShadowViewProjMatrix;
    
    float fAsymmetryParameter;
    uint iNumLight;
    float fHeightOffset;
    float fBaseHeight;
};

float3  ConvertThreadIdToNdc(uint3 id, uint3 dims);
float   ConvertNdcZToDepth(float ndcZ);
float3  ConvertToWorldPosition(float3 ndc, float depth);
float3  ConvertThreadIdToWorldPosition(uint3 id, uint3 dims);
float   HenyeyGreensteinPhaseFunction(float3 vInputDir, float3 vOutputDir, float fAnisotropy);
float   ComputePointLightVisible(float3 vLightDir, float fRange);
float   ComputeShadowVisibility(float3 vWorldPosition);
// 볼륨 텍스쳐 밀도, 산란 계산
[numthreads(8, 8, 4)]
void CS_MAIN( uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint3 dims;
    g_VolumeUAV.GetDimensions(dims.x, dims.y, dims.z);
    if (!all(DispatchThreadID < dims))
        return;

    float3 vWorldPosition = ConvertThreadIdToWorldPosition(DispatchThreadID, dims);
    float3 toCamera = normalize(vCameraPosition - vWorldPosition);
    
    float3 lighting = float3(0.1f, 0.1f, 0.1f); //vLightColor.rgb * vLightColor.a;
    
    float3 lightDir = float3(0.f, 0.f, 0.f);
    
    [loop]
    for (uint i = 0; i < iNumLight; ++i)
    {
        float visibility = 1.f;
        
        // Directional
        if (0.f < length(g_LightBuffer[i].vDirection))
        {
            lightDir = normalize(g_LightBuffer[i].vDirection);
            visibility = ComputeShadowVisibility(vWorldPosition);
        }
        else // Point
        {
            lightDir = vWorldPosition - g_LightBuffer[i].vPosition.xyz;
            visibility = ComputePointLightVisible(lightDir, g_LightBuffer[i].fRange);
            lightDir = normalize(lightDir);
        }

        float3 toLight = -lightDir;
        float phase = HenyeyGreensteinPhaseFunction(lightDir, toCamera, fAsymmetryParameter);
        
        lighting += g_LightBuffer[i].vDiffuse.rgb * g_LightBuffer[i].vDiffuse.a * phase * visibility;
    }

    float fHeightDensity = max(0.1f, exp(-(vWorldPosition.y - fBaseHeight) * fHeightOffset)) * fDensity;
    
    g_VolumeUAV[DispatchThreadID] = float4(lighting * fLightIntensity * fHeightDensity, fHeightDensity);
}

float3 ConvertThreadIdToNdc(uint3 id, uint3 dims)
{
    float3 ndc = id + 0.5f;
    
    ndc *= float3(2.0f / dims.x, -2.0f / dims.y, 1.0f / dims.z);
    ndc += float3(-1.0f, 1.0f, 0.0f);
    
    return ndc;
}
float ConvertNdcZToDepth(float ndcZ)
{
    float t = pow(ndcZ, fDepthPackExponent);
    return t * (fFar - fNear) + fNear;
}
float3 ConvertToWorldPosition(float3 ndc, float depth)
{
    float4 vViewRay = mul(float4(ndc, 1.f), InvProjMatrix);
    vViewRay /= vViewRay.w;
    vViewRay /= vViewRay.z;
    float4 worldPosition = mul(float4(vViewRay.xyz * depth, 1.f), InvViewMatrix);
    return worldPosition.xyz;
}
float3 ConvertThreadIdToWorldPosition(uint3 id, uint3 dims)
{
    float3 ndc = ConvertThreadIdToNdc(id, dims);
    float depth = ConvertNdcZToDepth(ndc.z);
    return ConvertToWorldPosition(ndc, depth);
}
float HenyeyGreensteinPhaseFunction(float3 vInputDir, float3 vOutputDir, float fAsymmetryParameter)
{
    float cosTheta = dot(vInputDir, vOutputDir);

    float denom = pow(1.f + (fAsymmetryParameter * fAsymmetryParameter) - 2.f * fAsymmetryParameter * cosTheta, 3.f / 2.f);
    return (1.f / (4.f * PI)) * ((1.f - (fAsymmetryParameter * fAsymmetryParameter)) / max(denom, EPSILON));
}

float ComputePointLightVisible(float3 vLightDir, float fRange)
{
    float fDistance = length(vLightDir);
    
    //if (fDistance < fRange)
    //    return 1.f;
    //else
    //    return 0.f;
    
    return saturate((fRange * 1.5f - fDistance) / fRange);
}

float ComputeShadowVisibility(float3 vWorldPosition)
{
    float4 vShadowCoord = mul(float4(vWorldPosition, 1.f), ShadowViewProjMatrix);
    float3 DepthUV = vShadowCoord.xyz / vShadowCoord.w;
 
    DepthUV.x = DepthUV.x * 0.5f + 0.5f;
    DepthUV.y = DepthUV.y * -0.5f + 0.5f;

    //float fNoise = NoiseTexture.SampleLevel(g_Sampler, DepthUV.xy, 4).r;
    
    return 1.f;
}
