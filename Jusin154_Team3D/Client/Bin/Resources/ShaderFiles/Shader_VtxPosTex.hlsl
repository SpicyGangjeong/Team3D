
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DepthTexture;

float   g_fFar;
float   g_fDeltaU;
float   g_fDeltaV;
uint    g_iIndexU;
uint    g_iIndexV;

Texture2D g_Texture;

Texture2D g_DiffuseTexture;
Texture2D g_MaskingTexture;
Texture2D g_NoiseTexture;

bool g_isDiffuse;
bool g_isNoise;
bool g_isMask;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    


    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
        
    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition;
    
    return Out;
}

VS_OUT VS_TRAIL(VS_IN In)
{
    VS_OUT Out;
    
    
    matrix matWVP;
    
    matWVP = mul(g_ViewMatrix , g_ProjMatrix); //월드행렬 곱해져서 오기때문에 월드 곱하지 않음

   
    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = vPosition;
    
    
    return Out;
    
}

struct PS_IN
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}
PS_OUT PS_UVMove(PS_IN In)
{
    PS_OUT Out;

    float2 vMovedUV = Get_MovedUV(In.vTexcoord, g_fDeltaU, g_fDeltaV, g_iIndexU, g_iIndexV);
    
    Out.vColor = g_Texture.Sample(DefaultSampler, vMovedUV);
    clip(Out.vColor.a - 0.01f);

    return Out;
}


struct PS_TRAILOUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vRevealage : SV_TARGET1;
    float4 vColorTarget : SV_TARGET2;
};

PS_TRAILOUT PS_Trail(PS_IN In)
{
    PS_TRAILOUT Out;
    
    vector vMtrlDiffuse;
    vector vMtrlMask;
    vector vMtrlNoise;
    
    if (g_isDiffuse == true)
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    else
        vMtrlDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    
    if (g_isMask == true)
        vMtrlMask = g_MaskingTexture.Sample(PointSampler, In.vTexcoord);
    else
        vMtrlMask = vector(1.f, 1.f, 1.f, 1.f);
    
    if (g_isNoise == true)
    {
        vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord);
        
        //vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlNoise.r);
        
        vMtrlDiffuse *= vMtrlNoise;
    }
   
    
    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlMask.r);
    

    float fWeight = clamp(pow(In.vProjPos.w, -2.5f), 1.0f, 1000.0f);
    
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb * vMtrlDiffuse.a, vMtrlDiffuse.a) * fWeight;
   
    Out.vRevealage.r = vMtrlDiffuse.a;
    Out.vColorTarget = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}


technique11 PosTexTechnique11
{
    pass PosTexPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ScissorPass
    {
        SetRasterizerState(RS_Scissor);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UIPass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_UIBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UVMovePass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UVMove();
    }

    pass TrailPass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Trail();
    }

}
