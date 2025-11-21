
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

bool g_bUVTargetDiffuse;
bool g_bAdditiveDisolve;

float g_LifeRatio;
float g_fFar;
float g_fTime;
float g_fDeltaU;
float g_fDeltaV;
uint g_iIndexU;
uint g_iIndexV;

float g_fDisolveRatio;
float g_fUsingSurfaceParams;

float2 g_vClipBoxSize;
float2 g_vClipBoxLTPos;

Texture2D g_DiffuseTexture;
Texture2D g_MaskingTexture;
Texture2D g_ClippingTexture;
Texture2D g_DisolveTexture;
Texture2D g_NormalTexture;
Texture2D g_GlowTexture;
Texture2D g_SurfaceParamsTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct VS_OUT_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};


VS_OUT_BLUR VS_MAIN_BLUR(VS_IN In)
{
    VS_OUT_BLUR Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
}
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
VS_OUT_EFFECT VS_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}
struct VS_OUT_CAPTUREDMODEL
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_CAPTUREDMODEL VS_CAPTUREDMODEL(VS_IN In)
{
    VS_OUT_CAPTUREDMODEL Out;
    
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
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vAlbedo : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vColor : SV_Target3;
    float4 vSurface : SV_Target4;
};
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSurface = g_SurfaceParamsTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.2f) { discard; }
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
    (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
    g_fUsingSurfaceParams,  // 서페이스 파라미터
    1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    
    return Out;
}

PS_OUT PS_MAIN_SELECT(PS_IN In)
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
    
    Out.vAlbedo = vMtrlDiffuse * float4(0.f, 1.f, 1.f, 0.3f);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.f);
    return Out;
}

PS_OUT PS_GLASS(PS_IN In)
{
    PS_OUT Out;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vNormalDesc = g_NormalTexture.Sample(MirrorSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);
    
    Out.vAlbedo = lerp(vMtrlDiffuse, float4(1.f, 1.f, 1.f, 1.f), 0.5f);
    float4(vMtrlDiffuse.xyz, 1.f);
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

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.vShadowLightDepth.x = In.vProjPos.z;
    
    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_EFFECT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT_EFFECT PS_EFFECT_UVMOVE(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out;
    
    float2 vMovedUV = Get_MovedUV(In.vTexcoord, g_fDeltaU, g_fDeltaV, g_iIndexU, g_iIndexV);
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, vMovedUV);
    
    if (vDiffuse.a < 0.01f)
    {
        discard;
    }
    
    Out.vColor = vDiffuse;
    
    return Out;
}

PS_OUT_EFFECT PS_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out;
    
    float2 uv = In.vTexcoord;
    float2 vClipBoxUV = g_vClipBoxLTPos + g_vClipBoxSize * In.vTexcoord;
    uv.x += g_fDeltaU * g_fTime;
    uv.y += g_fDeltaV * g_fTime;

    vector vDiffuse;
    vector vMasking;
    vector vClipCell = g_ClippingTexture.Sample(DefaultSampler, vClipBoxUV);
    vector vDisolveCell = g_DisolveTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bUVTargetDiffuse)
    {
        vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
        vMasking = g_MaskingTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        vMasking = g_MaskingTexture.Sample(DefaultSampler, uv);
    }
    if (true == g_bAdditiveDisolve)
    {
        vDiffuse.a = saturate(vDiffuse.a * vMasking.r * vClipCell.r + vDisolveCell.r * g_fDisolveRatio);
    }
    else
    {
        vDiffuse.a = saturate(vDiffuse.a * vMasking.r * vClipCell.r - vDisolveCell.r * g_fDisolveRatio);
    }
    
    if (vDiffuse.a < 0.01f)
    {
        discard;
    }
    
    Out.vColor = vDiffuse;
    
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
technique11 MeshTechnique11
{
    pass MeshPass // 0
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
 
    pass SkyBoxPass // 2
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ALPHABlendPass // 3
    {
        SetRasterizerState(RS_Nocull_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }



    pass EffectPass_AlphaBlend // 4
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass EffectPass_Blend // 5
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass EffectPass_AlphaBlend_Cull // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass EffectPass_Blend_Cull // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass CAPTUREDMODELPASS // 8
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
    pass UVMovePass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_UVMOVE();
    }

    pass MapToolPass // 11
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SELECT();
    }

    pass GlassPass // 12
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASS();
    }
}
