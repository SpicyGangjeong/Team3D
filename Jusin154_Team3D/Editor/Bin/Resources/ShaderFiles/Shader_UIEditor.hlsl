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

Texture2D g_Texture;
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

PS_OUT PS_MAIN(PS_IN In) // 일반 UI
{
    PS_OUT Out;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Color.a <= 0.1f)
        discard;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Cursor(PS_IN In) // 마우스 커서
{
    PS_OUT Out;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);                          // Out에 출력하기 위한 값
    float4 White = float4(1.f, 1.f, 1.f, 1.f);                          // 마지막에 하얀색으로 바꿔주기 위한 값 
    
    float2 UV = In.vTexcoord;                                           // UV를 돌리기 위해서 받아옴
    float2 Center = float2(0.5f, 0.5f);                                 // 이미지를 회전 하기 전에 중심으로 옮길 값
    
    float CW = -g_fTime;                                                // 시계 방향
    float CCW = g_fTime;                                                // 반시계 방향
    
    float2 RedCircle = UV;                                              // 빨간 원을 반시계 방향으로 돌려주기 위한 준비
    float2 RedUV = UV - Center;                                         // 중심으로 옮겨줌
                                                                        // 값을 돌리기 위해서 회전 행렬을 만들어 준다.
    RedCircle.x = RedUV.x * cos(CCW) - RedUV.y * sin(CCW);              // x축은 x*cos(각도) - y*sin(각도)
    RedCircle.y = RedUV.x * sin(CCW) + RedUV.y * cos(CCW);              // y축은 x*sin(각도) + y*cos(각도)
    RedCircle = RedCircle + Center;                                     // 이제 위에서 다 돌려주면 다시 중심에서 원점으로 옮겨준다.
    
    float2 BlueCircle = UV;                                             // 파란색은 시계 방향으로 돌려준다
    float2 BlueUV = UV - Center;                                        // 중심으로 옮겨줌
                                                                        // 값을 돌리기 위해서 회전 행렬을 만들어 준다.
    BlueCircle.x = BlueUV.x * cos(CW) - BlueUV.y * sin(CW);             // x축은 x*cos(각도) - y*sin(각도)
    BlueCircle.y = BlueUV.x * sin(CW) + BlueUV.y * cos(CW);             // y축은 x*sin(각도) + y*cos(각도)
    BlueCircle = BlueCircle + Center;                                   // 이제 위에서 다 돌려주면 다시 중심에서 원점으로 옮겨준다.
    
                                                                        // 두개 다 같은 이미지 인데 원의 색상이 2개임
    float4 Texture1 = g_Texture.Sample(ClampSampler, RedCircle);        // 큰 원은 빨간색임
    float4 Texture2 = g_Texture2.Sample(ClampSampler, BlueCircle);      // 작은 원은 파란색임
    
    if (Texture1.r >= 0.7f)                                             // 우선 빨간 원을 먼저 그려준다.
        Color = Texture1;
    
    if (Texture2.b >= 0.7f)                                             // 다음으로 파란 원을 그려준다.
        Color = Texture2;

    if (all(Color.rgb <= 0.5f))                                         // 이제 혹시 모를 색상이 들어갈 수 있어서 한번 정리 해준다.
        discard;
    
    Color = White; // 하얀색으로 바꿔줌
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Sptire_Sheet(PS_IN In)
{
    PS_OUT Out;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);                              // 최종 이미지 출력
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float2 UV = In.vTexcoord;                                               // 이미지의 UV값
    
    int iTotalFrame = g_iImageCountX * g_iImageCountY;                        // 이미지의 최대 프레임 (몇 곱하기 몇인지)
    
    int iCurrentFrame = int(floor(g_fTime / g_fFrame)) % iTotalFrame;       // 현재 프레임이 어디 위치인지
    
    int iFrameX = iCurrentFrame % g_iImageCountX;                            // 현재 x축의 위치(현재 이미지의 몇번째 칸을 보여줄 지)
    int iFrameY = iCurrentFrame / g_iImageCountX;                            // 현재 y축의 위치(현재 이미지의 몇번째 줄을 보여줄 지)
    
    float fFreamWidth = 1.0 / g_iImageCountX;                                // 1.0 나누기 이미지 갯수를 해서 한칸에 얼마나 갈지 정해준다.
    float fFreamHeight = 1.0 / g_iImageCountY;                               // 1.0 나누기 이미지 갯수를 해서 한줄에 얼마나 갈지 정해준다.
    
    UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth;                      // 먼저 uv를 0~1이 아닌 0~fFrameWidth로 만든 다음에 한칸씩 옆으로 밀어준다.
    UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight;                    // 먼저 uv를 0~1이 아닌 0~fFrameHeight로 만든 다음에 한줄씩 밑으로 내려준다.
    
    float4 Texture = g_Texture.Sample(DefaultSampler, UV);
    
    Color = Texture;
    
    if (Color.r <= 0.4f)
        discard;
    
    Color.a = Alpha;
    
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

PS_OUT PS_Key_Hold(PS_IN In)
{
    PS_OUT Out;
    
    float3 Grey = float3(128.f, 128.f, 128.f) / 255.f;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);

        
    
    if (Color.r >= 0.3f)
        Color.rgb = Grey;
    
    Out.vColor = Color;
    
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

    pass Key_Hold
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Key_Hold();
    }
}
