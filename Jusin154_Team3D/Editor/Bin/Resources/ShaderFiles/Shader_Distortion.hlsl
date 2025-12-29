#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


Texture2D g_DistortionTexture;
Texture2D g_SceneTexture;

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
    PS_OUT Out = (PS_OUT)0;
    

    /* 디스토션 렌더타겟 텍스쳐 샘플링 */
    vector vMtrlDistortion = g_DistortionTexture.Sample(PointSampler, In.vTexcoord);
    vector vMtrlBackBuffer = g_SceneTexture.Sample(PointSampler, In.vTexcoord);

    /* r 성분을 디스토션할 성분으로 잡아놨으니까, r이 0이라면 리턴. */
    
    if (vMtrlDistortion.r == 0)
    {
        Out.vBackBuffer = vMtrlBackBuffer;
        return Out;
    }

 
    // 0 ~ 1 -> -0.5 ~ 0.5
    float2 vCenteredUV = In.vTexcoord - float2(0.5f, 0.5f);
    
       
    float fDistortR2 = vMtrlDistortion.r * vMtrlDistortion.r;
    float fDistortR4 = fDistortR2 * fDistortR2;
    float fDistortIntensity = vMtrlDistortion.g;
    float fDistortionFactor = 1.0f + (fDistortIntensity * (fDistortR2 + fDistortR2 * fDistortR4));
    
    //float fDistortionFactor = 1 + vMtrlDistortion.g * vMtrlDistortion.r * vMtrlDistortion.r;
    //너무 낮은값 쓰면 찢어지는 효과 남. (같은 픽셀 계속 디스토션 돌려서)
    //fDistortionFactor = clamp(fDistortionFactor, 0.2f, 2.0f);

    vCenteredUV *= fDistortionFactor;
    vCenteredUV += float2(0.5f, 0.5f);
 
    float4 vResult = g_SceneTexture.Sample(MirrorSampler, vCenteredUV);
    
    Out.vBackBuffer = vResult;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
 
}
