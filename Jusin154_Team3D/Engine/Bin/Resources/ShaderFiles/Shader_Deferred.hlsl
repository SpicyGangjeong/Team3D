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

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vBackBuffer : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
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
    
    // float3 vNormal = normalize(g_NormalTexture.Sample(DefaultSampler, uv).xyz * 2.f - 1);
    float3 vNormal = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, uv);
    
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
    
    float3 vF0;
    float fMetallic;
    float fRoughness;
    float fOcclusion;
    float fAttenuation;

    float3 vWPos = vWorldPosition.xyz;
    float3 vFromView = normalize(vWPos - g_vCamPosition.xyz); // 카메라에서 픽셀로 가는 방향
    float3 vFromLight = normalize(g_vLightDir.xyz); // 라이트에서 오는 라이트 벡터
    
    float3 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    
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
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        float3 vSpec = vSRO.r;
        
        fMetallic = 0.f;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
        
        
        vF0 = vSpec.xxx;
    }
    else
    {
        fMetallic = 0.f;
        fRoughness = 0.5f;
        fOcclusion = 1.f;
        vF0 = float3(0.04f, 0.04f, 0.04f);
    }
    
    fAttenuation = 1.f;
    
    float fDiffuseAOStrength = lerp(0.3f, 1.f, fOcclusion);
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vFromView, vFromLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, fAttenuation, vF0);
    
    PBR_Out.vShade *= fDiffuseAOStrength;
    
    Out.vShade = float4(PBR_Out.vShade, 1.f);
    Out.vSpecular = float4(PBR_Out.vSpecular, 1.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    float2 uv = In.vTexcoord;
    
    float3 vNormal = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, uv);
    
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
    
    float3 vWPos = vWorldPosition.xyz;
    float3 vFromView = normalize(vWPos - g_vCamPosition.xyz); // 카메라에서 픽셀로 가는 방향
    float3 vFromLight = normalize(g_vLightDir.xyz); // 라이트에서 오는 라이트 벡터
    
    float3 vF0;
    float fMetallic;
    float fRoughness;
    float fOcclusion;
    float fAttenuation;
    
    {
        float fDistance = length(vFromLight);
        fAttenuation = saturate((g_fLightRange - fDistance) / g_fLightRange);
        if (fAttenuation <= 0.f)
        {
            discard;
        }
    }

    float3 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    
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
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        float3 vSpec = vSRO.r;
        
        fMetallic = 0.f;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
        
        
        vF0 = vSpec.xxx;
    }
    else
    {
        fMetallic = 0.f;
        fRoughness = 0.5f;
        fOcclusion = 1.f;
        vF0 = float3(0.04f, 0.04f, 0.04f);
    }
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vFromView, vFromLight,
    vAlbedo, fMetallic, fRoughness,
    g_vLightDiffuse.rgb, fAttenuation, vF0);
    
    Out.vShade = float4(PBR_Out.vShade, 1.f);
    Out.vSpecular = float4(PBR_Out.vSpecular, 1.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    float2 uv = In.vTexcoord;
    
    float3 vNormal = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, uv);
    
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
    
    float3 vWPos = vWorldPosition.xyz;
    float3 vFromView = normalize(vWPos - g_vCamPosition.xyz); // 카메라에서 픽셀로 가는 방향
    float3 vFromLight = normalize(g_vLightDir.xyz); // 라이트에서 오는 라이트 벡터
    
    float3 vF0;
    float fMetallic;
    float fRoughness;
    float fOcclusion;
    float fAttenuation;
    {
        float fDistance = length(vFromLight);
        fAttenuation = saturate((g_fLightRange - fDistance) / g_fLightRange);
        if (fAttenuation <= 0.f)
        {
            discard;
        }
    }
    float fCosAngle = dot(vFromLight, g_vLightDir.xyz);
    if (fCosAngle < g_fSpotOuterAngle)
    {
        discard;
    }
    float fSpotAttenenuation = saturate((fCosAngle - g_fSpotInnerAngle) / (g_fSpotInnerAngle - g_fSpotOuterAngle));
    fAttenuation *= fSpotAttenenuation;
    
    float3 vAlbedo = g_DiffuseTexture.Sample(DefaultSampler, uv).rgb;
    
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
        float3 vSRO = g_SurfaceTexture.Sample(DefaultSampler, uv).rgb;
        float3 vSpec = vSRO.r;
        
        fMetallic = 0.f;
        fRoughness = vSRO.g;
        fOcclusion = vSRO.b;
        
        
        vF0 = vSpec.xxx;
    }
    else
    {
        fMetallic = 0.f;
        fRoughness = 0.5f;
        fOcclusion = 1.f;
        vF0 = float3(0.04f, 0.04f, 0.04f);
    }
    
    PBR_LIGHT_OUT PBR_Out = PBR_Lighting(vNormal, vFromView, vFromLight, vAlbedo, fMetallic, fRoughness, g_vLightDiffuse.rgb, fAttenuation, vF0);
    
    Out.vShade = float4(PBR_Out.vShade, 1.f);
    Out.vSpecular = float4(PBR_Out.vSpecular, 1.f);
    
    return Out;
}

