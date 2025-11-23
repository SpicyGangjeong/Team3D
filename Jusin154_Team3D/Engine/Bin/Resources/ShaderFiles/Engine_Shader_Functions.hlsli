#ifndef ENGINE_SHADER_FUNCTIONS_HLSLI
#define ENGINE_SHADER_FUNCTIONS_HLSLI

#include "Engine_Shader_Constants.hlsli"
#include "Engine_Shader_Structures.hlsli"
#include "Engine_Shader_Samplers.hlsli"

bool AlmostEqual7(float fA, float fB)
{
    return abs(fA - fB) < FLT_EPSILON7;
}
bool AlmostEqual5(float fA, float fB)
{
    return abs(fA - fB) < FLT_EPSILON5;
}
bool AlmostEqual3(float fA, float fB)
{
    return abs(fA - fB) < FLT_EPSILON3;
}

float ShadowVisibility_hwPCF(Texture2D ShadowMap, float4 vLightClip, float2 vShadowMapSize, float bias)
{
    float2 uv = float2(vLightClip.x / vLightClip.w, -vLightClip.y / vLightClip.w) * 0.5f + 0.5f;
    float fDepthCenter = saturate(vLightClip.z / vLightClip.w);
    float2 vTexel = 1.f / vShadowMapSize;
    
    float fVisibility = 0.f;
    for (int v = -1; v <= 1; ++v)
    {
        for (int u = -1; u <= 1; ++u)
        {
            float2 vOffset = float2(u, v) * vTexel;
            float fDepth = ShadowMap.Sample(DefaultSampler, uv + vOffset).x;
            fVisibility += (fDepthCenter - bias > fDepth) ? 0.f : 1.f;
        }
    }
    return fVisibility / 9.f;
}

float GetRimLight(float3 vCamPosition, float3 vPosition, float3 vNormal, float fRimPower, float fRimStrength)
{
    float fRimLight = (1 - dot(normalize(vCamPosition - vPosition), vNormal));
    fRimLight = pow(fRimLight, fRimPower);
    fRimLight = fRimLight * fRimStrength;
    return fRimLight;
}

float2 Get_MovedUV(float2 vOriginalUV, float fDeltaU, float fDeltaV, uint iIndexU, uint iIndexV)
{
    float2 vDelta = float2(fDeltaU, fDeltaV);
    float2 vOffset = float2(iIndexU * fDeltaU, iIndexV * fDeltaV);
    return vOriginalUV * vDelta + vOffset;
}

float2 UV_Cutting(float2 vUV, float2 vUVCutting, int iCurrentFrame)
{
    float2 UV = vUV; // 이미지의 UV값
    
    int iTotalFrame = vUVCutting.x * vUVCutting.y; // 이미지의 최대 프레임 (몇 곱하기 몇인지)
    
    int iFrameX = iCurrentFrame % (int) vUVCutting.x; // 현재 x축의 위치(현재 이미지의 몇번째 칸을 보여줄 지)
    int iFrameY = iCurrentFrame / (int) vUVCutting.x; // 현재 y축의 위치(현재 이미지의 몇번째 줄을 보여줄 지)
    
    float fFreamWidth = 1.0 / vUVCutting.x; // 1.0 나누기 이미지 갯수를 해서 한칸에 얼마나 갈지 정해준다.
    float fFreamHeight = 1.0 / vUVCutting.y; // 1.0 나누기 이미지 갯수를 해서 한줄에 얼마나 갈지 정해준다.
    
    UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth; // 먼저 uv를 0~1이 아닌 0~fFrameWidth로 만든 다음에 한칸씩 옆으로 밀어준다.
    UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight; // 먼저 uv를 0~1이 아닌 0~fFrameHeight로 만든 다음에 한줄씩 밑으로 내려준다.
    
    return UV;
}

// (Kd F(Lambert)) + (Ks F(cook-torrance))

//float LambertDiffuse(float c)
//{
//    return c / PI;
//}

// F(cook-torrance)
// DFG / (4 (Wo dot N) (Wi dot N))

// D(distribution, NormalDistribution function), 표면의 거칠기에 영향을 받는 halfWay vector
// G(Geometry, GeometryFunc), 에너지 보존법칙 적용
// F(Fresnel-Fresnel equation), 표면 각도에 따른 반사율

// F->D
float NDF_ggxtr(float3 vNormal, float3 vHalfWayVector, float fAlpha) // NormalDistributionGGXTR, (H, halfWay vector), (A, Roughness), (N, Normal)
{
    float a2 = fAlpha * fAlpha;
    float NdotH = saturate(dot(vNormal, vHalfWayVector));
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.f) + 1.f);
    
    denom = PI * denom * denom;
    return (nom / denom);
}

// F->G
// GschlickGGX(n, v, k) 
// k는 이하 두개 조명 조건에 따라 가변적임
// Direct //  Kdir -> ((a + 1) * (a + 1)) / 8 // direct lighting 추천
// IBL Lighting // Kibl -> a * a / 2 // 이미지 기반 조명기법

float Geometry_SchlickGGX(float NdotV, float fK)
{
    return NdotV / (NdotV * (1.0f - fK) + fK);
}

