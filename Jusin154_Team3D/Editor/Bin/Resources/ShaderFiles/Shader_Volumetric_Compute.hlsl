#include "Engine_Shader_Functions.hlsli"
#include "Engine_Shader_Defines.hlsli"

static const float EPSILON = 1e-6f;

struct LightData
{
    uint    itype;
    float3  vDirection;
    
    float3  vPosition;
    float   fRange;
    
    float4  vColor;
};

//StructuredBuffer<LightData> g_LightBuffer : register(t1);

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
    
    float fAsymmetryParameter;
    float padding0;
    float padding1;
    float padding2;
}

float3  ConvertThreadIdToNdc(uint3 id, uint3 dims);
float   ConvertNdcZToDepth(float ndcZ);
float3  ConvertToWorldPosition(float3 ndc, float depth);
float3  ConvertThreadIdToWorldPosition(uint3 id, uint3 dims);
float   HenyeyGreensteinPhaseFunction(float3 vInputDir, float3 vOutputDir, float fAnisotropy);

// 볼륨 텍스쳐 밀도, 산란 계산
[numthreads(8, 8, 4)]
void CS_MAIN( uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint3 dims;
    g_VolumeUAV.GetDimensions(dims.x, dims.y, dims.z);
    if (!all(DispatchThreadID < dims))
        return;

    float3 worldPosition = ConvertThreadIdToWorldPosition(DispatchThreadID, dims);
    float3 toCamera = normalize(vCameraPosition - worldPosition);
    
    float3 lighting = vLightColor.rgb * vLightColor.a;
    float3 lightDir = normalize(vLightDirection);
    float3 toLight = -lightDir;
    
    float phase = HenyeyGreensteinPhaseFunction(lightDir, toCamera, fAsymmetryParameter);
    
    lighting += vLightColor.rgb * vLightColor.rgb * phase;

    g_VolumeUAV[DispatchThreadID] = float4(lighting * fLightIntensity * fDensity, fDensity);
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
