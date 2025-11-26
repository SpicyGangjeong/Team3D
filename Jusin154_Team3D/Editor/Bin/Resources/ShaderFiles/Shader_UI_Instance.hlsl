#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_Texture1;

float g_fAlpha;
float g_fOwnerAlpha;
float g_fCanvasAlpha;
float g_fAngle;
float g_fFar;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vSize : TEXCOORD1;
    float2 vPos : TEXCOORD2;
    float2 vUVStart : TEXCOORD3;
    float2 vUVEnd: TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    float3 vLocalPos = In.vPosition;
    vLocalPos.xy *= In.vSize;
    vLocalPos.xy += In.vPos; 
    float2 StartUV = In.vUVStart;
    float2 EndUV = In.vUVEnd;
    float3 vParentPos = float3(g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43);

    float3 vFinalWorldPos = vLocalPos + vParentPos;

    Out.vPosition = mul(float4(vFinalWorldPos, 1.f), matVP);
    Out.vTexcoord = StartUV + In.vTexcoord * (EndUV - StartUV);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    uv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;
    
    float4 color = g_Texture.Sample(ClampSampler, Rotation);
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Alpha_Blend(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    if(color.a <= 0.4f)
        discard;
        
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Megic_Meter(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float3 Blue = float3(41.f, 165.f, 255.f) / 255.f;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex1 = g_Texture.Sample(ClampSampler, In.vTexcoord);
    float4 tex2 = g_Texture1.Sample(ClampSampler, In.vTexcoord);
    
    color = tex1;
    tex2.rgb *= Blue * 1.5f;
    
    if (all(color.rgb >= float3(65.f / 255.f, 65.f / 255.f, 65.f / 255.f)))
        color = tex2;
    
    //if(color.a <= 0.4f)
    //    discard;
        
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

technique11 PosTexTechnique11
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Alpha_Blend();
    }

    pass Megic_Meter
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Megic_Meter();
    }
}
