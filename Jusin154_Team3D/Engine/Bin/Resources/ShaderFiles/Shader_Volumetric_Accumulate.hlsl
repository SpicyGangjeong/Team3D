#include "Engine_Shader_Functions.hlsli"
#include "Engine_Shader_Defines.hlsli"

static const float DensityScale = 0.01f;

Texture3D<float4> g_VolumeSRV : register(t0);

RWTexture3D<float4> g_AccVolumeUAV : register(u0);

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

float ConvertNdcZToDepth(float ndcZ);
float SliceTickness(float ndcZ, uint dimZ);
float4 ScatterStep(float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float tickness);
// 볼륨 텍스쳐 밀도, 산란 누적 계산
[numthreads(8, 8, 4)]
void CS_MAIN( uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint3 dims;
    g_AccVolumeUAV.GetDimensions(dims.x, dims.y, dims.z);
    if (!all(DispatchThreadID < dims))
        return;

    float4 accumulatedColor = float4(0.f, 0.f, 0.f, 1.f);
    uint3 Position = uint3(DispatchThreadID.xy, 0);
    
    [loop]
    for (uint i = 0; i < dims.z; ++i)
    {
        Position.z = i;
        float4 slice = g_VolumeSRV[Position];
        float tickness = SliceTickness((float)i / dims.z, dims.z);

        accumulatedColor = ScatterStep(accumulatedColor.rgb, accumulatedColor.a, slice.rgb, slice.a, tickness);
        g_AccVolumeUAV[Position] = accumulatedColor;
    }
}
    
float SliceTickness (float ndcZ, uint dimZ)
{
      return ConvertNdcZToDepth(ndcZ + 1.f / float(dimZ)) - ConvertNdcZToDepth(ndcZ);
}

float ConvertNdcZToDepth(float ndcZ)
{
    float t = pow(ndcZ, fDepthPackExponent);
    return t * (fFar - fNear) + fNear;
}

float4 ScatterStep(float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float tickness)
{
    sliceDensity = max(sliceDensity, 0.000001f);
    sliceDensity *= DensityScale;
    float sliceTransmittance = exp(-sliceDensity * tickness);

	// Frostbite의 누적 방식
    float3 sliceLightIntegral = sliceLight * (1.f - sliceTransmittance) / sliceDensity;

    accumulatedLight += sliceLightIntegral * accumulatedTransmittance;
    accumulatedTransmittance *= sliceTransmittance;

    return float4(accumulatedLight, accumulatedTransmittance);
}
