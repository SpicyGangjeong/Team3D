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

float4 g_UV;
float2 g_fOrigin_Size;
float2 g_fCurrent_Size;
float4 g_fNine_Slice;
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float2 vSize : TEXCOORD1;
    float2 vPos : TEXCOORD2;
    float4 vUV : TEXCOORD3;
    float vBaseColor : TEXCOORD4;
    float bHover : TEXCOORD5;
    float bSpell : TEXCOORD6;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vUV : TEXCOORD1;
    float vBaseColor : TEXCOORD2;
    float bHover : TEXCOORD3;
    float bSpell : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    float3 vLocalPos = In.vPosition;
    if (In.bHover == 1)
        vLocalPos.xy *= In.vSize * 1.1f;
    else
        vLocalPos.xy *= In.vSize;
    
    vLocalPos.xy += In.vPos;
    float4 UV = In.vUV;
    float3 vParentPos = float3(g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43);

    float3 vFinalWorldPos = vLocalPos + vParentPos;
    
    Out.vPosition = mul(float4(vFinalWorldPos, 1.f), matVP);
    Out.vUV = UV.xy + In.vTexcoord * (UV.zw - UV.xy);
    Out.vTexcoord = In.vTexcoord;
    Out.vBaseColor = In.vBaseColor;
    Out.bHover = In.bHover;
    Out.bSpell = In.bSpell;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vUV : TEXCOORD1;
    float vBaseColor : TEXCOORD2;
    float bHover : TEXCOORD3;
    float bSpell : TEXCOORD4;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
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
    PS_OUT Out = (PS_OUT)0;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    if (color.a <= 0.4f)
        discard;
        
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_UVAlpha_Blend(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = g_Texture.Sample(ClampSampler, In.vUV);
    
    if (color.a <= 0.4f)
        discard;
        
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Megic_Meter(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
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
    PS_OUT Out = (PS_OUT)0;
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
    PS_OUT Out = (PS_OUT)0;
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
    PS_OUT Out = (PS_OUT)0;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex1 = g_Texture.Sample(ClampSampler, In.vUV);
    
    color = tex1;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Color_Image(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    float3 BGColor = float3(1.f, 1.f, 1.f);
    switch (In.vBaseColor)
    {
        case 0:
            BGColor = float3(220.f, 165.f, 70.f) / 255.f;
            break;
        case 1:
            BGColor = float3(150.f, 120.f, 240.f) / 255.f;
            break;
        case 2:
            BGColor = float3(190.f, 46., 34.f) / 255.f;
            break;
        case 3:
            BGColor = float3(24.f, 190.f, 255.f) / 255.f;
            break;
        case 4:
            BGColor = float3(200.f, 220.f, 150.f) / 255.f;
            break;
        case 5:
            BGColor = float3(50.f, 150.f, 110.f) / 255.f;
            break;

    }
    
    float4 tex1 = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    color = tex1;
    color.rgb *= BGColor;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Spell_Hover(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
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
        
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Spell_Effect(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
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
    //float2 maskUV = float2(
    //uv.x * c - uv.y * s,
    //uv.x * s + uv.y * c
    //) + center;
    
    //float2 maskUV = In.vTexcoord;

    //maskUV.x += g_fTime * float2(0.05, 0.02);
    
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
    
    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Spell_Slot(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    uv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;
    
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
        
    if (In.bSpell != 0)
    {
        float3 BGColor = float3(1.f, 1.f, 1.f);
        switch (In.vBaseColor)
        {
            case 0:
                BGColor = float3(220.f, 165.f, 70.f) / 255.f;
                break;
            case 1:
                BGColor = float3(150.f, 120.f, 240.f) / 255.f;
                break;
            case 2:
                BGColor = float3(190.f, 46., 34.f) / 255.f;
                break;
            case 3:
                BGColor = float3(24.f, 190.f, 255.f) / 255.f;
                break;
            case 4:
                BGColor = float3(200.f, 220.f, 150.f) / 255.f;
                break;
            case 5:
                BGColor = float3(50.f, 150.f, 110.f) / 255.f;
                break;
        }
        float4 tex1 = g_Texture.Sample(ClampSampler, Rotation);
        tex1.rgb *= BGColor;
        Color = tex1;

        float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vUV);
        Color = lerp(Color, tex2, tex2.a);
        
        float4 tex3 = g_Texture2.Sample(ClampSampler, Rotation);
        Color = lerp(Color, tex3, tex3.a);
    }
    
    else
    {
        float4 tex3 = g_Texture2.Sample(ClampSampler, Rotation);
        Color = tex3;
    }
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Spell_Lock(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    if(In.bSpell == 0)
    {
        if (color.a <= 0.4f)
            discard;
    }
        
    else
        discard;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Quest_Slot(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 uv = In.vTexcoord;
    float2 CurrentPixelPosition = uv * g_fCurrent_Size;
    float OriginLeft = g_fNine_Slice.x;
    float OriginRight = g_fNine_Slice.y;
    float OriginTop = g_fNine_Slice.z;
    float OriginBottom = g_fNine_Slice.w;
    
    float CurrentLeft = OriginLeft;
    float CurrentRight = g_fCurrent_Size.x - (g_fOrigin_Size.x - OriginRight);
    float CurrentTop = OriginTop;
    float CurrentBottom = g_fCurrent_Size.y - (g_fOrigin_Size.y - OriginBottom);
    
    float2 Finaluv = In.vTexcoord;

    if (CurrentPixelPosition.x < CurrentLeft)
    {
        Finaluv.x = CurrentPixelPosition.x / g_fOrigin_Size.x;
    }
    else if (CurrentPixelPosition.x > CurrentRight)
    {
        float dist = CurrentPixelPosition.x - CurrentRight;
        Finaluv.x = (OriginRight + dist) / g_fOrigin_Size.x;
    }
    else
    {
        float scale = (CurrentPixelPosition.x - CurrentLeft) / (CurrentRight - CurrentLeft);
        Finaluv.x = (OriginLeft / g_fOrigin_Size.x) + scale * ((OriginRight - OriginLeft) / g_fOrigin_Size.x);
    }
    
    if (CurrentPixelPosition.y < CurrentTop)
    {
        Finaluv.y = CurrentPixelPosition.y / g_fOrigin_Size.y;
    }
    else if (CurrentPixelPosition.y > CurrentBottom)
    {
        float dist = CurrentPixelPosition.y - CurrentBottom;
        Finaluv.y = (OriginBottom + dist) / g_fOrigin_Size.y;
    }
    else
    {
        float scale = (CurrentPixelPosition.y - CurrentTop) / (CurrentBottom - CurrentTop);
        Finaluv.y = (OriginTop / g_fOrigin_Size.y) + scale * ((OriginBottom - OriginTop) / g_fOrigin_Size.y);
    }
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, Finaluv);
    
    color = tex1;
    
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

    pass UVAlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UVAlpha_Blend();
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

    pass Spell_Slot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Slot();
    }

    pass Spell_Lock
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Lock();
    }

    pass Quest_Slot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Quest_Slot();
    }
}