float g_fWeights[32] =
{
    0.000400, 0.000700, 0.001200, 0.002000, 0.003200, 0.005000, 0.007500, 0.011000,
    0.015700, 0.021700, 0.029000, 0.037300, 0.046200, 0.055200, 0.063600, 0.070800,
    0.075900, 0.078500, 0.078500, 0.075900, 0.070800, 0.063600, 0.055200, 0.046200,
    0.037300, 0.029000, 0.021700, 0.015700, 0.011000, 0.007500, 0.005000, 0.003200
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
    
    //float fShadowDepth = g_ShadowTexture.Sample(DefaultSampler, vTexcoord).x;
    //float fPreShadowDepth = g_PreShadowTexture.Sample(DefaultSampler, vPreShadowTexcoord).x;
    //bool IsShadow = { false };
    //if (vPosition.z - 0.0005f > fShadowDepth)
    //{
    //    IsShadow = true;
    //}
    //if (vPreShadowPosition.z - 0.0005f > fPreShadowDepth)
    //{
    //    IsShadow = true;
    //}
    //if (true == IsShadow)
    //{
    //    Out.vBackBuffer *= 0.5f;
    //}
    float4 vColor = 0.f;
    
    //int iBlurWeight = g_BlurWeightTexture.Sample(ClampSampler, In.vTexcoord).r * 32.f;
    
    //int iBlurMin;
    //int iBlurMax;
    
    //if (iBlurWeight % 2 == 0)
    //{
    //    iBlurMin = -1 * iBlurWeight / 2;
    //    iBlurMax = iBlurWeight / 2;
    //}
    //else
    //{
    //    iBlurMin = -iBlurWeight / 2;
    //    iBlurMax = iBlurWeight / 2 + 1;
    //}
    
    //[loop]  // 변수로 루프돌리려면 반드시 필요함
    
    for (int i = -15; i < 16; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float) i / g_vResolution.y;
        
        vColor += g_fWeights[i + 15] * g_BlurXTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBackBuffer += vColor;
    
    return Out;
}

struct PS_OUT_BLUR_X
{
    float4 vBlurX : SV_TARGET0;
};


PS_OUT_BLUR_X PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT_BLUR_X Out;
    
    float2 vTexcoord;
    float4 vColor = 0.f;
    
    //TODO 블러 조절 해보려햇는데 실패함..
    
    //int iBlurWeight = g_BlurWeightTexture.Sample(ClampSampler, In.vTexcoord).r * 32.f;
    
    //int iBlurMin;
    //int iBlurMax;
    
    //if (iBlurWeight % 2 == 0)
    //{
    //    iBlurMin = -1 * iBlurWeight / 2;
    //    iBlurMax = iBlurWeight / 2;
    //}
    //else
    //{
    //    iBlurMin = -iBlurWeight / 2;
    //    iBlurMax = iBlurWeight / 2 + 1;
    //}
    
    //[loop]  // 변수로 루프돌리려면 반드시 필요함
    
    for (int i = -15; i < 16; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_vResolution.x;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 15] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor;
    
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
}
