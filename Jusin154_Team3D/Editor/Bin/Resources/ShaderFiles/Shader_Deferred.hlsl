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
float g_fPreShadowFar;
float g_fCascadeSplitRatioNear;
float g_fCascadeSplitRatioFar;
uint g_iMaxShadowWidth;
uint g_iMaxShadowHeight;
float2 g_vResolution;
float4 g_vShadowBias;

uint g_iBloomEmbossingPass;
float g_fBloomThreshold;
float g_fDepthThreshold;
float g_fFocusDistance;
float g_fFarBlurStart;
float g_fFarBlurEnd;
float g_fDOFBlurMultiplier;
float g_fSSAO_BIAS;

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
Texture2D g_ShadowFarTexture;
Texture2D g_PreShadowTexture;
Texture2D g_BlurTexture;
Texture2D g_BlurXTexture;

Texture2D g_BlurWeightTexture;
Texture2D g_BlurWeightXTexture;

Texture2D g_SurfaceTexture;
Texture2D g_OriginalTexture;


vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

float g_fFogDensity;
float g_fFogPow;
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
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = 0.f;
    Out.vSpecular = 0.f;
    
    float2 uv = In.vTexcoord;
    
    float3 vAlbedo          = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    float3 vNormal          = normalize(g_NormalTexture.Sample(DefaultSampler, uv).xyz * 2.f - 1.f);
    float4 vDepth           = g_DepthTexture.Sample(DefaultSampler, uv);
    float fAmbientOcclusion = g_SSAOInputTexture.Sample(DefaultSampler, uv).r;
    
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
    
    
    
    if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_METALNESS)) // Metallic Roughness Occ
    {
        float3 vMRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;
        
        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_SPECULAR)) // Specular Roughness Occ
    {
        fMetallic = 0.f;
        
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        vF0 = vSRO.rrr;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
    }
    else if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_ANISOTROPY))
    {
        fMetallic = 0.f;
        
        float4 vSRXO = g_SurfaceTexture.Sample(DefaultSampler, uv);
        vF0 = vSRXO.rrr;
        fRoughness = vSRXO.g;
        fOcclusion = vSRXO.a;
    }
    else // basic Lighting(phong blinn) // if you were here, you miss some assets.
    {
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir.xyz) * -1.f, vNormal), 0.f) + (fAmbientOcclusion * g_vLightAmbient * g_vMtrlAmbient));
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f);
    
        return Out;
    }
    float fTotalOcclusion = saturate(fOcclusion * fAmbientOcclusion);
    
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
    Out.vSpecular = float4(vFinalSpecular, 1.f);
    
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
    float fAmbientOcclusion = g_SSAOInputTexture.Sample(DefaultSampler, uv).r;
    
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
        Out.vSpecular = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_METALNESS)) // Metallic
    {
        float3 vMRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;
        
        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_SPECULAR)) // Specular
    {
        fMetallic = 0.f;
        
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        vF0 = vSRO.rrr;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
    }
    else if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_ANISOTROPY))
    {
        fMetallic = 0.f;
        
        float4 vSRXO = g_SurfaceTexture.Sample(DefaultSampler, uv);
        vF0 = vSRXO.rrr;
        fRoughness = vSRXO.g;
        fOcclusion = vSRXO.a;
    }
    else // basic Lighting(phong blinn) // if you were here, you miss some assets.
    {
        Out.vShade = fAttenuation * (g_vLightDiffuse * saturate(max(dot(vToLight * -1.f, vNormal), 0.f) + (fAmbientOcclusion * g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fAttenuation * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f));
    
        return Out;
    }
    
    float fTotalOcclusion = saturate(fOcclusion * fAmbientOcclusion);
    
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
    Out.vSpecular = float4(vFinalSpecular, 1.f);
    
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
    float fAmbientOcclusion = g_SSAOInputTexture.Sample(DefaultSampler, uv).r;
    
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
        Out.vSpecular = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    // 각도 기반 감쇠
    float3 vLightDir = normalize(g_vLightDir.xyz); 
    float fCosAngle = dot(vLightDir, vLightToPixel);

    if (fCosAngle < g_fSpotOuterAngle)
    {
        Out.vShade = float4(0.f, 0.f, 0.f, 1.f);
        Out.vSpecular = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
    
    float fSpotAttenenuation = saturate((fCosAngle - g_fSpotInnerAngle) / (g_fSpotInnerAngle - g_fSpotOuterAngle));
    fSpotAttenenuation *= fSpotAttenenuation;
    fAttenuation *= fSpotAttenenuation;
    
    if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_METALNESS)) // Metallic
    {
        float3 vMRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;
        
        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_SPECULAR)) // Specular
    {
        fMetallic = 0.f;
        
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        vF0 = vSRO.rrr;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
    }
    else if (true == AlmostEqual3(vDepth.b * AI_TEXTURE_TYPE_MAX, AI_TEXTURE_TYPE_ANISOTROPY))
    {
        fMetallic = 0.f;
        
        float4 vSRXO = g_SurfaceTexture.Sample(DefaultSampler, uv);
        vF0 = vSRXO.rrr;
        fRoughness = vSRXO.g;
        fOcclusion = vSRXO.a;
    }
    else // basic Lighting(phong blinn) // if you were here, you miss some assets.
    {
        Out.vShade = fAttenuation * (g_vLightDiffuse * saturate(max(dot(vToLight * -1.f, vNormal), 0.f) + (fAmbientOcclusion * g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fAttenuation * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f));
    
        return Out;
    }
    float fTotalOcclusion = saturate(fOcclusion * fAmbientOcclusion);
    
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
    Out.vSpecular = float4(vFinalSpecular, 1.f);
    
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
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (0.f == vDiffuse.a) { discard; }
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vBackBuffer = vDiffuse * vShade + vSpecular;
    
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * g_fFar;
    
    
    vector vPosition, vPreShadowPosition;
    
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
    float fVisibility_Dynamic_Near = ShadowVisibility_hwPCF(g_ShadowNearTexture, vNearShadowPos, float2(g_iMaxShadowWidth, g_iMaxShadowHeight), g_vShadowBias.x);
    float fVisibility_Dynamic_Middle = ShadowVisibility_hwPCF(g_ShadowMiddleTexture, vMiddleShadowPos, float2(g_iMaxShadowWidth, g_iMaxShadowHeight), g_vShadowBias.y);
    float fVisibility_Dynamic_Far = ShadowVisibility_hwPCF(g_ShadowFarTexture, vFarShadowPos, float2(g_iMaxShadowWidth, g_iMaxShadowHeight), g_vShadowBias.z);
    float fVisibility_Static = ShadowVisibility_hwPCF(g_PreShadowTexture, vPreShadowPosition, float2(g_iMaxShadowWidth, g_iMaxShadowHeight), g_vShadowBias.w);
    
////////////////////////////
    // 케스케이드
    float fDepthRatio = saturate(vDepthDesc.y);

    // 경계 블렌딩 폭. 경계 섞는 비중
    float fShadowCascadeBlendWidthRatio = 0.02f; // 2% 섞음

    // 경계 주변 부드럽게 할 비중
    float fCascadeBlend_NearToMiddle = smoothstep(g_fCascadeSplitRatioNear - fShadowCascadeBlendWidthRatio,
        g_fCascadeSplitRatioNear + fShadowCascadeBlendWidthRatio, fDepthRatio);
    float fCascadeBlend_MiddleToFar = smoothstep(g_fCascadeSplitRatioFar - fShadowCascadeBlendWidthRatio,
        g_fCascadeSplitRatioFar + fShadowCascadeBlendWidthRatio, fDepthRatio);

    // Near -> Middle
    float fVisibilityDynamic = lerp(fVisibility_Dynamic_Near, fVisibility_Dynamic_Middle, fCascadeBlend_NearToMiddle);
    // Middle -> Far 경계 부드럽게
    fVisibilityDynamic = lerp(fVisibilityDynamic, fVisibility_Dynamic_Far, fCascadeBlend_MiddleToFar);

    // PreShadow(Static)로 넘어가는 구간(원하는 대로 조절)
    float fStaticShadowBlendStartRatio = 0.70f;
    float fStaticShadowBlendEndRatio = 0.95f;

    float fStaticShadowBlendWeight = smoothstep(fStaticShadowBlendStartRatio, fStaticShadowBlendEndRatio, fDepthRatio);
    float fVisibilityCombined = lerp(fVisibilityDynamic, fVisibility_Static, fStaticShadowBlendWeight);

    // 최소 밝기
    float fMinShadowBrightness = 0.25f;
    float fShadowMultiplier = lerp(fMinShadowBrightness, 1.0f, saturate(fVisibilityCombined));

    Out.vBackBuffer.rgb *= fShadowMultiplier;
    
    
////////////////////////////
    // 블러 추가
    float4 vColor = 0.f;
    
    /* (로컬위치 * 월드 * 광원의 뷰 * 광원의 투영 ) -> (로컬위치 * 월드 * 광원의 뷰 * 광원의 투영 * (/w) */
    float2 vTexcoord;
    vTexcoord.x = (vNearShadowPos.x / vNearShadowPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vNearShadowPos.y / vNearShadowPos.w) * -0.5f + 0.5f;
    
    for (int i = -63; i < 64; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_vResolution.y;
        
        vColor += g_fWeights_128[i + 63] * g_BlurXTexture.Sample(BorderZeroSampler, vTexcoord);
    }
    
    Out.vBackBuffer += vColor;
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
    float fMask = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0).a;
    float2 vSrcTexelSize = float2(1.f / g_vResolution.x, 1.f / g_vResolution.y);
    float3 vColor = (0.f, 0.f, 0.f);
    
    uint iMask = (uint) round(fMask * 255.f); // int a = 1  -> // vBloom.a = (enum / 255);
    //if (0 != iMask) {
        // 주변 4개 셀(LT RT LB RB)및 중앙을 이중선형샘플링, 이후 자체적으로 이중선형샘플링
        
    float3 vCenterColor = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0);
        uv = In.vTexcoord + vSrcTexelSize * float2(-1.0, -1.0);
    float3 fLTColor = BilinearFetches(g_vResolution, g_DiffuseTexture, uv, BorderZeroLinearSampler);
        uv = In.vTexcoord + vSrcTexelSize * float2(0.0, -1.0);
    float3 fTColor = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0);
        uv = In.vTexcoord + vSrcTexelSize * float2(+1.0, -1.0);
    float3 fRTColor = BilinearFetches(g_vResolution, g_DiffuseTexture, uv, BorderZeroLinearSampler);
        uv = In.vTexcoord + vSrcTexelSize * float2(-1.0, 0.0);
    float3 fLColor = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0);
        uv = In.vTexcoord + vSrcTexelSize * float2(1.0, 0.0);
    float3 fRColor = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0);
        uv = In.vTexcoord + vSrcTexelSize * float2(-1.0, +1.0);
    float3 fLBColor = BilinearFetches(g_vResolution, g_DiffuseTexture, uv, BorderZeroLinearSampler);
        uv = In.vTexcoord + vSrcTexelSize * float2(0.0, 1.0);
    float3 fBColor = g_DiffuseTexture.SampleLevel(BorderZeroSampler, uv, 0);
        uv = In.vTexcoord + vSrcTexelSize * float2(+1.0, +1.0);
    float3 fRBColor = BilinearFetches(g_vResolution, g_DiffuseTexture, uv, BorderZeroLinearSampler);
    
        
    
        // vSample /= fWeight 이중선형샘플러가 자체적으로 웨이츠 계싼해주기 때문에 안해도 상관없을듯
    vColor = vCenterColor * 0.25f + (fLTColor + fRTColor + fLBColor + fRBColor) * 0.0625f + (fTColor + fLColor + fRColor + fBColor) * 0.125f;
        //if (iMask == 2)
        //{
        //    vColor *= 3.f;
        //}
    //}
    float fIntensity = dot(vColor, float3(0.2126f, 0.7152f, 0.0722f));
    fIntensity = max(FLT_EPSILON3, fIntensity);

    
    float fBloomIntensity = GetBloomCurve(fIntensity, g_fBloomThreshold, g_iBloomEmbossingPass);
    float3 bloomColor = (vColor * fBloomIntensity) / fIntensity;
    Out.vFirstTarget = float4(bloomColor, 1.f);
    
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_ACCUM(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    float3 vColorSrcA = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord).xyz;
    float3 vColorSrcB = g_BlurTexture.Sample(BorderZeroSampler, In.vTexcoord).xyz;
    
    Out.vFirstTarget = float4(saturate(vColorSrcA + vColorSrcB), 1.f);
    
    return Out;
}
PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_FINISH(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    vector vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    float3 vBloom = g_BlurTexture.Sample(BorderZeroSampler, In.vTexcoord).xyz;
    Out.vFirstTarget = vColor;
    Out.vFirstTarget += float4(vBloom, 1.f);
    
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
    
    vector vDepthDesc;
    float2 vTexcoord;
    float fSampleViewZ;
    float4 vColor = 0.f;
    float fBlurAmount = 0.0f;
    
    float4 vCenter = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vOriginal = g_BlurTexture.Sample(BorderZeroSampler, In.vTexcoord);
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
        
        if (false == AlmostEqual3(vDepthDesc.a, fTerrain))
        { // 터레인셀이 아니면 스킵
            continue;
        }
        fSampleViewZ = vDepthDesc.y * g_fFar;
        if (abs(fCenterViewZ - fSampleViewZ) > g_fDepthThreshold)
        { // 센터 위치와 z값이 너무 많이 차이나면 샘플링 스킵
            continue;
        }
        
        vColor += g_fWeights_32[i + 16] * g_BlurTexture.Sample(BorderZeroSampler, vTexcoord);
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
        vColor.xyz /= fAcc;
    }
    
    Out.vBlurX = vColor;
    Out.vBlurWeight = fAcc;
    
    return Out;
}
PS_OUT_BACKBUFFER PS_MAIN_POSTCOMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDepthDesc;
    float2 vTexcoord;
    float fSampleViewZ;
    float4 vColor = 0.f;
    float fBlurAmount = 0.0f;
    
    float4 vOriginal = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vCenter = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
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
        // 최소치 이내면 블러 스킵
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
        
        if (false == AlmostEqual3(vDepthDesc.a, fTerrain))
        { // 터레인셀이 아니면 스킵
            continue;
        }
        fSampleViewZ = vDepthDesc.y * g_fFar;
        if (abs(fCenterViewZ - fSampleViewZ) > g_fDepthThreshold)
        { // 센터 위치와 z값이 너무 많이 차이나면 샘플링 스킵
            continue;
        }
        
        vColor += g_fWeights_32[i + 16] * g_BlurTexture.Sample(BorderZeroSampler, vTexcoord);
        fAcc += g_fWeights_32[i + 16];
    }
    if (fAcc <= FLT_EPSILON3)
    { // 블러 실패하면 원본값 씀
        Out.vBackBuffer = vOriginal;
        Out.vEnvironment = 0.0f;
        return Out;
        return Out;
    }
    else
    {
        vColor.xyz /= fAcc;
    }
    
    float fOriginalLumen = length(vOriginal.rgb);
    float fBlurLumen = length(vColor.rgb);

    float fLumenScale = 1.0f;
    if (fBlurLumen > 1e-4f)
    {
        fLumenScale = fOriginalLumen / fBlurLumen;
    }
    float3 vBlurMatched = vColor.rgb * fLumenScale;

    float fBlendAmount = saturate(fBlurAmount * g_fDOFBlurMultiplier);
    float3 finalColor = lerp(vOriginal.rgb, vBlurMatched, fBlendAmount);
    
    Out.vBackBuffer = float4(finalColor.xyz, vOriginal.a);
    Out.vEnvironment = 0.f;
    
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
    
    vector vFinalColor;
    float  fViewZ = vDepthDesc.y * g_fFar;
    
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
    fOcclusion = (1.f - (fOcclusion / g_iKernelSize) * fRatio);
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
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
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
}
