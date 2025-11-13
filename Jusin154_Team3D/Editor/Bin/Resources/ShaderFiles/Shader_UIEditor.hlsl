#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DepthTexture;

int g_iImageCountX;
int g_iImageCountY;

float g_fFar;
float g_fTime;
float g_fFrame;
float g_fAlpha;
float g_fOwnerAlpha;
float g_fCanvasAlpha;
float g_fDeltaU;
float g_fDeltaV;
uint g_iIndexU;
uint g_iIndexV;

uint g_iQuestType;

Texture2D g_Texture;
Texture2D g_Texture1;
Texture2D g_Texture2;
Texture2D g_DiffuseTexture;
Texture2D g_MaskingTexture;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
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
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_Target0;
};

PS_OUT PS_MAIN(PS_IN In) // ?쇰컲 UI
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Color.a <= 0.1f)
        discard;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_AlphaBlend(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Clamp(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    Out.vColor = Color;

    
    return Out;
}

PS_OUT PS_Cursor(PS_IN In) 
{
    PS_OUT Out;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f); 
    float4 White = float4(1.f, 1.f, 1.f, 1.f); 
    
    float2 UV = In.vTexcoord; 
    float2 Center = float2(0.5f, 0.5f); 
    
    float CW = -g_fTime; 
    float CCW = g_fTime; 
    
    float2 RedCircle = UV; 
    float2 RedUV = UV - Center; 
                                                                        
    RedCircle.x = RedUV.x * cos(CCW) - RedUV.y * sin(CCW); 
    RedCircle.y = RedUV.x * sin(CCW) + RedUV.y * cos(CCW); 
    RedCircle = RedCircle + Center; 
    
    float2 BlueCircle = UV; 
    float2 BlueUV = UV - Center;
                                                                        
    BlueCircle.x = BlueUV.x * cos(CW) - BlueUV.y * sin(CW); 
    BlueCircle.y = BlueUV.x * sin(CW) + BlueUV.y * cos(CW); 
    BlueCircle = BlueCircle + Center; 
    
                                                                   
    float4 Texture1 = g_Texture.Sample(ClampSampler, RedCircle);   
    float4 Texture2 = g_Texture2.Sample(ClampSampler, BlueCircle); 
    
    if (Texture1.r >= 0.7f)                                            
        Color = Texture1;
    
    if (Texture2.b >= 0.7f)                                           
        Color = Texture2;

    if (all(Color.rgb <= 0.5f))                                         
        discard;
    
    Color = White; 
    
    return Out;
}

