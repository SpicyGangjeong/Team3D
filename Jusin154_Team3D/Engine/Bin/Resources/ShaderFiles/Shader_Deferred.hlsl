#include "Engine_Shader_Defines.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_invMatView, g_invmatProj;
matrix g_LightViewMatrix_NEAR, g_LightProjMatrix_NEAR;
matrix g_LightViewMatrix_MIDDLE, g_LightProjMatrix_MIDDLE;
matrix g_LightViewMatrix_FAR, g_LightProjMatrix_FAR;
matrix g_PreShadowLightViewMatrix, g_PreShadowLightProjMatrix;


float4 g_SamplePos[64];
float g_fFar;
float g_fShadowFar_NEAR;
float g_fShadowFar_MIDDDLE;
float g_fShadowFar_FAR;
float g_fCascadeSplitRatioNear;
float g_fCascadeSplitRatioFar;
float2 g_vNearShadowResolution;
float2 g_vMiddleShadowResolution;
float2 g_vFarShadowResolution;
float2 g_vPreShadowResolution;
float2 g_vResolution;
float2 g_vSrcResolution;
float4 g_vShadowBias;

uint g_iBloomEmbossingPass;
float g_fBloomThreshold;
float g_fDepthThreshold;
float g_fFocusDistance;
float g_fFarBlurStart;
float g_fFarBlurEnd;
float g_fDOFBlurMultiplier;
float g_fSSAO_BIAS;
float g_fSSAOStrength;

Texture2D   g_Texture;
Texture2D   g_SSAONoiseTexture;
Texture2D   g_SSAOInputTexture;
uint        g_iKernelSize;
float       g_fSSAORadius;
float2      g_vSSAOTextureSize;

float g_fLightRange;
float g_fLightIntensity;
float g_fSpotInnerAngle;
float g_fSpotOuterAngle;

float g_fToneMappingExposure;
uint g_iToneMappingType;
bool  g_bUsePowerLightAttenuation;
float g_fLightAttenuationPower;

vector g_vLightDir;
vector g_vLightPos;
vector g_vLightPosOffset;

vector g_vCamPosition;


Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_DepthTexture;
Texture2D g_SpecularTexture;
Texture2D g_ShadowNearTexture;
Texture2D g_ShadowMiddleTexture;
Texture2D g_PreShadowTexture;
Texture2D g_BlurTexture;
Texture2D g_BlurXTexture;

Texture2D g_BlurWeightTexture;
Texture2D g_BlurWeightXTexture;

Texture2D g_SurfaceTexture;
Texture2D g_OriginalTexture;

Texture2D g_ColorTexture;
Texture2D g_VelocityTexture;
Texture2D g_TileVelocityTexture;

int g_iMBSampleCount;
int g_iMBMaxSampleCount;
float g_fMBBlurRadius;
int g_iMBType;
int g_iMBTileSize;
float g_fMBSampleBias;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

float g_fFogDensity;
float g_fFogPow;
bool  g_bFogVisible;
vector g_vFogColor;

float g_fWeights_32[32] =
{
    0.000477, 0.000888, 0.001585, 0.002720, 0.004485, 0.007105, 0.010814, 0.015813,
    0.022216, 0.029988, 0.038893, 0.048463, 0.058021, 0.066740, 0.073759, 0.078320,
    0.079902, 0.078320, 0.073759, 0.066740, 0.058021, 0.048463, 0.038893, 0.029988,
    0.022216, 0.015813, 0.010814, 0.007105, 0.004485, 0.002720, 0.001585, 0.000888
};
float g_fWeight_3x3_UPSAMPLE[3][3] =
{
    { 0.0625f, 0.125f, 0.0625f },
    { 0.125f, 0.250f, 0.125f },
    { 0.0625f, 0.125f, 0.0625f }
};

float g_fWeights_64[64] =
{
    0.0004, 0.0014, 0.0033, 0.0053, 0.0053, 0.0033, 0.0014, 0.0004,
    0.0014, 0.0045, 0.0105, 0.0171, 0.0171, 0.0105, 0.0045, 0.0014,
    0.0033, 0.0105, 0.0245, 0.0400, 0.0400, 0.0245, 0.0105, 0.0033,
    0.0053, 0.0171, 0.0400, 0.0654, 0.0654, 0.0400, 0.0171, 0.0053,
    0.0053, 0.0171, 0.0400, 0.0654, 0.0654, 0.0400, 0.0171, 0.0053,
    0.0033, 0.0105, 0.0245, 0.0400, 0.0400, 0.0245, 0.0105, 0.0033,
    0.0014, 0.0045, 0.0105, 0.0171, 0.0171, 0.0105, 0.0045, 0.0014,
    0.0004, 0.0014, 0.0033, 0.0053, 0.0053, 0.0033, 0.0014, 0.0004
};


float g_fWeights_128[128] =
{
    0.000209902, 0.000241594, 0.000277451, 0.000317919, 0.000363477, 0.000414637, 0.000471944, 0.000535974,
    0.000607333, 0.000686659, 0.000774615, 0.000871889, 0.00097919, 0.00109725, 0.00122679, 0.00136858,
    0.00152334, 0.00169183, 0.00187477, 0.00207285, 0.00228675, 0.0025171, 0.00276448, 0.0030294,
    0.00331231, 0.00361356, 0.00393342, 0.00427205, 0.00462949, 0.00500564, 0.0054003, 0.00581308,
    0.00624346, 0.00669075, 0.00715411, 0.0076325, 0.00812472, 0.0086294, 0.009145, 0.0096698,
    0.0102019, 0.0107393, 0.0112798, 0.0118211, 0.0123608, 0.0128962, 0.0134249, 0.0139441,
    0.0144511, 0.0149431, 0.0154174, 0.0158713, 0.0163021, 0.0167073, 0.0170844, 0.0174311,
    0.0177452, 0.0180246, 0.0182676, 0.0184726, 0.0186383, 0.0187635, 0.0188474, 0.0188895,

    0.0188895, 0.0188474, 0.0187635, 0.0186383, 0.0184726, 0.0182676, 0.0180246, 0.0177452,
    0.0174311, 0.0170844, 0.0167073, 0.0163021, 0.0158713, 0.0154174, 0.0149431, 0.0144511,
    0.0139441, 0.0134249, 0.0128962, 0.0123608, 0.0118211, 0.0112798, 0.0107393, 0.0102019,
    0.0096698, 0.009145, 0.0086294, 0.00812472, 0.0076325, 0.00715411, 0.00669076, 0.00624346,
    0.00581308, 0.0054003, 0.00500564, 0.00462948, 0.00427205, 0.00393342, 0.00361356, 0.00331231,
    0.0030294, 0.00276448, 0.0025171, 0.00228675, 0.00207285, 0.00187477, 0.00169183, 0.00152334,
    0.00136858, 0.00122679, 0.00109725, 0.000979191, 0.000871889, 0.000774615, 0.000686658, 0.000607333,
    0.000535973, 0.000471944, 0.000414637, 0.000363477, 0.000317919, 0.000277451, 0.000241594, 0.000209902
};
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_VELOCITYBLUR
{
    float4 vColor : SV_TARGET0;
    float2 vVelocity : SV_TARGET1;
};
struct PS_OUT_VELOCITYTILE
{
    float2 vVelocity : SV_TARGET0;
};
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}
VS_OUT VS_CAPTURE(VS_IN In)
{
    VS_OUT Out;

    float2 vClipPos = In.vPosition.xy * 2.0f; // → -1.0 ~ +1.0

    Out.vPosition = float4(vClipPos, 0.0f, 1.0f);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}
