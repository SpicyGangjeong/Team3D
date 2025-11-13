#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_ColorTexture;
Texture2D g_DiffuseTexture;


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

PS_OUT_BACKBUFFER PS_OUT_LASTCOLOR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vBackBuffer = float4(0.f, 0.f, 0.f, 0.f);
    
    vector vColorTexture = g_ColorTexture.Sample(ClampSampler, In.vTexcoord);
    
    if (0.f == vColorTexture.a)
    {
        discard;
    }
    
    int iOption = vColorTexture.a * 10;
    

    switch (iOption)
    {
        case 1:
            Out.vBackBuffer.rgb /= vColorTexture.rgb;
            break;
        case 2:
            Out.vBackBuffer.rgb += vColorTexture.rgb;
            break;
        case 3:
            Out.vBackBuffer.rgb -= vColorTexture.rgb;
            break;
        case 4:
            Out.vBackBuffer.rgb *= vColorTexture.rgb;
            break;
        case 5:
            Out.vBackBuffer.rgb = vColorTexture.rgb;
            break;
        default:
            break;
    }
    
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUT_LASTCOLOR();
    }
 
}
