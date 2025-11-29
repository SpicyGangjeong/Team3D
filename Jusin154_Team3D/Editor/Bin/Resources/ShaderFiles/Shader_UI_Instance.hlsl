#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_Texture1;
Texture2D g_Texture2;
Texture2D g_Texture3;
Texture2D g_Texture4;
Texture2D g_Texture5;
Texture2D g_MaskTexture;

float g_fAlpha;
float g_fOwnerAlpha;
float g_fCanvasAlpha;
float g_fAngle;
float g_fFar;
float g_fTime;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vSize : TEXCOORD1;
    float2 vPos : TEXCOORD2;
    float4 vUV : TEXCOORD3;
    float4 vColor : TEXCOORD4;
    float bHover : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float bHover : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    float3 vLocalPos = In.vPosition;
    if (In.bHover != 0)
        vLocalPos.xy *= In.vSize * 1.1f;
    else
        vLocalPos.xy *= In.vSize;
    
    vLocalPos.xy += In.vPos;
    float4 UV = In.vUV;
    float3 vParentPos = float3(g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43);

    float3 vFinalWorldPos = vLocalPos + vParentPos;
    
    Out.vPosition = mul(float4(vFinalWorldPos, 1.f), matVP);
    Out.vTexcoord = UV.xy + In.vTexcoord * (UV.zw - UV.xy);
    Out.vColor = In.vColor;
    Out.bHover = In.bHover;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float bHover : TEXCOORD2;
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
    
    if (color.a <= 0.4f)
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

PS_OUT PS_Spell_List(PS_IN In)
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

PS_OUT PS_Eessential_Spell_Slot(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex1 = g_Texture.Sample(ClampSampler, In.vTexcoord);
    float4 tex2 = g_Texture1.Sample(ClampSampler, In.vTexcoord);
    
    color = tex1;
    
    color = lerp(tex1, tex2, tex2.a);
        
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Eessential_Spell(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex1 = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    color = tex1;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Color_Image(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);

    float4 tex1 = g_Texture.Sample(ClampSampler, In.vTexcoord);
    color = tex1;
    color *= In.vColor;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Spell_Hover(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float3 HoverOff = g_Texture2.Sample(DefaultSampler, In.vTexcoord).rgb;
    float3 HoverOn = g_Texture3.Sample(DefaultSampler, In.vTexcoord).rgb;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (In.bHover != 0)
    {
        float tex1 = g_Texture5.Sample(DefaultSampler, In.vTexcoord).r;
        color.r *= tex1.r;
        float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
        color = lerp(tex1, tex2, tex2.a);
        color.rgb = HoverOn;
    }
    else
    {
        float tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord).r;
        color.r *= tex1.r;
        float4 tex2 = g_Texture4.Sample(DefaultSampler, In.vTexcoord);
        color = lerp(tex1, tex2, tex2.a);
        color.rgb = HoverOff;
        color.a *= 0.5f;
    }
    

    color *= In.vColor;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Spell_Effect(PS_IN In)
{
    PS_OUT Out;
    
    if (In.bHover == 0)
        discard;
    
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float2 maskUV = In.vTexcoord + g_fTime * float2(0.2f, 0.0f);

    //float2 center = float2(0.5, 0.5);
    //float2 uv = In.vTexcoord - center;
    //float angle = -g_fTime;
    //float s = sin(angle);
    //float c = cos(angle);
    //float2 rotUV = float2(
    //uv.x * c - uv.y * s,
    //uv.x * s + uv.y * c
    //) + center;
    
    //float2 maskUV = In.vTexcoord;

    //maskUV.x += g_fTime * float2(0.05, 0.02); // 속도 조절 가능
    
    //float2 center = float2(0.5, 0.0); 
    //float2 uv = In.vTexcoord - center;

    //float scale = 0.5 + sin(g_fTime * 20) * 0.5; 
    //uv.x *= scale;

    //uv += center;
    
    float tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord).r;
    float mask = g_MaskTexture.Sample(DefaultSampler, maskUV).r;

    color.a *= tex1;
    color.a *= mask;
    color.a *= Alpha;

    color *= In.vColor;
    
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

    pass Spell_List
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_List();
    }

    pass Eessential_Spell_Slot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Eessential_Spell_Slot();
    }

    pass Eessential_Spell
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Eessential_Spell();
    }

    pass Color_Image
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Color_Image();
    }

    pass Spell_Hover
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Hover();
    }

    pass Spell_Effect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Effect();
    }
}