VS_OUT VS_TONE_MAPPING(VS_IN In)
{
    VS_OUT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_BACKBUFFER
{
    float4 vBackBuffer  : SV_TARGET0;
    float4 vEnvironment : SV_Target1;
};
struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};
struct PS_OUT_FLT4_SINGLE
{
    float4 vFirstTarget : SV_TARGET0;
};
struct PS_OUT_FLT4_DOUBLE
{
    float4 vFirstTarget : SV_TARGET0;
    float4 vSecondTarget : SV_TARGET1;
};
struct PS_OUT_BLUR_X
{
    float4 vBlurX : SV_TARGET0;
    float4 vBlurWeight : SV_TARGET1;
};
struct PS_OUT_SSAO_AMBIENT_OCCLUSION
{
    float fOcclusion : SV_TARGET0;
};
struct PS_OUT_SSAO_BLUR
{
    float fBlur : SV_TARGET0;
};
// g_ColorTexture;
// g_DepthTexture;
// g_VelocityTexture;
// g_DiffuseTexture;
// g_fFar
// g_fMBSampleBias
// g_fMBBlurRadius
// g_iMBSampleCount
// g_iMBMaxSampleCount
// g_iMBType
PS_OUT_VELOCITYBLUR PS_MOTIONBLUR(PS_IN In)
{
    PS_OUT_VELOCITYBLUR Out = (PS_OUT_VELOCITYBLUR) 0;
    
    float2 vCenterUV = In.vTexcoord;
    float2 vFullResolutionTexelSize = 1.f / g_vResolution;
    float fCenterDepth = g_DepthTexture.Sample(PointSampler, vCenterUV).y;
    float2 vCenterVelo = g_VelocityTexture.Sample(PointSampler, vCenterUV).xy * 2.f - 1.f; // 0~1 -> -1~+1
    float2 vCenterVeloPixel = vCenterVelo * g_vResolution.xy;
    bool bBorrowedVelocityFromTile = false;
    
    float2 vBlurVelo = vCenterVelo;
    float2 vBlurVeloPixel = vCenterVeloPixel;
    if (length(vBlurVeloPixel) < 0.5f) {
        vBlurVelo = g_TileVelocityTexture.Sample(PointSampler, vCenterUV).xy * 2.f - 1.f;
        vBlurVeloPixel = vBlurVelo * g_vResolution.xy;
        bBorrowedVelocityFromTile = true;
    }
    
    if (length(vBlurVeloPixel) < 0.5f) {
        Out.vColor = g_ColorTexture.Sample(ClampLinearSampler, vCenterUV);
        Out.vVelocity = float2(0.5f, 0.5f);
        return Out;
    }
    
    float2 vBlurDirPixel = (vBlurVeloPixel / max(length(vBlurVeloPixel), FLT_EPSILON5));
    int iMaxRadius = min(g_iMBSampleCount, g_iMBMaxSampleCount);
    
     // 샘플링 당 이동 픽셀
    float fSamplePixelSpeed = g_fMBBlurRadius / max((float) iMaxRadius, 1.f);
    
    // 샘플유닛의 크기
    float fScalePixelToSampleUnit = (float) iMaxRadius / max(g_fMBBlurRadius, FLT_EPSILON5);
    float fScaleDepth = 0.5f / max(g_fMBSampleBias, FLT_EPSILON5);
    float4 vAccColorSum = float4(0.f, 0.f, 0.f, 0.f);
    float2 vAccVeloSum = float2(0.f, 0.f);
    float fAccWeight = 0.f;
    
    // 센터의 스프레드 길이
    float2 vReferenceVelocityPixel = vCenterVeloPixel;
    if (true == bBorrowedVelocityFromTile)
    {
        vReferenceVelocityPixel = vBlurVeloPixel;
    }
    float fCenterSpreadLengthPixel = clamp(abs(dot(vReferenceVelocityPixel, vBlurDirPixel)), 0.f, g_fMBBlurRadius);

    [loop]
    for (int iInterval = -iMaxRadius; iInterval <= iMaxRadius; ++iInterval) {
        float fOffset = (float) abs(iInterval);
        float2 vOffsetPixel = vBlurDirPixel * ((float) iInterval * fSamplePixelSpeed);
        float2 vSamplingUV = vCenterUV + vOffsetPixel * vFullResolutionTexelSize;
        if (false == IsValidUV(vSamplingUV)) {
            continue;
        }

        float fSampledDepth = g_DepthTexture.Sample(PointSampler, vSamplingUV).y;
        float2 vSampledVelo = g_VelocityTexture.Sample(PointSampler, vSamplingUV).xy * 2.f - 1.f;
        float2 vSampledVeloPixel = vSampledVelo * g_vResolution.xy;

        // 샘플의 스프레드 길이
        float fBlurSampleAccurate = abs(dot(vSampledVeloPixel, vBlurDirPixel));
        float fSampleSpreadLengthPixel = clamp(fBlurSampleAccurate, 0.f, g_fMBBlurRadius);
        if (true == bBorrowedVelocityFromTile)
        {
            fSampleSpreadLengthPixel = max(fSampleSpreadLengthPixel, fCenterSpreadLengthPixel);
        }

        // 샘플과 센터의 (깊이테스트 + 스프레드테스트)
        float fWeightCalculated = SampleWeight(fCenterDepth, fSampledDepth, fOffset, fCenterSpreadLengthPixel, fSampleSpreadLengthPixel, fScalePixelToSampleUnit, fScaleDepth);
        
        if (fWeightCalculated <= 0.f) {
            continue;
        }
        
        float4 vSampledColor = g_ColorTexture.Sample(ClampLinearSampler, vSamplingUV);
        vAccColorSum += vSampledColor * fWeightCalculated;
        vAccVeloSum += vSampledVelo * fWeightCalculated;
        fAccWeight += fWeightCalculated;
    }
    float4 vBlurredColor = vAccColorSum / max(fAccWeight, FLT_EPSILON5);
    float2 vBlurredVelo = vAccVeloSum / max(fAccWeight, FLT_EPSILON5);
    
    if (length(vAccVeloSum) < FLT_EPSILON5)
    {
        Out.vColor = g_ColorTexture.Sample(ClampLinearSampler, vCenterUV);
        Out.vVelocity = float2(0.5f, 0.5f);
        return Out;
    }
    Out.vColor = vBlurredColor;
    Out.vVelocity = vBlurredVelo * 0.5f + 0.5f;
    return Out;
}
PS_OUT_VELOCITYTILE PS_MOTIONBLUR_TILESAMPLE(PS_IN In)
{
    PS_OUT_VELOCITYTILE Out = (PS_OUT_VELOCITYTILE) 0;
    
    float2 uv = In.vTexcoord;
    float2 vSrcTexelSize = float2(1.f / g_vSrcResolution.x, 1.f / g_vSrcResolution.y);
    
    int2 vTilePixelIndex = int2(uv * g_vResolution);
    vTilePixelIndex = clamp(vTilePixelIndex, int2(0, 0), int2(g_vResolution) - 1);
    
    int2 vTileStartSourcePixel = vTilePixelIndex * g_iMBTileSize;
    
    float2 vMaxVel = float2(0.5f, 0.5f);
    float fMaxSpeed = 0.f;
    float2 vSampleVel;
    float2 vSampleUV;
    float2 vSampleDiff;
    float fSampleSpeed;
    
    [loop]
    for (int iY = 0; iY < g_iMBTileSize; ++iY)
    {
        [loop]
        for (int iX = 0; iX < g_iMBTileSize; ++iX)
        {
            int2 vSourcePixel = vTileStartSourcePixel + int2(iX, iY);

            if (vSourcePixel.x < 0 || vSourcePixel.y < 0 || vSourcePixel.x >= (int) g_vSrcResolution.x || vSourcePixel.y >= (int) g_vSrcResolution.y)
            {
                continue;
            }

            float2 vSourceUV = (float2(vSourcePixel) + 0.5f) * vSrcTexelSize;

            vSampleVel = g_VelocityTexture.SampleLevel(ClampSampler, vSourceUV, 0).xy;

            vSampleUV = vSampleVel * 2.0f - 1.0f;

            fSampleSpeed = length(vSampleUV * g_vSrcResolution);

            if (fSampleSpeed > fMaxSpeed)
            {
                fMaxSpeed = fSampleSpeed;
                vMaxVel = vSampleVel;
            }
        }
    }

    Out.vVelocity = vMaxVel;
    return Out;
}
PS_OUT_VELOCITYTILE PS_MOTIONBLUR_TENTSAMPLE(PS_IN In)
{
    PS_OUT_VELOCITYTILE Out = (PS_OUT_VELOCITYTILE) 0;
    
    float2 uv = In.vTexcoord;
    float2 vSrcTexelSize = float2(1.f / g_vResolution.x, 1.f / g_vResolution.y);
    float2 vMaxVel = float2(0.5f, 0.5f);
    float vMaxSpeed = 0.f;
    float2 vSampleVel;
    float2 vSampleUV;
    float2 vSampleDiff;
    float fSampleSpeed;
    
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            vSampleUV.x = uv.x + vSrcTexelSize.x * i;
            vSampleUV.y = uv.y + vSrcTexelSize.y * j;
            vSampleVel = g_VelocityTexture.Sample(ClampSampler, vSampleUV);
            vSampleDiff = vSampleVel *2.f - 1.f;
            fSampleSpeed = length(vSampleDiff * g_vResolution * g_iMBTileSize);
            if (fSampleSpeed > vMaxSpeed) {
                vMaxSpeed = fSampleSpeed;
                vMaxVel = vSampleVel;
            }
        }
    }
    Out.vVelocity = vMaxVel;
    return Out;
}
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = 0.f;
    Out.vSpecular = 0.f;
    
    float2 uv = In.vTexcoord;
    
    float3 vAlbedo                  = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    float3 vNormal                  = normalize(g_NormalTexture.Sample(DefaultSampler, uv).xyz * 2.f - 1.f);
    float4 vDepth                   = g_DepthTexture.Sample(DefaultSampler, uv);
    float fSSAO_AmbientOcclusion    = g_SSAOInputTexture.Sample(DefaultSampler, uv).r;
    
    float fViewZ = vDepth.y * g_fFar;
    
    float4 vWorldPosition; // 월드 복원
    {
        vWorldPosition.x = uv.x * 2 - 1;
        vWorldPosition.y = uv.y * -2 + 1;
        vWorldPosition.z = vDepth.x;
        vWorldPosition.w = 1.f;
        vWorldPosition *= fViewZ;
        vWorldPosition = mul(vWorldPosition, g_invmatProj);
        vWorldPosition = mul(vWorldPosition, g_invMatView);
    }
    
    float3 vF0 = float3(0.04f, 0.04f, 0.04f);
    float fMetallic = 0.f;
    float fRoughness = 0.5f;
    float fOcclusion = 1.f;
    float fAttenuation = 1.f;

    float fCameraDistance = length(g_vCamPosition.xyz - vWorldPosition.xyz);
    float3 vToView = normalize(g_vCamPosition.xyz - vWorldPosition.xyz); // 픽셀에서 카메라로
    float3 vToLight = normalize(-g_vLightDir.xyz); // 픽셀에서 라이트로
    
    if (false == CalcLighting(g_SurfaceTexture, vDepth.b, uv, vAlbedo, vF0, fMetallic, fRoughness, fOcclusion, fAttenuation))
    {
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir.xyz) * -1.f, vNormal), 0.f) + (fSSAO_AmbientOcclusion * g_vLightAmbient * g_vMtrlAmbient));
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f);
        Out.vSpecular.a = 0.f;
        return Out;
    }
    
    float fTotalOcclusion = saturate(fOcclusion * fSSAO_AmbientOcclusion);
    
    { // 수치조정 // 디렉셔널
        float fMinRoughness = 0.05f;
        fRoughness = max(fRoughness, fMinRoughness); // 러프니스 최소값 보장
        fRoughness = saturate(saturate(fCameraDistance * 0.01f) * 0.1f + fRoughness); // 거리기반 러프니스 ( 대충 멀수록 반짝반짝한건 더 뭉개져서 표현 )
        vF0 = min(vF0, 0.9f); // 메탈릭 상한선
    }
    
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vToView, vToLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, g_fLightIntensity, fAttenuation, vF0);
    PBR_Out.vShade *= fTotalOcclusion;
    
    float3 vAmbient = g_vLightAmbient.rgb * fTotalOcclusion * fAttenuation;
    float3 vFinalDiffuse = PBR_Out.vShade + vAmbient;
    float3 vFinalSpecular = PBR_Out.vSpecular * g_vLightSpecular.rgb;
    
    Out.vShade = float4(vFinalDiffuse, 1.f);
    Out.vSpecular = float4(vFinalSpecular, 0.f);
    
    return Out;
}
PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = 0.f;
    Out.vSpecular = 0.f;
    
    float2 uv = In.vTexcoord;
    
    float3 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    float3 vNormal = normalize(g_NormalTexture.Sample(DefaultSampler, uv).xyz * 2.f - 1.f);
    float4 vDepth = g_DepthTexture.Sample(DefaultSampler, uv);
    
    float fViewZ = vDepth.y * g_fFar;
    
    float4 vWorldPosition;
    {
        vWorldPosition.x = uv.x * 2 - 1;
        vWorldPosition.y = uv.y * -2 + 1;
        vWorldPosition.z = vDepth.x;
        vWorldPosition.w = 1.f;
        vWorldPosition *= fViewZ;
        vWorldPosition = mul(vWorldPosition, g_invmatProj);
        vWorldPosition = mul(vWorldPosition, g_invMatView);
    }
    
    float3 vF0 = float3(0.04f, 0.04f, 0.04f);
    float fMetallic = 0.f;
    float fRoughness = 0.5f;
    float fOcclusion = 1.f;
    float fAttenuation = 1.f;
    
    float3 vFinalLightPosition = g_vLightPos.xyz + g_vLightPosOffset.xyz;
    
    float fCameraDistance   = length(g_vCamPosition.xyz     - vWorldPosition.xyz);
    float fLightDistance    = length(vFinalLightPosition - vWorldPosition.xyz);
    float3 vToView          = normalize(g_vCamPosition.xyz  - vWorldPosition.xyz); // 픽셀에서 카메라로
    float3 vToLight         = normalize(vFinalLightPosition - vWorldPosition.xyz); // 픽셀에서 라이트로
    float fSSAO_AmbientOcclusion = g_SSAOInputTexture.Sample(DefaultSampler, uv).r;
    
    if (g_bUsePowerLightAttenuation)
    {
        fAttenuation = PowerAttenuation(g_fLightRange, fLightDistance, g_fLightAttenuationPower);
    }
    else
    {
        fAttenuation = LinearAttenuation(g_fLightRange, fLightDistance);
    }
    
    
    if (fAttenuation <= 0.f)
    {
        Out.vShade = float4(0.f, 0.f, 0.f, 1.f);
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }
    
    if (false == CalcLighting(g_SurfaceTexture, vDepth.b, uv, vAlbedo, vF0, fMetallic, fRoughness, fOcclusion, fAttenuation))
    {
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir.xyz) * -1.f, vNormal), 0.f) + (fSSAO_AmbientOcclusion * g_vLightAmbient * g_vMtrlAmbient));
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f);
        Out.vSpecular.a = 0.f;
        return Out;
    }
    
    float fTotalOcclusion = saturate(fOcclusion * fSSAO_AmbientOcclusion);
    
    { // 수치조정 // 점광원
        float fMinRoughness = 0.05f;
        fRoughness = max(fRoughness, fMinRoughness); // 러프니스 최소값 보장
        fRoughness = saturate(saturate(fCameraDistance * 0.01f) * 0.1f + fRoughness); // 거리기반 러프니스 ( 대충 멀수록 반짝반짝한건 더 뭉개져서 표현 )
        vF0 = min(vF0, 0.9f); // 메탈릭 상한선
    }
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vToView, vToLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, g_fLightIntensity, fAttenuation, vF0);
    
    PBR_Out.vShade *= fTotalOcclusion;

    float3 vAmbient = g_vLightAmbient.rgb * fTotalOcclusion * fAttenuation;
    float3 vFinalDiffuse = PBR_Out.vShade + vAmbient;
    float3 vFinalSpecular = PBR_Out.vSpecular * g_vLightSpecular.rgb;

    Out.vShade = float4(vFinalDiffuse, 1.f);
    Out.vSpecular = float4(vFinalSpecular, 0.f);
    
    return Out;
}
PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = 0.f;
    Out.vSpecular = 0.f;
    
    float2 uv = In.vTexcoord;
    
    float3 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    float3 vNormal = normalize(g_NormalTexture.Sample(DefaultSampler, uv).xyz * 2.f - 1.f);
    float4 vDepth = g_DepthTexture.Sample(DefaultSampler, uv);
    float fSSAO_AmbientOcclusion = g_SSAOInputTexture.Sample(DefaultSampler, uv).r;
    
    float fViewZ = vDepth.y * g_fFar;
    
    float4 vWorldPosition; // 월드 복원
    {
        vWorldPosition.x = uv.x * 2 - 1;
        vWorldPosition.y = uv.y * -2 + 1;
        vWorldPosition.z = vDepth.x;
        vWorldPosition.w = 1.f;
        vWorldPosition *= fViewZ;
        vWorldPosition = mul(vWorldPosition, g_invmatProj);
        vWorldPosition = mul(vWorldPosition, g_invMatView);
    }
    
    float3 vF0 = float3(0.04f, 0.04f, 0.04f);
    float fMetallic = 0.f;
    float fRoughness = 0.5f;
    float fOcclusion = 1.f;
    float fAttenuation = 1.f;
    
    float fCameraDistance = length(g_vCamPosition.xyz - vWorldPosition.xyz);
    float fLightDistance = length(g_vLightPos.xyz - vWorldPosition.xyz);
    float3 vToView = normalize(g_vCamPosition.xyz - vWorldPosition.xyz); // 픽셀에서 카메라로
    float3 vToLight = normalize(g_vLightPos.xyz - vWorldPosition.xyz); // 픽셀에서 라이트로
    float3 vLightToPixel = -vToLight;
    
    
    if (g_bUsePowerLightAttenuation)
    {
        fAttenuation = PowerAttenuation(g_fLightRange, fLightDistance, g_fLightAttenuationPower);
    }
    else
    {
        fAttenuation = LinearAttenuation(g_fLightRange, fLightDistance);
    }
    
    
    if (fAttenuation <= 0.f)
    {
        Out.vShade = float4(0.f, 0.f, 0.f, 1.f);
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }
    
    // 각도 기반 감쇠
    float3 vLightDir = normalize(g_vLightDir.xyz); 
    float fCosAngle = dot(vLightDir, vLightToPixel);

    if (fCosAngle < g_fSpotOuterAngle)
    {
        Out.vShade = float4(0.f, 0.f, 0.f, 1.f);
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }
    
    float fSpotAttenenuation = saturate((fCosAngle - g_fSpotInnerAngle) / (g_fSpotInnerAngle - g_fSpotOuterAngle));
    fSpotAttenenuation *= fSpotAttenenuation;
    fAttenuation *= fSpotAttenenuation;
    
    if (false == CalcLighting(g_SurfaceTexture, vDepth.b, uv, vAlbedo, vF0, fMetallic, fRoughness, fOcclusion, fAttenuation))
    {
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir.xyz) * -1.f, vNormal), 0.f) + (fSSAO_AmbientOcclusion * g_vLightAmbient * g_vMtrlAmbient));
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f);
        Out.vSpecular.a = 0.f;
        return Out;
    }
    
    float fTotalOcclusion = saturate(fOcclusion * fSSAO_AmbientOcclusion);
    
    { // 수치조정 // 점광원
        float fMinRoughness = 0.05f;
        fRoughness = max(fRoughness, fMinRoughness); // 러프니스 최소값 보장
        fRoughness = saturate(saturate(fCameraDistance * 0.01f) * 0.1f + fRoughness); // 거리기반 러프니스 ( 대충 멀수록 반짝반짝한건 더 뭉개져서 표현 )
        vF0 = min(vF0, 0.9f); // 메탈릭 상한선
    }
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vToView, vToLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, g_fLightIntensity, fAttenuation, vF0);
    
    PBR_Out.vShade *= fTotalOcclusion;
    
    float3 vAmbient = g_vLightAmbient.rgb * fTotalOcclusion * fAttenuation;
    float3 vFinalDiffuse = PBR_Out.vShade + vAmbient;
    float3 vFinalSpecular = PBR_Out.vSpecular * g_vLightSpecular.rgb;
    
    Out.vShade = float4(vFinalDiffuse, 1.f);
    Out.vSpecular = float4(vFinalSpecular, 0.f);
    
    return Out;
}
PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vEnvironment = float4(0.f, 0.f, 0.f, 0.f);
    return Out;

};

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (0.f == vDiffuse.a) { discard; }
    
    float4 vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    vSpecular.a = 0.f;
    Out.vBackBuffer = vDiffuse * vShade + vSpecular;
    
    
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * g_fFar;
    
    
    float4 vPosition, vPreShadowPosition;
    
    /* (로컬위치 * 월드 * 뷰 * 투영 / w) -> (로컬위치 * 월드)   */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    vPosition = vPosition * fViewZ;
    
    vPosition = mul(vPosition, g_invmatProj);
    vPosition = mul(vPosition, g_invMatView);
    vPreShadowPosition = vPosition;
    
    /* (로컬위치 * 월드) -> (로컬위치 * 월드 * 광원의 뷰 * 광원의 투영 ) */
    float4 vNearShadowPos = vPosition;
    float4 vMiddleShadowPos = vPosition;
    float4 vFarShadowPos = vPosition;
    {
        vNearShadowPos = mul(vNearShadowPos, g_LightViewMatrix_NEAR);
        vNearShadowPos = mul(vNearShadowPos, g_LightProjMatrix_NEAR);
        vMiddleShadowPos = mul(vMiddleShadowPos, g_LightViewMatrix_MIDDLE);
        vMiddleShadowPos = mul(vMiddleShadowPos, g_LightProjMatrix_MIDDLE);
        vFarShadowPos = mul(vFarShadowPos, g_LightViewMatrix_FAR);
        vFarShadowPos = mul(vFarShadowPos, g_LightProjMatrix_FAR);
        vPreShadowPosition = mul(vPreShadowPosition, g_PreShadowLightViewMatrix);
        vPreShadowPosition = mul(vPreShadowPosition, g_PreShadowLightProjMatrix);
    }
    /* 광원의 NDC에서 샘플링 */
    float fVisibility_Dynamic_Near = ShadowVisibility_hwPCF(g_ShadowNearTexture, vNearShadowPos, g_vNearShadowResolution, g_vShadowBias.x);
    float fVisibility_Dynamic_Middle = ShadowVisibility_hwPCF(g_ShadowMiddleTexture, vMiddleShadowPos, g_vMiddleShadowResolution, g_vShadowBias.y);
    float fVisibility_Static = ShadowVisibility_hwPCF(g_PreShadowTexture, vPreShadowPosition, g_vPreShadowResolution, g_vShadowBias.w);
    
