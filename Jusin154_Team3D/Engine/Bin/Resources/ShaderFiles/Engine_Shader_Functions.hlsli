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
            float fDepth = ShadowMap.Sample(BorderOneSampler, uv + vOffset).x;
            fVisibility += (fDepthCenter - bias > fDepth) ? 0.f : 1.f;
        }
    }
    if (abs(fVisibility - 9.f) < FLT_EPSILON3)
    {
        fVisibility = 9.f;
    }
    return fVisibility / 9.f;
}

float GetRimLight(float3 vCamPosition, float3 vPosition, float3 vNormal, float fRimPower, float fRimStrength)
{
    float fRimLight = (1 - dot(normalize(vCamPosition - vPosition), vNormal));
    fRimLight = max(fRimLight, 1e-6f);
    fRimLight = pow(fRimLight, fRimPower);
    fRimLight = fRimLight * fRimStrength;
    return fRimLight;
}

float LinearAttenuation(float fLightRange, float fLightDistance)
{
    return saturate((fLightRange - fLightDistance) / fLightRange);
}

float PowerAttenuation(float fLightRange, float fLightDistance, float fPower)
{
    // 1 -> Linear, 
    // ( 1 > fPower )-> 뾰족하게 감쇄
    // ( 1 < fPower )-> 완만하게 감쇄
    float fValue = saturate((fLightRange - fLightDistance) / fLightRange);
    return pow(fValue, fPower);
}

float2 Get_MovedUV(float2 vOriginalUV, float fDeltaU, float fDeltaV, uint iIndexU, uint iIndexV)
{
    float2 vDelta = float2(fDeltaU, fDeltaV);
    float2 vOffset = float2(iIndexU * fDeltaU, iIndexV * fDeltaV);
    return vOriginalUV * vDelta + vOffset;
}

