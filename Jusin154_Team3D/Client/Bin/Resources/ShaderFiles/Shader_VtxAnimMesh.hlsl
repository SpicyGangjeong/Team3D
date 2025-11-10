
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

bool g_bRimLight;
bool g_bUseNormalMap;

float g_fFar;
float g_LifeRatio;
float g_fRimStrength;
float g_fRimPower;
float3 g_vRimColor;
vector g_vCamPosition;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;

matrix g_BoneMatrices[512];


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        mul(g_BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x) +
        mul(g_BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y) +
        mul(g_BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z) +
        mul(g_BoneMatrices[In.vBlendIndex.w], fWeightW);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    vector vBinormal = mul(vector(In.vBinormal, 0.f), BoneMatrix);
    vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
    

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vBinormal, g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct VS_OUT_CAPTUREDMODEL
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_CAPTUREDMODEL VS_CAPTUREDMODEL(VS_IN In)
{
    VS_OUT_CAPTUREDMODEL Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = 
        mul(g_BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x ) +
        mul(g_BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y ) +
        mul(g_BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z ) +
        mul(g_BoneMatrices[In.vBlendIndex.w], fWeightW);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    return Out;
}

VS_OUT_BLUR VS_MAIN_BLUR(VS_IN In)
{
    VS_OUT_BLUR Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        mul(g_BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x) +
        mul(g_BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y) +
        mul(g_BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z) +
        mul(g_BoneMatrices[In.vBlendIndex.w], fWeightW);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    /* ?ㅽ궎??*/
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
   
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
    {
        discard;
    }
    
    vector vNormalDesc = g_NormalTexture.Sample(MirrorSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);
    
    if (true == g_bRimLight)
    {
        float fRimLight = GetRimLight(g_vCamPosition.xyz, In.vWorldPos.xyz, vNormal, g_fRimPower, g_fRimStrength);
        vMtrlDiffuse.xyz = (vMtrlDiffuse.xyz * (1 - fRimLight)) + g_vRimColor * fRimLight;
    }
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.f);
    
    
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vShadowLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.z;
    
    return Out;
}

struct PS_IN_CAPTUREDMODEL
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_CAPTUREDMODEL
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_CAPTUREDMODEL PS_CAPTUREDMODEL(PS_IN_CAPTUREDMODEL In)
{
    PS_OUT_CAPTUREDMODEL Out = (PS_OUT_CAPTUREDMODEL) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = saturate(1 - g_LifeRatio) * 0.5f;
    return Out;
}

struct PS_IN_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_BLUR
{
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT_BLUR PS_MAIN_BLUR(PS_IN_BLUR In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = saturate(vMtrlDiffuse);
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ShadowPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass CAPTUREDMODELPASS
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTUREDMODEL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CAPTUREDMODEL();
    }
    pass BlurPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_BLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }
}
