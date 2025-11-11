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
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.0f);
    
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vector vPosition;

{ // vDepthDesc과 vTexcoord.xy로부터 World Position의 추론
        /* 직육면체의 NDC */
    /* LocalPos * World * View * Proj * ( 1 / w ) */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    /* [* (1 / w)] 소거*/
    /* z나누기를 되돌림 */
    vPosition = vPosition * fViewZ;
// vPosition.x = In.vTexcoord.x * 2.f - 1.f * Far;
// vPosition.y = In.vTexcoord.y * -2.f + 1.f * Far;
// vPosition.z = vDepthDesc.x  * Far;
// vPosition.w = Far;
    
    vPosition = mul(vPosition, g_invmatProj);
    vPosition = mul(vPosition, g_invMatView);
}
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vector vPosition;
    /* 0 ~ 1 에서 -1 ~ 1 */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_invmatProj);
    vPosition = mul(vPosition, g_invMatView);
    
    vector vLightDir = vPosition - g_vLightPos;
    float fDistance = length(vLightDir);
    float fAttenuation = saturate((g_fLightRange - fDistance) / g_fLightRange);
    if (fAttenuation <= 0.f)
    {
        discard;
    }
    vLightDir = normalize(vLightDir);
    
    Out.vShade = fAttenuation * (g_vLightDiffuse * saturate(max(dot(vLightDir * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(vLightDir, vNormal);
    
    Out.vSpecular = fAttenuation * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f));
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vector vPosition;
    /* 0 ~ 1 에서 -1 ~ 1 */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_invmatProj);
    vPosition = mul(vPosition, g_invMatView);
    
    vector vLightDir = vPosition - g_vLightPos;
    float fDistance = length(vLightDir);
    float fAttenuation = saturate((g_fLightRange - fDistance) / g_fLightRange);
    if (fAttenuation <= 0.f)
    {
        discard;
    }
    vLightDir = normalize(vLightDir);
    vector vSpotDir = normalize(g_vLightDir);
    float fCosAngle = dot(vLightDir, vSpotDir);
    if (fCosAngle < g_fSpotOuterAngle)
    {
        discard;
    }
    float fSpotAttenenuation = saturate((fCosAngle - g_fSpotInnerAngle) /  (g_fSpotInnerAngle - g_fSpotOuterAngle));
    fAttenuation *= fSpotAttenenuation;
    
    Out.vShade = fAttenuation * (g_vLightDiffuse * saturate(max(dot(vLightDir * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));
    
    vector vLook = vPosition - g_vCamPosition;
    vector vReflect = reflect(vLightDir, vNormal);
    
    Out.vSpecular = fAttenuation * ((g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f));
    
    return Out;
}

float g_fWeights[13] =
{
    0.0090, 0.0218, 0.0448, 0.0784, 0.1169,
    0.1486, 0.1610, 0.1486, 0.1169, 0.0784,
    0.0448, 0.0218, 0.0090
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
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x;
        vTexcoord.y = In.vTexcoord.y + (float)i / g_vResolution.y;
        
        vColor += g_fWeights[i + 6] * g_BlurXTexture.Sample(ClampSampler, vTexcoord);
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
    
    for (int i = -6; i < 7; ++i)
    {
        vTexcoord.x = In.vTexcoord.x + (float) i / g_vResolution.x;
        vTexcoord.y = In.vTexcoord.y;
        
        vColor += g_fWeights[i + 6] * g_BlurTexture.Sample(ClampSampler, vTexcoord);
    }
    
    Out.vBlurX = vColor; 
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DebugPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass DirectionalPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass PointPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass CombinedPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass BlurPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass SpotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }
}