float2 UV_Cutting(float2 originalUV, float2 uvCuttingCountFloat, int currentFrameIndex)
{
    // (1) 칸 개수는 정수로 취급 (0 방지)
    int uvCuttingCountX = max((int) uvCuttingCountFloat.x, 1);
    int uvCuttingCountY = max((int) uvCuttingCountFloat.y, 1);

    int totalFrameCount = uvCuttingCountX * uvCuttingCountY;

    // (2) currentFrameIndex를 [0, totalFrameCount)로 래핑 (정수 % 제거)
    int wrappedFrameIndex = currentFrameIndex;
    if (totalFrameCount > 0)
    {
        float inverseTotalFrameCount = 1.0f / (float) totalFrameCount;

        // loopCount = floor(currentFrameIndex / totalFrameCount)
        int loopCount = (int) floor((float) currentFrameIndex * inverseTotalFrameCount);

        wrappedFrameIndex = currentFrameIndex - loopCount * totalFrameCount;

        // 음수 프레임까지 방어(선택)
        if (wrappedFrameIndex < 0)
        {
            wrappedFrameIndex += totalFrameCount;
        }
    }
    else
    {
        wrappedFrameIndex = 0;
    }

    // (3) 가로 기준으로 행/열 구하기 (정수 /, % 제거)
    float inverseCuttingCountX = 1.0f / (float) uvCuttingCountX;
    float inverseCuttingCountY = 1.0f / (float) uvCuttingCountY;

    // frameIndexY = floor(wrappedFrameIndex / uvCuttingCountX)
    int frameIndexY = (int) floor((float) wrappedFrameIndex * inverseCuttingCountX);

    // frameIndexX = wrappedFrameIndex - frameIndexY * uvCuttingCountX   (% 대체)
    int frameIndexX = wrappedFrameIndex - frameIndexY * uvCuttingCountX;

    // (4) UV 변환
    float2 resultUV = originalUV;
    resultUV.x = resultUV.x * inverseCuttingCountX + (float) frameIndexX * inverseCuttingCountX;
    resultUV.y = resultUV.y * inverseCuttingCountY + (float) frameIndexY * inverseCuttingCountY;

    return resultUV;
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
    float3 vLightColor, float fLightIntensity, float fAttenuation, float3 vFO
) {
    PBR_LIGHT_OUT Out = (PBR_LIGHT_OUT)0;
    Out.vShade = 0;
    Out.vSpecular = 0;
    float3 vLighting = vLightColor * fLightIntensity;
    
    float3 vHalfVector = normalize(vToView + vToLight);
    float NdotL = saturate(dot(vNormal, vToLight));
    float NdotV = saturate(dot(vNormal, vToView));
    if (NdotL <= 0 || NdotV <= 0)
    {
        Out.vShade = float3(0.f, 0.f, 0.f);
        Out.vSpecular = float3(0.f, 0.f, 0.f);
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
    
    float3 vDiffuseBRDF = (kD * vAlbedo) / PI;
    
    Out.vShade = vDiffuseBRDF * vLighting * NdotL * fAttenuation;
    
    Out.vSpecular = specularBRDF * vLighting * NdotL * fAttenuation;

    return Out;
}
bool CalcLighting(
    in Texture2D SurfaceTexture2D,
    in float fTagDepthBlue,
    in float2 uv,
    in float3 vAlbedo,
    inout float3 vF0,
    inout float fMetallic, 
    inout float fRoughness,
    inout float fOcclusion, 
    inout float fAttenuation)
{
    bool bResult = false;
    if (true == AlmostEqual3(fTagDepthBlue * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_METALNESS)) // Metallic Roughness Occ
    {
        bResult = true;
        float3 vMRO = SurfaceTexture2D.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;
        
        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(fTagDepthBlue * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_SPECULAR)) // Specular Roughness Occ
    {
        bResult = true;
        fMetallic = 0.f;
        
        float3 vSRO = SurfaceTexture2D.Sample(DefaultSampler, uv).rgb;
        vF0 = vSRO.rrr;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
    }
    else if (true == AlmostEqual3(fTagDepthBlue * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_ANISOTROPY)) // Specular Roughness SubSurfaceScattering Occlusion
    {
        bResult = true;
        fMetallic = 0.f;
        
        float4 vSRXO = SurfaceTexture2D.Sample(DefaultSampler, uv);
        vF0 = vSRXO.rrr;
        fRoughness = vSRXO.g;
        fOcclusion = vSRXO.a;
    }
    return bResult;
}

float GetBloomCurve(float x, float fThreshold, uint iMethod)
{
    float fResult = x;
    // float fThreshold = 0.68f * 2.f;
    x *= 2.f;
    
    if (iMethod == 0)
    {
        fResult = x * 0.05f // 최소 기본값 보장
                + max(0, x - fThreshold) // 밝기값 68 이하는 기본값으로하고 초과하면 그 값만큼 더해줌
                * 0.5f; // 0~2로 정규화 되어있기 때문에 강조 값 보정해줌
    }
    else if (iMethod == 1)
    {
        fResult = x * x / 3.2f;
    }
    else if (iMethod == 2)
    {
        fResult = max(0, x - fThreshold);
        fResult = pow(fResult, 3.f);
    }
    
    return fResult * 0.5f;
}

// RG 노말맵에서 Z 벡터 복원
float3 DecodeNormalFromRG(Texture2D NormalMap, SamplerState Samp, float2 uv)
{
    float2 vSampledRG = (NormalMap.Sample(Samp, uv).rg * 2.f - 1.f);

    float z2 = 1.0f - saturate(dot(vSampledRG, vSampledRG));
    float z = sqrt(max(z2, 0.0f));

    float3 n = float3(vSampledRG, z);

    return normalize(n);
}

// BackGround ForeGround 합은 1
float2 DepthCompare(float fCenterDepth, float fSampleDepth, float fDepthScale)
{
    return saturate(0.5f + float2(fDepthScale, -fDepthScale) * (fSampleDepth - fCenterDepth));
}

float2 SpreadCompare(float fOffsetLength, float2 fSpreadLength, float fPixelToSampleUnitsScale)
{
    return saturate(fPixelToSampleUnitsScale * fSpreadLength - fOffsetLength + 1.f);
}

float SampleWeight(float fCenterDepth, float fSampleDepth, float fOffsetLength, float fCenterSpreadLength, float fSampleSpreadLength, float fPixelToSampleUnitsScale, float fDepthScale)
{
    float2 vDepthCompare = DepthCompare(fCenterDepth, fSampleDepth, fDepthScale);
    float2 vSpreadCompare = SpreadCompare(fOffsetLength, float2(fCenterSpreadLength, fSampleSpreadLength), fPixelToSampleUnitsScale);
    return dot(vDepthCompare, vSpreadCompare);
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
    
    float3 vAxis = normalize(_vAxis).xyz;
    
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

float3 UnpackRGB8(int iPacked)
{
    return float3((iPacked >> 16) & 255, (iPacked >> 8) & 255, iPacked & 255) / 255.0f;
}
float3 ColorMixer(float3 vSlotA, float3 vSlotB, float fAToBFactor, int iMethod)
{
    float3 vResult = { 0.f, 0.f, 0.f };
    switch (iMethod)
    {
        case 1:
            vResult = (fAToBFactor) * vSlotA + (1.f - fAToBFactor) * vSlotB;
            break;
        case 0:
            vResult = (fAToBFactor) * vSlotA * (1.f - fAToBFactor) * vSlotB;
            break;
    }
    return vResult;
}


float2 SelectLerpUV(float2 fAmount, float _fRatio, int iSelectOption)
{
    if (iSelectOption < 0)
        return float2(0, 0);
    
    float fRatio = _fRatio;
    switch (iSelectOption)
    {
        case 0:
            fRatio = fRatio; // Linear 
            break;
        case 1:
            fRatio = fRatio * fRatio; // EaseInQuad 후반에 속도 증가
            break;
        case 2:
            fRatio = pow(1 - fRatio, 2); // EaseOutQuad 초반에 속도 증가
            break;
        case 3:
            fRatio = fRatio * fRatio * fRatio; // EaseInCubic  더 강하게 후반 속도 증가
            break;
        case 4:
            fRatio = pow(1 - fRatio, 3); // EaseOutCubic 더 강하게 초반 속도 증가
            break;
        case 5:
            fRatio = 0.5f * (1 - cos(PI * fRatio)); // EaseInOutSin 사인 곡선 
            break;
        case 6:
            fRatio = sin(13 * PI * fRatio) * (1 - fRatio) * (1 - fRatio); // EaseInBack 뒤로갔다가 앞으로
            break;
        case 7:
            fRatio = pow(2, 10 * (fRatio - 1)); // Expo 지수 함수
            break;

    }
    
    return fAmount * fRatio;
}

float3 ReinHard_ToneMapper(float3 vColor)
{
    float k = 1.f;
    vColor = (vColor / (vColor + k));
    return pow(vColor, 1.f / 2.2f);
}

float3 Filmic_ToneMapper(float3 vColor)
{
    const float fA = 2.51f;
    const float fB = 0.03f;
    const float fC = 2.43f;
    const float fD = 0.59f;
    const float fE = 0.14f;

    vColor = saturate(
        vColor * (fA * vColor + fB)
        / (vColor * (fC * vColor + fD) + fE)
    );
    
    return pow(vColor, 1.f / 2.2f);
}


float4 ApplyDissolve(Texture2D DisolveTexture, float fDisolveRatio, float fDisolveAmount, float fDisolveEdgeWidth, float4 vDisolveEdgeColor,  float4 vMtrlDiffuse, float2 vTexcoord)
{
    float4 vDisolve = DisolveTexture.Sample(DefaultSampler, vTexcoord);
    float fDisolveValue = vDisolve.r;
    
    float fDisolveThreshold = saturate(fDisolveRatio + fDisolveAmount);

    clip(fDisolveValue - fDisolveThreshold); // 진짜 잘려나감
    
    float edgeWidth = max(fDisolveEdgeWidth, FLT_EPSILON5); // 불타는 라인
    float edgeFactor = smoothstep(fDisolveThreshold, fDisolveThreshold + edgeWidth, fDisolveValue);

    vMtrlDiffuse.rgb = lerp(vDisolveEdgeColor.rgb, vMtrlDiffuse.rgb, edgeFactor);

    return vMtrlDiffuse;
}

bool IsValidUV(float2 uv)
{
    bool bReturn = true;
    if (uv.x < 0 || uv.x >= 1 || uv.y < 0 || uv.y >= 1)
    {
        bReturn = false;
    }
    return bReturn;
}

float4 BilinearFetches(float2 vGlobalTexelSize, Texture2D SrcTexture2D, float2 vCenterTexCoord, sampler samplerLinear)
{
    float2 uv = vCenterTexCoord;
    float2 vSrcTexelSize = float2(1.f/vGlobalTexelSize.x, 1.f/vGlobalTexelSize.y);
    float fWeight = 0.f;
    
    float3 vCenterColor = float3(0.f, 0.f, 0.f);
    float3 fLTColor = float3(0.f, 0.f, 0.f);
    float3 fRTColor = float3(0.f, 0.f, 0.f);
    float3 fLBColor = float3(0.f, 0.f, 0.f);
    float3 fRBColor = float3(0.f, 0.f, 0.f);
    {
        if (true == IsValidUV(uv)) {
            vCenterColor = SrcTexture2D.SampleLevel(samplerLinear, uv, 0).rgb;
            fWeight += 0.5f;
        }
        uv = vCenterTexCoord + vSrcTexelSize * float2(-1.0, -1.0);
        if (true == IsValidUV(uv)) {
            fLTColor = SrcTexture2D.SampleLevel(samplerLinear, uv, 0).rgb;
            fWeight += 0.125f;
        }
        uv = vCenterTexCoord + vSrcTexelSize * float2(+1.0, -1.0);
        if (true == IsValidUV(uv)) {
            fRTColor = SrcTexture2D.SampleLevel(samplerLinear, uv, 0).rgb;
            fWeight += 0.125f;
        }
        uv = vCenterTexCoord + vSrcTexelSize * float2(-1.0, +1.0);
        if (true == IsValidUV(uv)) {
            fLBColor = SrcTexture2D.SampleLevel(samplerLinear, uv, 0).rgb;
            fWeight += 0.125f;
        }
        uv = vCenterTexCoord + vSrcTexelSize * float2(+1.0, +1.0);
        if (true == IsValidUV(uv)) {
            fRBColor = SrcTexture2D.SampleLevel(samplerLinear, uv, 0).rgb;
            fWeight += 0.125f;
        }
    }

    float3 vFilteredColor = (vCenterColor * 0.5 + (fLTColor + fRTColor + fLBColor + fRBColor) * 0.125) / fWeight;

    return float4(vFilteredColor, 1.0);
}

float4 BlendDiffuse(float4 vDiffuseA, Texture2D DiffuseBlendTexture, float2 vTexcoord, float fRatio)
{
    float4 vDiffuseBlendColor = DiffuseBlendTexture.Sample(DefaultSampler, vTexcoord);
    
    return lerp(vDiffuseA, vDiffuseBlendColor, fRatio);

}
float2 CalcVelocityUV(float4 vCurrentProjPos, float4 vPreviousProjPos, float fIntensity = 1.f)
{
    float2 vReturnVelocity = float2(0.5f, 0.5f);
    if (vCurrentProjPos.w <= FLT_EPSILON5 || vPreviousProjPos.w <= FLT_EPSILON5)
    {
        return vReturnVelocity;
    }

    float2 currentNDC = vCurrentProjPos.xy / vCurrentProjPos.w;
    float2 previousNDC = vPreviousProjPos.xy / vPreviousProjPos.w;

    currentNDC.y *= -1.f;
    previousNDC.y *= -1.f;

    // UV 델타
    float2 velocityUV = (currentNDC - previousNDC) * 0.5f;
    velocityUV *= fIntensity;
    velocityUV = clamp(velocityUV, -1.0f, 1.0f);

    return velocityUV * 0.5f + 0.5f; // 0 속도 -> 0.5
}
float2 CalcVelocityUV(float4 vCurrentProjPos, float4 vPreviousProjPos)
{
    return CalcVelocityUV(vCurrentProjPos, vPreviousProjPos, 1.0f);
}

float3 DownSampleFast(Texture2D SrcTexture2D, float2 vCenterTexcoord, float2 vSrcTexelSize, float2 vResolution)
{
    float2 uv = float2(0.f, 0.f);
    float3 vCenterColor = SrcTexture2D.SampleLevel(BorderZeroSampler, vCenterTexcoord, 0).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(-1.0, -1.0);
    float3 fLTColor = BilinearFetches(vResolution, SrcTexture2D, uv, BorderZeroSampler).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(0.0, -1.0);
    float3 fTColor = SrcTexture2D.SampleLevel(BorderZeroSampler, uv, 0).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(+1.0, -1.0);
    float3 fRTColor = BilinearFetches(vResolution, SrcTexture2D, uv, BorderZeroSampler).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(-1.0, 0.0);
    float3 fLColor = SrcTexture2D.SampleLevel(BorderZeroSampler, uv, 0).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(1.0, 0.0);
    float3 fRColor = SrcTexture2D.SampleLevel(BorderZeroSampler, uv, 0).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(-1.0, +1.0);
    float3 fLBColor = BilinearFetches(vResolution, SrcTexture2D, uv, BorderZeroSampler).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(0.0, 1.0);
    float3 fBColor = SrcTexture2D.SampleLevel(BorderZeroSampler, uv, 0).rgb;
    uv = vCenterTexcoord + vSrcTexelSize * float2(+1.0, +1.0);
    float3 fRBColor = BilinearFetches(vResolution, SrcTexture2D, uv, BorderZeroSampler).rgb;
    
    return vCenterColor * 0.25f + (fLTColor + fRTColor + fLBColor + fRBColor) * 0.0625f + (fTColor + fLColor + fRColor + fBColor) * 0.125f;
}

float4x4 Inverse4x4(float4x4 m)
{
    float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3];
    float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3];
    float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3];
    float a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    // 역행렬이 존재하지 않는 경우 (det ≈ 0)
    if (abs(det) < 1e-6)
        return float4x4(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1); // fallback: 단위행렬

    float invDet = 1.0 / det;

    float4x4 inv;
    inv[0][0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    inv[0][1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    inv[0][2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    inv[0][3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;

    inv[1][0] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    inv[1][1] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    inv[1][2] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    inv[1][3] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;

    inv[2][0] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    inv[2][1] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    inv[2][2] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    inv[2][3] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;

    inv[3][0] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    inv[3][1] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    inv[3][2] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    inv[3][3] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return inv;
}


#endif // ENGINE_SHADER_FUNCTIONS_HLSLI
