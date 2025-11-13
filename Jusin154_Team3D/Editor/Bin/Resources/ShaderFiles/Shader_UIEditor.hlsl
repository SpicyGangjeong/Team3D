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

PS_OUT PS_Key_Hold_Rotation(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Color.a <= 0.1f)
        discard;
    
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