float Geometry_Smith(float3 vNormal, float3 vFromView, float3 vFromLight, float k)
{
    // Geometry Obstruction     
    // 철이 있는, 주로 튀어나온 장애물
    float NdotV = saturate(dot(vNormal, vFromView));
    
    // Geometry Shadowing       
    // 요가 있는, 주로 음푹 들어간 장애물 (그래서 셰도잉)
    float NdotL = saturate(dot(vNormal, vFromLight));

    // 요철이 골고루 있다고 가정하고 적당히 섞음
    return Geometry_SchlickGGX(NdotV, k) * Geometry_SchlickGGX(NdotL, k); 
}

// F -> F
float3 Fresnel_Schlick(float cosTheta, float3 F0)
{
    // 모서리 부분의 반사
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

PBR_LIGHT_OUT PBR_Lighting(
    float3 vNormal, float3 vToView, float3 vToLight,
    float3 vAlbedo, float fMetallic, float fRoughness,
    float3 vLightColor, float fAttenuation, float3 vFO
) {
    PBR_LIGHT_OUT Out;
    Out.vShade = 0;
    Out.vSpecular = 0;
    
    float3 vHalfVector = normalize(vToView + vToLight);
    float NdotL = saturate(dot(vNormal, vToLight));
    float NdotV = saturate(dot(vNormal, vToView));
    if (NdotL <= 0 || NdotV <= 0)
    {
        return Out;
    }
    
    float fAlpha = max(fRoughness * fRoughness, 0.04f);
    float k = ((fRoughness + 1.f) * (fRoughness + 1.f)) / 8.f;
    
    float D = NDF_ggxtr(vNormal, vHalfVector, fAlpha);
    float G = Geometry_Smith(vNormal, vToView, vToLight, k);
    float3 F = Fresnel_Schlick(saturate(dot(vHalfVector, vToView)), vFO);
    
    float3 vNumerator = D * G * F;
    float fDenominator = max(4.f * NdotL * NdotV, 1e-7);
    float3 specularBRDF = vNumerator / fDenominator;
    
    float3 kS = F;
    float3 kD = (1.f - kS) * (1.f -fMetallic);
    
    //Out.vShade = float(1.f, 1.f, 1. 1.f);
    Out.vShade = (kD / PI) * (NdotL * fAttenuation) * vLightColor;
    
    Out.vSpecular = specularBRDF * vLightColor * fAttenuation * NdotL;
    
    return Out;
}

float3 DecodeNormalFromRG(Texture2D NormalMap, SamplerState Samp, float2 uv)
{
    float2 vSampledRG = (NormalMap.Sample(Samp, uv).rg * 2.f - 1.f);

    float z2 = 1.0f - saturate(dot(vSampledRG, vSampledRG));
    float z = sqrt(max(z2, 0.0f));

    float3 n = float3(vSampledRG, z);

    return normalize(n);
}

float4x4 RotateX(float fAngle)
{
    float fCos = cos(radians(fAngle));
    float fSin = sin(radians(fAngle));
    
    float4x4 RotateXMat =
    {
        1,      0,      0,     0,
        0,   fCos,  -fSin,     0,
        0,   fSin,   fCos,     0,
        0,      0,      0,     1,  
    };

    return RotateXMat;
}

float4x4 RotateY(float fAngle)
{
    float fCos = cos(radians(fAngle));
    float fSin = sin(radians(fAngle));
    
    float4x4 RotateYMat =
    {
        fCos  ,    0,   fSin,       0,
        0     ,    1,      0,       0,
        -fSin ,    0,   fCos,       0,
        0     ,    0,      0,       1
    };

    return RotateYMat;
}

float4x4 RotateZ(float fAngle)
{
    float fCos = cos(radians(fAngle));
    float fSin = sin(radians(fAngle));
    
    float4x4 RotateZMat =
    {
        fCos,  -fSin,     0,    0,
        fSin,   fCos,     0,    0,
           0,      0,     1,    0,
           0,      0,     0,    1,
    };

    return RotateZMat;
}

float4x4 RotateAxis(float4 _vAxis , float fAngle)
{
    float flenhth = length(_vAxis);
    if (flenhth < FLT_EPSILON5)
    {
        return float4x4(
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        );
    }
    
    float3 vAxis = normalize(_vAxis);
    
    float fCos = cos(radians(fAngle));
    float fSin = sin(radians(fAngle));
    
    float fX = vAxis.x;
    float fY = vAxis.y;
    float fZ = vAxis.z;
    
    //Rodrigues 공식 기반 
    float4x4 RotateMat =
    float4x4(
        fCos + fX * fX * (1.0 - fCos),               fX * fY * (1.0 - fCos) - fZ * fSin,         fX * fZ * (1.0 - fCos) + fY * fSin,                0.0,
        fY * fX * (1.0 - fCos) + fZ * fSin,          fCos + fY * fY * (1.0 - fCos),              fY * fZ * (1.0 - fCos) - fX * fSin,                0.0,
        fZ * fX * (1.0 - fCos) - fY * fSin,          fZ * fY * (1.0 - fCos) + fX * fSin,         fCos + fZ * fZ * (1.0 - fCos),                     0.0,
        0.0,                                         0.0,                                        0.0,                                               1.0
    );
    
    return RotateMat;

}


#endif // ENGINE_SHADER_FUNCTIONS_HLSLI