////////////////////////////
    // 케스케이드
    float fDepthRatio = saturate(vDepthDesc.y);

    // 경계 블렌딩 폭. 경계 섞는 비중
    float fShadowCascadeBlendWidthRatio = 0.02f; // 2% 섞음

    // 경계 주변 부드럽게 할 비중
    float fCascadeBlend_NearToMiddle = smoothstep(g_fCascadeSplitRatioNear - fShadowCascadeBlendWidthRatio,
        g_fCascadeSplitRatioNear + fShadowCascadeBlendWidthRatio, fDepthRatio);
    //float fCascadeBlend_MiddleToFar = smoothstep(g_fCascadeSplitRatioFar - fShadowCascadeBlendWidthRatio,
    //    g_fCascadeSplitRatioFar + fShadowCascadeBlendWidthRatio, fDepthRatio);

    // Near -> Middle
    float fVisibilityDynamic = lerp(fVisibility_Dynamic_Near, fVisibility_Dynamic_Middle, fCascadeBlend_NearToMiddle);
    
    // 1번째 방법
    // Dynamic->Static 부드럽게 전환
    //float fVisibilityCombinedLogical = min(fVisibilityDynamic, fVisibility_Static);
    //float fStaticShadowBlendStartRatio = g_fCascadeSplitRatioFar;
    //float fStaticShadowBlendWidthRatio = 0.05f;
    //float fStaticShadowBlendEndRatio = min(1.0f, fStaticShadowBlendStartRatio + fStaticShadowBlendWidthRatio);
    //float fStaticShadowBlendWeight = smoothstep(fStaticShadowBlendStartRatio, fStaticShadowBlendEndRatio, fDepthRatio);
    //float fVisibilityCombined = lerp(fVisibilityDynamic, fVisibilityCombinedLogical, fStaticShadowBlendWeight);
    
    // 2번째 방법
    // Dynamic * Static 항상 최소곱
    //float fVisibilityCombined = saturate(fVisibilityDynamic * fVisibility_Static);
    
    // 3번째 방법
    // Dynamic , Static 최소값만 취함
    float fVisibilityCombined = min(fVisibilityDynamic, fVisibility_Static);

    // 최소 밝기
    float fMinShadowBrightness = 0.25f;
    float fShadowMultiplier = lerp(fMinShadowBrightness, 1.0f, saturate(fVisibilityCombined));

    Out.vBackBuffer.rgb *= fShadowMultiplier;
    Out.vEnvironment = Out.vBackBuffer;
    
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_UPSAMPLE(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    float3 vColor = float3(0.f, 0.f, 0.f);
    float fWeight = 0.f;
    float2 uv = In.vTexcoord;
    float2 vTexcelSize = float2(1.f / g_vResolution.x, 1.f / g_vResolution.y);
    
    for (int iRow = -1; iRow < 2; ++iRow) {
        for (int iCol = -1; iCol < 2; ++iCol) {
            uv = In.vTexcoord + vTexcelSize * float2(iRow, iCol);
            if (true == IsValidUV(uv)) {
                vColor  += g_fWeight_3x3_UPSAMPLE[1 + iRow][1 + iCol] * g_DiffuseTexture.SampleLevel(BorderZeroLinearSampler, uv, 0);
                fWeight += g_fWeight_3x3_UPSAMPLE[1 + iRow][1 + iCol];
            }
        }
    }
    vColor /= fWeight;
    Out.vFirstTarget = float4(vColor, 1.f);
    return Out;
}
PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_BLUR_X(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    float4 vSample = float4(0.f, 0.f, 0.f, 1.f);
    float fWeight = 0.f;
    float2 uv = float2(0.f, 0.f);
    for (int i = -15; i < 16; ++i)
    {
        uv.x = In.vTexcoord.x + ((float) i / g_vResolution.x);
        uv.y = In.vTexcoord.y;
        if (false == IsValidUV(uv)) {
            continue;
        }
        
        vSample += g_fWeights_32[i + 15] * g_DiffuseTexture.Sample(BorderZeroSampler, uv);
        fWeight += g_fWeights_32[i + 15];
    }
    if (fWeight <= FLT_EPSILON7)
    {
        Out.vFirstTarget = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    Out.vFirstTarget = (vSample / fWeight);
    return Out;
}
PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_BLUR_COMBINE(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    float4 vSample = float4(0.f, 0.f, 0.f, 1.f);
    float fWeight = 0.f;
    float2 uv = float2(0.f, 0.f);
    for (int i = -15; i < 16; ++i)
    {
        uv.x = In.vTexcoord.x;
        uv.y = In.vTexcoord.y + ((float) i / g_vResolution.y);
        
        if (false == IsValidUV(uv))
        {
            continue;
        }
        
        vSample += g_fWeights_32[i + 15] * g_BlurXTexture.Sample(BorderZeroSampler, uv);
        fWeight += g_fWeights_32[i + 15];
    }
    if (fWeight <= FLT_EPSILON7)
    {
        Out.vFirstTarget = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    Out.vFirstTarget = (vSample / fWeight);
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_EMBOSS(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    float2 uv = In.vTexcoord;
    float fSrcAlpha = g_DiffuseTexture.Sample(PointSampler, uv).a;
    float2 vSrcTexelSize = float2(1.f / g_vSrcResolution.x, 1.f / g_vSrcResolution.y);
    float3 vColor = (0.f, 0.f, 0.f);
    
    vColor = DownSampleFast(g_DiffuseTexture, In.vTexcoord, vSrcTexelSize, g_vSrcResolution);
    
    float fIntensity = dot(vColor, float3(0.2126f, 0.7152f, 0.0722f));
    fIntensity = max(FLT_EPSILON3, fIntensity);

    
    float fBloomIntensity = GetBloomCurve(fIntensity, g_fBloomThreshold, g_iBloomEmbossingPass);
    float3 bloomColor = (vColor * fBloomIntensity) / fIntensity;
    Out.vFirstTarget = float4(bloomColor, fSrcAlpha);
    
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_DOWNSAMPLE(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    float2 uv = In.vTexcoord;
    float fSrcAlpha = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0).a;
    float2 vSrcTexelSize = float2(1.f / g_vSrcResolution.x, 1.f / g_vSrcResolution.y);
    float3 vColor = (0.f, 0.f, 0.f);
    
    vColor = DownSampleFast(g_DiffuseTexture, In.vTexcoord, vSrcTexelSize, g_vSrcResolution);
    
    Out.vFirstTarget = float4(vColor, fSrcAlpha);
    
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_ACCUM(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    float4 vColorSrcA = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    float4 vColorSrcB = g_BlurTexture.Sample(BorderZeroSampler, In.vTexcoord);
    
    Out.vFirstTarget = saturate(vColorSrcA + vColorSrcB);
    
    return Out;
}
PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_FINISH(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    vector vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    float4 vBloom = g_BlurTexture.Sample(BorderZeroSampler, In.vTexcoord);
    Out.vFirstTarget = vColor;
    Out.vFirstTarget += vBloom;
    
    return Out;
}
PS_OUT_BLUR_X PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -15; i < 16; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_vResolution.x;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights_32[i + 15] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor;
    Out.vBlurWeight = 0.f;
    
    return Out;
}
PS_OUT_BLUR_X PS_MAIN_BLUR_X_ENV(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float4 vDepthDesc = float4(0.f, 0.f, 0.f, 1.f);
    float3 vColor = float3(0.f, 0.f, 0.f);
    float2 vTexcoord = In.vTexcoord;
    float fSampleViewZ;
    float fBlurAmount = 0.0f;
    
    float4 vCenter = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    float4 vOriginal = g_BlurTexture.Sample(PointSampler, vTexcoord);
    float fOriginalAlpha = vOriginal.a;
    float fCenterViewZ = vCenter.y * g_fFar;
    
    float fTerrain = (1.f / 255.f);
    if (false == AlmostEqual3(vCenter.a, fTerrain))
    { // 터레인셀이 아니면 스킵
        Out.vBlurX = vOriginal;
        Out.vBlurWeight = 0.0f;
        return Out;
    }
    
    if (fCenterViewZ > g_fFocusDistance)
    { 
        // 현재 셀이 초점거리보다 멀면 블러
        // 블러되는 양은 (현재 셀의 거리가 얼마나 더 멀리 있냐) // END보다 멀면 최대치이므로 saturate
        float fValue = saturate((fCenterViewZ - g_fFarBlurStart) / (g_fFarBlurEnd - g_fFarBlurStart));
        fBlurAmount = fValue; // 0~1
    }
    
    if (fBlurAmount <= FLT_EPSILON3)  {
        // 최소치 이내면 블러 안함
        Out.vBlurX = vOriginal;
        Out.vBlurWeight = 0.0f;
        return Out;
    }
    
    float fAcc = 0.f;
    for (int i = -16; i < 16; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + ((float) i / g_vResolution.x) * fBlurAmount * g_fDOFBlurMultiplier;
        vTexcoord.y = In.vTexcoord.y;
        
        vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
        
        if (false == AlmostEqual3(vDepthDesc.w, fTerrain))
        { // 터레인셀이 아니면 스킵
            continue;
        }
        fSampleViewZ = vDepthDesc.y * g_fFar;
        if (abs(fCenterViewZ - fSampleViewZ) > g_fDepthThreshold)
        { // 센터 위치와 z값이 너무 많이 차이나면 샘플링 스킵
            continue;
        }
        
        vColor += g_fWeights_32[i + 16] * g_BlurTexture.Sample(BorderZeroSampler, vTexcoord).rgb;
        fAcc += g_fWeights_32[i + 16];
    }
    
    if (fAcc <= FLT_EPSILON3)
    { // 블러 실패하면 원본값 씀
        Out.vBlurX = vOriginal;
        Out.vBlurWeight = 0.0f;
        return Out;
    }
    else
    {
        vColor /= fAcc;
    }
    
    float fOriginalLumen = length(vOriginal.rgb); // 원본 빛양을 보존
    float fBlurLumen = length(vColor);

    float fLumenScale = 1.0f;
    if (fBlurLumen > FLT_EPSILON3)
    {
        fLumenScale = fOriginalLumen / fBlurLumen;
    }
    float3 vBlurAdjusted = vColor * fLumenScale;

    float fBlendAmount = saturate(fBlurAmount * g_fDOFBlurMultiplier);
    float3 vFinalColor = lerp(vOriginal.rgb, vBlurAdjusted, fBlendAmount);
    
    Out.vBlurX = float4(vColor, fOriginalAlpha);
    Out.vBlurWeight = fAcc;
    
    return Out;
}
PS_OUT_BACKBUFFER PS_MAIN_POSTCOMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float4 vDepthDesc = float4(0.f, 0.f, 0.f, 1.f);
    float3 vColor = float3(0.f, 0.f, 0.f);
    float2 vTexcoord = In.vTexcoord;
    float fSampleViewZ;
    float fBlurAmount = 0.0f;
    
    float4 vCenter = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    float4 vOriginal = g_DiffuseTexture.Sample(PointSampler, vTexcoord);
    float4 vXBlur = g_BlurTexture.Sample(BorderZeroSampler, vTexcoord);
    float fOriginalAlpha = vOriginal.a;
    float fCenterViewZ = vCenter.y * g_fFar;
    
    float fTerrain = (1.f / 255.f);
    if (false == AlmostEqual3(vCenter.a, fTerrain))
    { // 터레인셀이 아니면 스킵
        Out.vBackBuffer = vOriginal;
        Out.vEnvironment = 0.0f;
        return Out;
    }
    
    if (fCenterViewZ > g_fFocusDistance)
    {
        // 현재 셀이 초점거리보다 멀면 블러
        // 블러되는 양은 (현재 셀의 거리가 얼마나 더 멀리 있냐) // END보다 멀면 최대치이므로 saturate
        float fValue = saturate((fCenterViewZ - g_fFarBlurStart) / (g_fFarBlurEnd - g_fFarBlurStart));
        fBlurAmount = fValue; // 0~1
    }
    
    if (fBlurAmount <= FLT_EPSILON3)
    {
        // 최소치 이내면 블러 안함
        Out.vBackBuffer = vOriginal;
        Out.vEnvironment = 0.0f;
        return Out;
    }
    
    float fAcc = 0.f;
    for (int i = -16; i < 16; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + ((float) i / g_vResolution.y) * fBlurAmount * g_fDOFBlurMultiplier;
        
        vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
        
        if (false == AlmostEqual3(vDepthDesc.w, fTerrain))
        { // 터레인셀이 아니면 스킵
            continue;
        }
        fSampleViewZ = vDepthDesc.y * g_fFar;
        if (abs(fCenterViewZ - fSampleViewZ) > g_fDepthThreshold)
        { // 센터 위치와 z값이 너무 많이 차이나면 샘플링 스킵
            continue;
        }
        
        vColor += g_fWeights_32[i + 16] * g_BlurTexture.Sample(BorderZeroSampler, vTexcoord).rgb;
        fAcc += g_fWeights_32[i + 16];
    }
    if (fAcc <= FLT_EPSILON3)
    { // 블러 실패하면 원본값 씀
        Out.vBackBuffer = vOriginal;
        Out.vEnvironment = 0.0f;
        return Out;
    }
    else
    {
        vColor /= fAcc;
    }
    
    float fOriginalLumen = length(vOriginal.rgb); // 원본 빛양을 보존
    float fBlurLumen = length(vColor);

    float fLumenScale = 1.0f;
    if (fBlurLumen > FLT_EPSILON3)
    {
        fLumenScale = fOriginalLumen / fBlurLumen;
    }
    float3 vBlurAdjusted = vColor * fLumenScale;

    float fBlendAmount = saturate(fBlurAmount * g_fDOFBlurMultiplier);
    float3 vFinalColor = lerp(vOriginal.rgb, vBlurAdjusted, fBlendAmount);
    
    Out.vBackBuffer = float4(vColor, fOriginalAlpha);
    Out.vEnvironment = fAcc;
    
    return Out;
}
PS_OUT_BLUR_X PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -15; i < 16; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_vResolution.y;
        
        vColor += g_fWeights_32[i + 16] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor;
    Out.vBlurWeight = 0.f;
    
    return Out;
}
PS_OUT_FLT4_SINGLE PS_MAIN_FOG(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    vector vColor = g_OriginalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (0.f == g_fFogPow)
    {
        if (1 == vDepthDesc.y)
            Out.vFirstTarget = float4(vColor.rgb, 0.f);
        else
            Out.vFirstTarget = float4(vColor.rgb, 1.f);
        return Out;
    }
    
    vector vFinalColor;
    float fViewZ = vDepthDesc.y * g_fFar;
    
    float fRatio;
    
    fRatio = pow(1.f - saturate(exp(-1.f * vDepthDesc.y * g_fFogDensity)), g_fFogPow);
    
    vFinalColor = lerp(vColor, g_vFogColor, fRatio);
    vFinalColor.a = 1.f;
    
    if (1.f == vDepthDesc.y)
    {
        vFinalColor = float4(g_vFogColor);
    }
    Out.vFirstTarget = vFinalColor;
 
    return Out;
}
PS_OUT_BACKBUFFER PS_TONE_MAPPING(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    vector vColor = g_OriginalTexture.Sample(DefaultSampler, In.vTexcoord);
    vColor *= g_fToneMappingExposure; 
    vColor = pow(vColor, 2.2f);
    switch (g_iToneMappingType) {
        case 0:
            vColor = pow(vColor, 1.f/2.2f);
            vColor /= g_fToneMappingExposure;
            break;
        case 1:
            vColor = float4(ReinHard_ToneMapper(vColor.xyz), 1.f);
            break;
        case 2:
            vColor = float4(Filmic_ToneMapper(vColor.xyz), 1.f);
            break;
        default:
            break;
    }
    
    Out.vBackBuffer = vColor;
    Out.vEnvironment = float4(0.f, 0.f, 0.f, 1.f);
    return Out;
}
PS_OUT_SSAO_AMBIENT_OCCLUSION PS_SSAO_AMBIENT_OCCLUSION(PS_IN In)
{
    PS_OUT_SSAO_AMBIENT_OCCLUSION Out;
    float2 uv = In.vTexcoord;
    
    float4 vDepthDesc       = g_DepthTexture.Sample(PointSampler, uv);
    float fCenterViewSpaceZ = vDepthDesc.y * g_fFar; // n ~ f
    float fRatio = saturate(smoothstep(0.f, 1.f, (g_fFar - fCenterViewSpaceZ) / g_fFar)); // 멀어지면 너무 반짝거려서 스무딩
    
    if (fRatio <= 0.f) {
        Out.fOcclusion = 1.f;
        return Out;
    }
    float4 vCenterViewPosition;
    {
        vCenterViewPosition.x = uv.x * 2.f - 1.f;
        vCenterViewPosition.y = uv.y * -2.f + 1.f;
        vCenterViewPosition.z = vDepthDesc.x;
        vCenterViewPosition.w = 1.f;
        vCenterViewPosition *= fCenterViewSpaceZ;
        vCenterViewPosition = mul(vCenterViewPosition, g_invmatProj);
        vCenterViewPosition /= vCenterViewPosition.w;
    }
    float3 vNormal = normalize(g_NormalTexture.Sample(PointSampler, uv).xyz * 2.f - 1.f);
    float3 vCenterViewNormal = mul(vNormal, (float3x3)g_ViewMatrix);
    
    float2 vNoiseScale = g_vResolution / 4.f;
    float3 vNoise = g_SSAONoiseTexture.Sample(SsaoDataSampler, uv * vNoiseScale).xyz;
    
    float3 vTangent = normalize(vNoise - vCenterViewNormal * dot(vNoise, vCenterViewNormal));
    float3 vBiNormal = cross(vCenterViewNormal, vTangent);
    float3x3 toViewTBNMatrix = float3x3(vTangent, vBiNormal, vCenterViewNormal);
    
    float fOcclusion = 0.f;
    for (uint i = 0; i < g_iKernelSize; ++i)
    {
        float3 vViewSamplePosVec = mul(g_SamplePos[i].xyz, toViewTBNMatrix); // TANSPACE -> VIEW
        float4 vViewSamplePos = vCenterViewPosition + float4(vViewSamplePosVec * g_fSSAORadius, 0.f);
        float4 vNDCSampleOffset = mul(vViewSamplePos, g_ProjMatrix);
               vNDCSampleOffset /= vNDCSampleOffset.w;
        
        float2 sampleUV;
        sampleUV.x = vNDCSampleOffset.x * 0.5f + 0.5f;
        sampleUV.y = vNDCSampleOffset.y * -0.5f + 0.5f;

        if (sampleUV.x < 0 || sampleUV.x > 1
         || sampleUV.y < 0 || sampleUV.y > 1) {
            continue;
        }

        float fRecordedSampleViewPosDepth = g_DepthTexture.Sample(PointSampler, sampleUV).y * g_fFar;
        float fIsOccluded = (fRecordedSampleViewPosDepth < vViewSamplePos.z - g_fSSAO_BIAS) ? 1.0f : 0.0f;

        float fRangeCheck = smoothstep(0.f, 1.f, g_fSSAORadius / abs(vViewSamplePos.z - fRecordedSampleViewPosDepth));
        fIsOccluded *= fRangeCheck;
        fOcclusion += fIsOccluded;
    }
    fOcclusion = saturate(1.f - (fOcclusion * g_fSSAOStrength / g_iKernelSize) * fRatio);
    Out.fOcclusion = fOcclusion ;
    
    return Out;
}
PS_OUT_SSAO_BLUR PS_SSAO_BLUR(PS_IN In)
{
    PS_OUT_SSAO_BLUR Out;

    float2 vTexelSize = 1.0f / g_vResolution;

    float fResult = 0.0f;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            float2 vOffset = float2((float) x, (float) y) * vTexelSize;
            fResult += g_SSAOInputTexture.Sample(BorderZeroSampler, In.vTexcoord + vOffset).r;
        }
    }

    fResult /= 16.f; // 16개 샘플 평균

    Out.fBlur = fResult;
    return Out;
}

