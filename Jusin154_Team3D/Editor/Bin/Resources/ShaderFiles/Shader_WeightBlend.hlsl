#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


Texture2D g_MixedDiffuseTexture;
Texture2D g_RevealageTexture;

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

struct PS_OUT
{
    float4 vBackBuffer : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    

    
    vector vMixedDiffuse = g_MixedDiffuseTexture.Sample(PointSampler, In.vTexcoord);
    float fRevealage = g_RevealageTexture.Sample(PointSampler, In.vTexcoord);

    
    Out.vBackBuffer = vector(vMixedDiffuse.rgb / fRevealage , fRevealage);
    
    //float fClamp;
    

    //fClamp = clamp(vMixedDiffuse.a, 1e-4, 5e4);
    
    //Out.vBackBuffer = vector(vMixedDiffuse.rgb / fClamp, fRevealage);
    
    
    return Out;
}

technique11 DefaultTechnique
{
    pass WeightBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
 
}