PS_OUT PS_Cursor(PS_IN In) // 留덉슦??而ㅼ꽌
{
    PS_OUT Out;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f); // Out??異쒕젰?섍린 ?꾪븳 媛?
    float4 White = float4(1.f, 1.f, 1.f, 1.f); // 留덉?留됱뿉 ?섏??됱쑝濡?諛붽퓭二쇨린 ?꾪븳 媛?
    
    float2 UV = In.vTexcoord; // UV瑜??뚮━湲??꾪빐??諛쏆븘??
    float2 Center = float2(0.5f, 0.5f); // ?대?吏瑜??뚯쟾 ?섍린 ?꾩뿉 以묒떖?쇰줈 ??만 媛?
    
    float CW = -g_fTime; // ?쒓퀎 諛⑺뼢
    float CCW = g_fTime; // 諛섏떆怨?諛⑺뼢
    
    float2 RedCircle = UV; // 鍮④컙 ?먯쓣 諛섏떆怨?諛⑺뼢?쇰줈 ?뚮젮二쇨린 ?꾪븳 以鍮?
    float2 RedUV = UV - Center; // 以묒떖?쇰줈 ??꺼以?
                                                                        // 媛믪쓣 ?뚮━湲??꾪빐???뚯쟾 ?됰젹??留뚮뱾??以??
    RedCircle.x = RedUV.x * cos(CCW) - RedUV.y * sin(CCW); // x異뺤? x*cos(媛곷룄) - y*sin(媛곷룄)
    RedCircle.y = RedUV.x * sin(CCW) + RedUV.y * cos(CCW); // y異뺤? x*sin(媛곷룄) + y*cos(媛곷룄)
    RedCircle = RedCircle + Center; // ?댁젣 ?꾩뿉?????뚮젮二쇰㈃ ?ㅼ떆 以묒떖?먯꽌 ?먯젏?쇰줈 ??꺼以??
    
    float2 BlueCircle = UV; // ?뚮??됱? ?쒓퀎 諛⑺뼢?쇰줈 ?뚮젮以??
    float2 BlueUV = UV - Center; // 以묒떖?쇰줈 ??꺼以?
                                                                        // 媛믪쓣 ?뚮━湲??꾪빐???뚯쟾 ?됰젹??留뚮뱾??以??
    BlueCircle.x = BlueUV.x * cos(CW) - BlueUV.y * sin(CW); // x異뺤? x*cos(媛곷룄) - y*sin(媛곷룄)
    BlueCircle.y = BlueUV.x * sin(CW) + BlueUV.y * cos(CW); // y異뺤? x*sin(媛곷룄) + y*cos(媛곷룄)
    BlueCircle = BlueCircle + Center; // ?댁젣 ?꾩뿉?????뚮젮二쇰㈃ ?ㅼ떆 以묒떖?먯꽌 ?먯젏?쇰줈 ??꺼以??
    
                                                                        // ?먭컻 ??媛숈? ?대?吏 ?몃뜲 ?먯쓽 ?됱긽??2媛쒖엫
    float4 Texture1 = g_Texture.Sample(ClampSampler, RedCircle); // ???먯? 鍮④컙?됱엫
    float4 Texture2 = g_Texture2.Sample(ClampSampler, BlueCircle); // ?묒? ?먯? ?뚮??됱엫
    
    if (Texture1.r >= 0.7f)                                             // ?곗꽑 鍮④컙 ?먯쓣 癒쇱? 洹몃젮以??
        Color = Texture1;
    
    if (Texture2.b >= 0.7f)                                             // ?ㅼ쓬?쇰줈 ?뚮? ?먯쓣 洹몃젮以??
        Color = Texture2;

    if (all(Color.rgb <= 0.5f))                                         // ?댁젣 ?뱀떆 紐⑤? ?됱긽???ㅼ뼱媛????덉뼱???쒕쾲 ?뺣━ ?댁???
        discard;
    
    Color = White; // ?섏??됱쑝濡?諛붽퓭以?
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Sptire_Sheet(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f); // 理쒖쥌 ?대?吏 異쒕젰
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float2 UV = In.vTexcoord; // ?대?吏??UV媛?
    
    int iTotalFrame = g_iImageCountX * g_iImageCountY; // ?대?吏??理쒕? ?꾨젅??(紐?怨깊븯湲?紐뉗씤吏)
    
    int iCurrentFrame = int(floor(g_fTime / g_fFrame)) % iTotalFrame; // ?꾩옱 ?꾨젅?꾩씠 ?대뵒 ?꾩튂?몄?
    
    int iFrameX = iCurrentFrame % g_iImageCountX; // ?꾩옱 x異뺤쓽 ?꾩튂(?꾩옱 ?대?吏??紐뉖쾲吏?移몄쓣 蹂댁뿬以?吏)
    int iFrameY = iCurrentFrame / g_iImageCountX; // ?꾩옱 y異뺤쓽 ?꾩튂(?꾩옱 ?대?吏??紐뉖쾲吏?以꾩쓣 蹂댁뿬以?吏)
    
    float fFreamWidth = 1.0 / g_iImageCountX; // 1.0 ?섎늻湲??대?吏 媛?닔瑜??댁꽌 ?쒖뭏???쇰쭏??媛덉? ?뺥빐以??
    float fFreamHeight = 1.0 / g_iImageCountY; // 1.0 ?섎늻湲??대?吏 媛?닔瑜??댁꽌 ?쒖쨪???쇰쭏??媛덉? ?뺥빐以??
    
    UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth; // 癒쇱? uv瑜?0~1???꾨땶 0~fFrameWidth濡?留뚮뱺 ?ㅼ쓬???쒖뭏???놁쑝濡?諛?댁???
    UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight; // 癒쇱? uv瑜?0~1???꾨땶 0~fFrameHeight濡?留뚮뱺 ?ㅼ쓬???쒖쨪??諛묒쑝濡??대젮以??
    
    float4 Texture = g_Texture.Sample(DefaultSampler, UV);
    
    Color = Texture;
    
    if (Color.r <= 0.4f)
        discard;
    
    Color.a = Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Key_Hold_Rotation(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Color.a <= 0.1f)
        discard;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_QuestType(PS_IN In)
{
    PS_OUT Out;
    float3 Color = float3(253.f, 207.f, 11.f) / 255.f;
    float4 Texture1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (g_iQuestType == 0)
    {
        if (Texture1.r >= 0.3f)
        {
            Texture1.rgb = Color;
        }
    }

    Out.vColor = Texture1;
    
    return Out;
}

PS_OUT PS_UVMult(PS_IN In)
{
    PS_OUT Out;
    
    float2 UV = In.vTexcoord;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);
    
    float4 Texture1 = g_Texture.Sample(ClampSampler, float2(UV.x * g_fDeltaU, UV.y * g_fDeltaV));
    
    Color = Texture1;

    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Sptire_Sheet(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f); 
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float2 UV = In.vTexcoord;
    
    int iTotalFrame = g_iImageCountX * g_iImageCountY;
    
    int iCurrentFrame = int(floor(g_fTime / g_fFrame)) % iTotalFrame; 
    
    int iFrameX = iCurrentFrame % g_iImageCountX; 
    int iFrameY = iCurrentFrame / g_iImageCountX; 
    
    float fFreamWidth = 1.0 / g_iImageCountX;  
    float fFreamHeight = 1.0 / g_iImageCountY; 
    
    UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth;  
    UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight;
    
    float4 Texture = g_Texture.Sample(DefaultSampler, UV);
    
    Color = Texture;
    
    if (Color.r <= 0.4f)
        discard;
    
    Color.a = Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Key_Hold_Rotation(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Color.a <= 0.1f)
        discard;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_QuestType(PS_IN In)
{
    PS_OUT Out;
    float3 Color = float3(253.f, 207.f, 11.f) / 255.f;
    float4 Texture1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (g_iQuestType == 0)
    {
        if (Texture1.r >= 0.3f)
        {
            Texture1.rgb = Color;
        }
    }

    Out.vColor = Texture1;
    
    return Out;
}

PS_OUT PS_UVMult(PS_IN In)
{
    PS_OUT Out;

    float4 color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = color;

    return Out;
}

technique11 PosTexTechnique11
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

    pass AlphaBlend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_AlphaBlend();
    }

    pass TextureClamp
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Clamp();
    }

    pass Mouse_Cursor
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Cursor();
    }

    pass Sptire_Sheet
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_UIBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Sptire_Sheet();
    }

    pass Key_Hold_Rotation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Key_Hold_Rotation();
    }

    pass QuestType
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_QuestType();
    }

    pass UVMult
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UVMult();
    }
}
