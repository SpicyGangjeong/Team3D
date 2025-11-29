#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_invMatView, g_invmatProj;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_PreShadowLightViewMatrix, g_PreShadowLightProjMatrix;

float g_fFar;
float g_fPreShadowFar;
uint g_iMaxShadowWidth;
uint g_iMaxShadowHeight;
float2 g_vResolution;

uint g_iBloomEmbossingPass;
float g_fThreshold;

Texture2D g_Texture;

float g_fLightRange;
float g_fSpotInnerAngle;
float g_fSpotOuterAngle;

vector g_vLightDir;
vector g_vLightPos;
vector g_vLightPosOffset;

vector g_vCamPosition;


Texture2D g_NormalTexture;
Texture2D g_DiffuseTexture;
Texture2D g_ShadeTexture;
Texture2D g_DepthTexture;
Texture2D g_SpecularTexture;
Texture2D g_ShadowTexture;
Texture2D g_PreShadowTexture;
Texture2D g_BlurTexture;
Texture2D g_BlurXTexture;

Texture2D g_BlurWeightTexture;
Texture2D g_BlurWeightXTexture;

Texture2D g_SurfaceTexture;


vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

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

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vBackBuffer : SV_TARGET0;
    float4 vBloomBuffer : SV_Target1;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vBloomBuffer = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
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

    float3 vToView = normalize(g_vCamPosition.xyz - vWorldPosition.xyz); // 픽셀에서 카메라로
    float3 vToLight = normalize(-g_vLightDir.xyz); // 픽셀에서 라이트로
    
    
    if (true == AlmostEqual3(vDepth.b, AI_TEXTURE_TYPE_METALNESS)) // Metallic
    {
        float3 vMRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;
        
        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(vDepth.b, AI_TEXTURE_TYPE_SPECULAR)) // Specular
    {
        fMetallic = 0.f;
        
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        vF0             = vSRO.rrr;
        fRoughness      = vSRO.g;
        fOcclusion      = vSRO.b;
    }
    else // basic Lighting(phong blinn) // if you were here, you miss some assets.
    {
        Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir.xyz) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f);
    
        return Out;
    }
    
    float fDiffuseAOStrength = lerp(0.3f, 5.f, fOcclusion);
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vToView, vToLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, fAttenuation, vF0);
    
    PBR_Out.vShade *= fDiffuseAOStrength;

    Out.vShade = float4(PBR_Out.vShade, 1.f);
    Out.vSpecular = float4(PBR_Out.vSpecular, 1.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
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
    
    float3 vToView = normalize(g_vCamPosition.xyz - vWorldPosition.xyz); // 픽셀에서 카메라로
    float3 vToLight = normalize(g_vLightPos.xyz - vWorldPosition.xyz); // 픽셀에서 라이트로
    
    {
        float fDistance = length(vToLight);
        fAttenuation = saturate((g_fLightRange - fDistance) / g_fLightRange);
        if (fAttenuation <= 0.f) { discard; }
    }

    if (true == AlmostEqual3(vDepth.b, AI_TEXTURE_TYPE_METALNESS)) // Metallic
    {
        float3 vMRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;

        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(vDepth.b, AI_TEXTURE_TYPE_SPECULAR)) // Specular
    {
        fMetallic = 0.f;
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        vF0 = vSRO.rrr;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
    }
    else // basic Lighting(phong blinn) // if you were here, you miss some assets.
    {
        Out.vShade = fAttenuation * (g_vLightDiffuse * saturate(max(dot(vToLight * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fAttenuation * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f));
    
        return Out;
    }
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vToView, vToLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, fAttenuation, vF0);
    
    Out.vShade = float4(PBR_Out.vShade, 1.f);
    Out.vSpecular = float4(PBR_Out.vSpecular, 1.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
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
    
    float3 vToView = normalize(g_vCamPosition.xyz - vWorldPosition.xyz); // 픽셀에서 카메라로
    float3 vToLight = normalize(g_vLightPos.xyz - vWorldPosition.xyz); // 픽셀에서 라이트로
    
    {
        float fDistance = length(vToLight);
        fAttenuation = saturate((g_fLightRange - fDistance) / g_fLightRange);
        if (fAttenuation <= 0.f) { discard; }
    }
    float fCosAngle = dot(vToView, g_vLightDir.xyz);
    if (fCosAngle < g_fSpotOuterAngle) { discard; }
    
    float fSpotAttenenuation = saturate((fCosAngle - g_fSpotInnerAngle) / (g_fSpotInnerAngle - g_fSpotOuterAngle));
    fAttenuation *= fSpotAttenenuation;
    
    if (true == AlmostEqual3(vDepth.b, AI_TEXTURE_TYPE_METALNESS)) // Metallic
    {
        float3 vMRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        fMetallic = vMRO.r;
        fRoughness = vMRO.g;
        fOcclusion = vMRO.b;

        vF0 = lerp(float3(0.04f, 0.04f, 0.04f), vAlbedo, fMetallic);
    }
    else if (true == AlmostEqual3(vDepth.b, AI_TEXTURE_TYPE_SPECULAR)) // Specular
    {
        fMetallic = 0.f;
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        vF0 = vSRO.rrr;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
    }
    else // basic Lighting(phong blinn) // if you were here, you miss some assets.
    {
        Out.vShade = fAttenuation * (g_vLightDiffuse * saturate(max(dot(vToLight * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
        Out.vSpecular = fAttenuation * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(vToView * -1.f, vToLight), 0.f), 50.f));
    
        return Out;
    }
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vToView, vToLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, fAttenuation, vF0);
    
    Out.vShade = float4(PBR_Out.vShade, 1.f);
    Out.vSpecular = float4(PBR_Out.vSpecular, 1.f);
    
    return Out;
}

//float g_fWeights[32] =
//{
//    0.000400, 0.000700, 0.001200, 0.002000, 0.003200, 0.005000, 0.007500, 0.011000,
//    0.015700, 0.021700, 0.029000, 0.037300, 0.046200, 0.055200, 0.063600, 0.070800,
//    0.075900, 0.078500, 0.078500, 0.075900, 0.070800, 0.063600, 0.055200, 0.046200,
//    0.037300, 0.029000, 0.021700, 0.015700, 0.011000, 0.007500, 0.005000, 0.003200
//};

float g_fWeights[128] =
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


PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (0.f == vDiffuse.a)
    {
        discard;
    }
    
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
    vPosition = mul(vPosition, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
    vPreShadowPosition = mul(vPreShadowPosition, g_PreShadowLightViewMatrix);
    vPreShadowPosition = mul(vPreShadowPosition, g_PreShadowLightProjMatrix);
    
    /* (로컬위치 * 월드 * 광원의 뷰 * 광원의 투영 ) -> (로컬위치 * 월드 * 광원의 뷰 * 광원의 투영 * (/w) */
    float2 vTexcoord, vPreShadowTexcoord;
    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    vPreShadowTexcoord.x = (vPreShadowPosition.x / vPreShadowPosition.w) * 0.5f + 0.5f;
    vPreShadowTexcoord.y = (vPreShadowPosition.y / vPreShadowPosition.w) * -0.5f + 0.5f;
    
    /* 광원의 NDC에서 샘플링 */
    float fVisibility_Dynamic = ShadowVisibility_hwPCF(g_ShadowTexture, vPosition, float2(g_iMaxShadowWidth, g_iMaxShadowHeight), 0.0005f);
    float fVisibility_Static = ShadowVisibility_hwPCF(g_PreShadowTexture, vPreShadowPosition, float2(g_iMaxShadowWidth, g_iMaxShadowHeight), 0.0005f);
    
    Out.vBackBuffer.rgb *= lerp(0.5f, 1.f, min(fVisibility_Dynamic, fVisibility_Static));
    
    float4 vColor = 0.f;
    
    for (int i = -63; i < 64; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_vResolution.y;
        
        vColor += g_fWeights[i + 63] * g_BlurXTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBackBuffer += vColor;
    Out.vBloomBuffer;
    
    return Out;
}


struct PS_OUT_FLT4_SINGLE
{
    vector vSingleTarget : SV_TARGET0;
};


PS_OUT_FLT4_SINGLE PS_MAIN_REFIT(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;

    Out.vSingleTarget = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_EMBOSS(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;

    vector vInput = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    //float fMask = g_BloomMaskTexture.Sample(PointSampler, In.vTexcoord).a;
    float3 vColor = vInput.rgb;
    uint iMask = (uint) round(vInput.a * 255.f); // int a = 1  -> // vBloom.a = (enum / 255);
    
    switch (iMask)
    {
        case 0:
            /* None */
            vColor = float3(0.f, 0.f, 0.f);
            break;
        case 1:
            /* Basic_Apply */
            break;
        case 2:
            /* Multiply */
            vColor *= 3;
            break;
        case 3:
            break;
    }
    
    float fIntensity = dot(vColor, float3(0.2126f, 0.7152f, 0.0722f)); // 대략적인 밝기 ( 인간적인 )
    
    if (fIntensity <= 1e-4f)
    {
        Out.vSingleTarget = 0;
        return Out;
    }
    
    float fBloomIntensity = GetBloomCurve(fIntensity, g_fThreshold, g_iBloomEmbossingPass);
    
    float3 bloomColor = (vColor * fBloomIntensity) / fIntensity;
    Out.vSingleTarget = float4(bloomColor, 1.f);
    
    return Out;
}

struct PS_OUT_BLUR_X
{
    float4 vBlurX : SV_TARGET0;
    float4 vBlurWeight : SV_TARGET1;
};

PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_ACCUM(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    float3 vColorSrcA = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord).xyz;
    float3 vColorSrcB = g_BlurTexture.Sample(PointSampler, In.vTexcoord).xyz;
    
    Out.vSingleTarget = float4(saturate(vColorSrcA + vColorSrcB), 1.f);
    
    return Out;
}

PS_OUT_FLT4_SINGLE PS_MAIN_BLOOM_FINISH(PS_IN In)
{
    PS_OUT_FLT4_SINGLE Out;
    
    vector vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    float3 vBloom = g_BlurTexture.Sample(PointSampler, In.vTexcoord).xyz;
    Out.vSingleTarget = vColor;
    Out.vSingleTarget += float4(vBloom, 1.f);
    
    return Out;
}


PS_OUT_BLUR_X PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -63; i < 64; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_vResolution.x;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 63] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor;
    Out.vBlurWeight = 0.f;
    
    return Out;
}

PS_OUT_BLUR_X PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    for (int i = -63; i < 64; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_vResolution.y;
        
        vColor += g_fWeights[i + 63] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor;
    Out.vBlurWeight = 0.f;
    
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

    pass RefitPass // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_REFIT();
    }

    pass EmbossingPass // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMBOSS();
    }

    pass Bloom_BlurXPass // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Bloom_BlurYPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTURE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
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
}