struct PS_OUT_DECAL
{
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT_DECAL PS_DECAL(PS_IN In)
{
    PS_OUT_DECAL Out;

    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * g_fFar;
    
    
    float4 vPosition, vPreShadowPosition;
    
    /* (로컬위치 * 월드 * 뷰 * 투영 / w) -> (로컬위치 * 월드)   */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    vPosition = vPosition * fViewZ;
    
    vPosition = mul(vPosition, g_invmatProj);
    vPosition = mul(vPosition, g_invMatView);
    vPreShadowPosition = vPosition;
    
    
    return Out;
}
technique11 DefaultTechnique
{
    pass DebugPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass DirectionalPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass PointPass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass CombinedPass // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass BlurPass // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass SpotPass // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }

    pass UpSamplePass // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UPSAMPLE();
    }
    pass PS_MAIN_BLOOM_BLUR_XPASS // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_BLUR_X();
    }
    pass PS_MAIN_BLOOM_BLUR_COMBINEPASS // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_BLUR_COMBINE();
    }

    pass EmbossingPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMBOSS();
    }

    pass Bloom_AccumPass // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_ACCUM();
    }

    pass Bloom_FinishPass // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_FINISH();
    }
    pass BlurX_ENV_Pass // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X_ENV();
    }
    pass PostCombinedPass // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_Environment_SRead, 1);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POSTCOMBINED();
    }

    pass FogPass // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FOG();
    }

    pass Tone_Mapping_Pass // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TONE_MAPPING();
    }

    pass SSAO_AmbientOcclusion // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO_AMBIENT_OCCLUSION();
    }

    pass SSAO_Blur_X // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SSAO_BLUR();
    }

    pass MotionBlurPass // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTIONBLUR();
    }
    pass DownSamplePass // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOWNSAMPLE();
    }
    pass MotionBlurTileSamplePass // 20
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTIONBLUR_TILESAMPLE();
    }
    pass MotionBlurTentSamplePass // 21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MOTIONBLUR_TENTSAMPLE();
    }

    pass DecalPass // 22
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }
}
