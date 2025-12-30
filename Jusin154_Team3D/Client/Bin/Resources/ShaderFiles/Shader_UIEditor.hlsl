
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DepthTexture;

float3 g_CamaraPosition;

int g_iImageCountX;
int g_iImageCountY;

uint g_iIndexU;
uint g_iIndexV;
uint g_iQuestType;
int g_iSpellType;

float g_fPI;
float g_fFar;
float g_fBlinkTime;
float g_fTime;
float g_fColorTime;
float g_fFrame;
float g_fDeltaU;
float g_fDeltaV;
float g_fAlpha;
float g_fOwnerAlpha;
float g_fCanvasAlpha;
float g_fAngle;
float g_fMapAngle;
float g_fCoolTime;
float g_fHp;

float2 g_fViewport;
float2 g_fOrigin_Size;
float2 g_fCurrent_Size;
float2 g_fPosition;
float2 g_fHpBG;
float2 g_Pos;
float2 g_fImageSize;

float4 g_fNine_Slice;
float4 g_fImageUV;
float4 g_fImageUV1;

uint g_iArrayCount;
float2 g_fItemPosition1;
float2 g_fItemImageSizes1;
float2 g_fItemPosition2;
float2 g_fItemImageSizes2;
float2 g_fImageSizes[4];
float4 g_fImagesUV[4];

float4 g_fImageSipos1;
float4 g_fImageSipos2;
float4 g_fImageSipos3;

Texture2D g_Texture;
Texture2D g_Texture1;
Texture2D g_Texture2;
Texture2D g_Texture3;
Texture2D g_Texture4;
Texture2D g_Texture5;
Texture2D g_Texture6;
Texture2D g_DiffuseTexture;
Texture2D g_MaskingTexture;

int g_iHover;
int g_iBoosterOn;
int g_iBoosterOff;
int g_iClick;
int g_iColor;
int g_Count;
int g_ChaseCount;

int g_iStep1 = 0;
int g_iStep2 = 0;
int g_iStep3 = 0;

float2 g_Trail[200];
float2 g_ChaseTrail[200];


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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Texture_Color(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float4 Color = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    
    tex1.rgb *= Color.rgb;
    
    tex1.a *= Alpha;
    Out.vColor = tex1;
    return Out;
}

PS_OUT PS_Logo(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha;
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 Dissolve = g_MaskingTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (g_fTime >= Dissolve.r)
        discard;
    
    if (g_fTime != 0.f)
    {
        Color.a *= Dissolve.a;
    }
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Logo_Text(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha;
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Color.rgb = (Color.rgb - 0.5f) * 1.2f + 0.5f;

    Color.rgb = pow(Color.rgb, 1.25f);

    float luminance = dot(Color.rgb, float3(0.299f, 0.587f, 0.114f));
    float light = smoothstep(0.7f, 0.92f, luminance);
    Color.rgb *= (1.0f - light * 0.1f);

    Color.rgb = lerp(Color.rgb, Color.rgb * Color.rgb, 0.12f);

    Color.rgb *= 0.7f;

    Color.rgb = saturate(Color.rgb);

    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Logo_Glow(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord).r;
    float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    float3 Blue = float3(14.f, 180.f, 252.f) / 255.f;
    
    Color.a *= tex1;
    tex2.rgb *= Color.a * 5.f * Alpha;
    Color.rgb *= Blue;
    
    Color = lerp(Color, tex2, tex2.a);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_AlphaBlend(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    Color.rgb *= 1.5f;
    
    Color.a *= Alpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Clamp(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 Color = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    Color.rgb = saturate(Color.rgb * g_fTime);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Cursor(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
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
    
    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Key_Hold_Rotation(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;

    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Color = tex1;
    
    float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
    float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
    float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
    bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
    float4 tex2 = g_Texture1.Sample(ClampSampler, Imagelocal);
    if (inside1)
    {
        float2 center = float2(0.5f, 0.5f);
        float2 dir = Imagelocal - center;

        float angle = atan2(dir.x, -dir.y);
        if (angle < 0)
            angle += g_fPI;

        float rotation = saturate(g_fTime) * g_fPI;

        tex2.rgb = (angle <= rotation) ? 1.0 : tex2.rgb;

       
        Color = lerp(Color, tex2, tex2.a);
    }
    

    
    float2 Imagetexpos2 = g_fImageSipos2.xy / g_fCurrent_Size;
    float2 Imagetexsize2 = g_fImageSipos2.zw / g_fCurrent_Size;
    float2 Imagelocal2 = (In.vTexcoord - Imagetexpos2) / Imagetexsize2;
    bool inside2 = all(Imagelocal2 >= 0.0f && Imagelocal2 <= 1.0f);
    if (inside2)
    {
        float2 atlasUV = g_fImageUV.xy + Imagelocal2 * (g_fImageUV.zw - g_fImageUV.xy);
        float2 safeUV = saturate(atlasUV);
        float4 tex3 = g_Texture2.Sample(ClampSampler, safeUV);
        Color = lerp(Color, tex3, tex3.a);
    }

    float2 Imagetexpos3 = g_fImageSipos3.xy / g_fCurrent_Size;
    float2 Imagetexsize3 = g_fImageSipos3.zw / g_fCurrent_Size;
    float2 Imagelocal3 = (In.vTexcoord - Imagetexpos3) / Imagetexsize3;
    bool inside3 = all(Imagelocal3 >= 0.0f && Imagelocal3 <= 1.0f);
    if (inside3)
    {
        float3 QuestColor = float3(253.f, 207.f, 11.f) / 255.f;
        
        float4 tex4 = g_Texture3.Sample(ClampSampler, Imagelocal3);
        
        if (g_iQuestType == 0)
        {
 
            tex4.rgb *= QuestColor;
            
        }
        
        Color = lerp(Color, tex4, tex4.a);
    }

    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Mission(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    
    tex1.a *= 0.5f;

    if (tex1.a >= 0.f)
        Color = tex1;
    
    Color = lerp(Color, tex2, tex2.a);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Sptire_Sheet(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(0.f, 0.f, 0.f, 0.f);
    float2 UV = In.vTexcoord;
    
    int iTotalFrame = g_iImageCountX * g_iImageCountY;
    
    uint iCurrentFrame = uint(floor(g_fTime / g_fFrame)) % iTotalFrame;
    
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
    

    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_QuestType(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float3 Color = float3(253.f, 207.f, 11.f) / 255.f;
    float4 Texture1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (g_iQuestType == 0)
    {
        if (Texture1.r >= 0.3f)
        {
            Texture1.rgb = Color;
        }
    }

    Texture1.a *= Alpha;
    Out.vColor = Texture1;
    
    return Out;
}

PS_OUT PS_Spell_Anim(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Color = tex1;
    
    float2 texpos = g_fImageSipos1.xy / g_fCurrent_Size;
    float2 texsize = g_fImageSipos1.zw / g_fCurrent_Size;
    float2 texlocal = (In.vTexcoord - texpos) / texsize;
    bool inside = all(texlocal >= 0.0f && texlocal <= 1.0f);
    if (inside)
    {
        float4 tex2 = g_Texture1.Sample(DefaultSampler, texlocal);
        Color = lerp(Color, tex2, tex2.a);
    }
    
    Color.a *= Alpha;
    Out.vColor = Color;

    return Out;
}

PS_OUT PS_NineSlice(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
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
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Finaluv);
    float4 tex2 = g_Texture1.Sample(ClampSampler, Finaluv);
    
    tex1.a *= 0.5f;

    if (tex1.a >= 0.f)
        Color = tex1;
 
    Color = lerp(Color, tex2, tex2.a);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Rotation(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    float2 Rotation;
    Rotation.x = uv.x * cos(g_fMapAngle) - uv.y * sin(g_fMapAngle);
    Rotation.y = uv.x * sin(g_fMapAngle) + uv.y * cos(g_fMapAngle);
    Rotation += center;
            
    float4 tex1 = g_Texture.Sample(BorderZeroLinearSampler, Rotation);
    
    if (tex1.a <= 0.f)
        discard;
    
    color = tex1;
    
    color.a *= Alpha;

    Out.vColor = color;
    return Out;
   
}

PS_OUT PS_Slot(PS_IN In)
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

PS_OUT PS_SpellAnim(PS_IN In)
{
    PS_OUT Out;
    
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    uv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;
            
    float2 startUV = g_fImageUV.xy;
    float2 endUV = g_fImageUV.zw;

    float2 atlasUV = startUV + In.vTexcoord * (endUV - startUV);
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Rotation);
    float4 tex2 = g_Texture2.Sample(DefaultSampler, atlasUV);
        
    tex1.rgb *= 0.4f;
    color = tex1;
    tex2.rgb *= 0.3f;
        
    if (tex2.a >= 0.9f)
        color = tex2;
    float3 BGColor = float3(1.f, 1.f, 1.f);
    
    switch (g_iSpellType)
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
        case 6:
            BGColor = float3(0.f, 0.f, 0.f);
            break;
    }
    float4 color1 = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 OverrayUV = In.vTexcoord;
    float CoolTime = 1.f - g_fDeltaV;
    float CoolTime2 = 1.f - g_fDeltaV + 0.1f;
    atlasUV = startUV + In.vTexcoord * (endUV - startUV);
    float4 tex3 = g_Texture1.Sample(ClampSampler, OverrayUV);
    tex3.rgb *= BGColor;
    float4 tex4 = g_Texture2.Sample(DefaultSampler, atlasUV);
    
    if (tex4.a >= 0.9f)
        tex3 = tex4;
    
    color1 = tex3;
        
    float wave1 = sin(OverrayUV.x * 10.f + g_fTime) * 0.01f;
    float wave2 = sin(OverrayUV.x * 10.f + g_fTime) * 0.01f;
    float waveThreshold1 = CoolTime + wave1;
    float waveThreshold2 = CoolTime2 + wave2;
    float2 waveUV = float2(waveUV.x = OverrayUV.x * 2.f + g_fTime * 0.3f, waveUV.y = (OverrayUV.y - waveThreshold1) / (waveThreshold2 - waveThreshold1));
    waveUV = saturate(waveUV);
    float tex5 = g_Texture3.Sample(DefaultSampler, waveUV).r;

    if (g_fDeltaV <= 1.f)
    {
        if (OverrayUV.y >= waveThreshold1 && OverrayUV.y <= waveThreshold2)
        {
            color1.rgb += float3(1.f, 1.f, 1.f) * tex5 * g_fDeltaU;
        }
    }
    
    float waveThreshold = CoolTime + wave1;

    if (OverrayUV.y <= waveThreshold)
    {
        color1.a = 0.f;
    }
        
    color = lerp(color, color1, color1.a);
    
    color.a *= Alpha;
    
    Out.vColor = color;
    return Out;
}

PS_OUT PS_HpBar(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 Color1 = float4(1.f, 1.f, 1.f, 1.f);
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
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Finaluv);
    tex1.rgb = float3(255.f, 0.f, 0.f) / 255.f;
    if (g_fHp <= 0.3f)
    {
        tex1.a *= (sin(g_fBlinkTime) * 0.25f + 0.75f) * 1.5f;
    }
    else
    {
        tex1.a = 0.f;
    }
    Color = tex1;
    
    float2 BGPos = g_Pos;
    float2 BGSize = g_fImageSize;
    float2 BGPixel = Finaluv * g_fOrigin_Size;
    float2 localUV = (BGPixel - BGPos) / BGSize;
    bool inside = all(localUV >= 0.0f && localUV <= 1.0f);
    
    float4 tex2 = g_Texture1.Sample(ClampSampler, localUV);
    
    if (inside)
        Color = lerp(Color, tex2, tex2.a);
    
    float2 reversuv = In.vTexcoord;
    
    reversuv.x = 1.0f - localUV.x;

    float4 tex3 = g_Texture2.Sample(ClampSampler, localUV);
    
    if (reversuv.x >= g_fHp)
    {
        tex3.rgb = float3(0.f, 0.f, 0.f);
    }
    
    float3 Green = (float3(112.f, 241.f, 31.f) / 255.f) * 1.2f;
    float3 Red = (float3(255.f, 0.f, 0.f) / 255.f) * 1.2f;
    
    tex3.rgb *= lerp(Green, Red, g_fTime);
    
    if (all(Color.rgb >= float3(55.f / 255.f, 0.f / 255.f, 55.f / 255.f)))
        Color = lerp(Color, tex3, tex3.a);
        
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Magic_Meter(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float3 Blue = float3(41.f, 165.f, 255.f) / 255.f;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex1 = g_Texture.Sample(ClampSampler, In.vTexcoord);
    float4 tex2 = g_Texture1.Sample(ClampSampler, In.vTexcoord);
    
    float2 reverseuv = 1.f - In.vTexcoord;
    
    color = tex1;
    tex2.rgb *= Blue * 1.5f;
    
    if (reverseuv.x >= g_fHp)
    {
        tex2.rgb = float3(0.f, 0.f, 0.f);
    }
    
    if (all(color.rgb >= float3(65.f / 255.f, 65.f / 255.f, 65.f / 255.f)))
        color = tex2;
    
    color.a *= Alpha;

    Out.vColor = color;
    
    return Out;
}


PS_OUT PS_Magic_Icon(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 BGuv = In.vTexcoord - center;
    BGuv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = BGuv.x * cos(g_fAngle) - BGuv.y * sin(g_fAngle);
    Rotation.y = BGuv.x * sin(g_fAngle) + BGuv.y * cos(g_fAngle);
    Rotation += center;
            
    float4 tex1 = g_Texture.Sample(ClampSampler, Rotation);
    float4 tex2 = g_Texture1.Sample(ClampSampler, Rotation);
    
    Color = tex1;
    Color = lerp(tex1, tex2, tex2.a);
    
    float2 uv = In.vTexcoord;
    float scale = 1.2;

    uv = (uv - center) * scale + center;
    float4 tex3 = g_Texture2.Sample(ClampSampler, uv);

    
    Color = Color * (1 - tex3.a) + tex3;
    
    Color.a *= Alpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_RemapUV(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    
    float2 startUV = g_fImageUV.xy;
    float2 endUV = g_fImageUV.zw;

    float2 atlasUV = startUV + In.vTexcoord * (endUV - startUV);
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, atlasUV);
    
    Color = tex1;
    
    Color.a *= Alpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Revelio(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float2 UV = In.vTexcoord;
    
    //float2 Position[4];
    //float2 startUV[4];
    //float2 endUV[4];

    //float2 Atlases[4];
    
    //for (int i = 0; i < 4; ++i)
    //{
    //    Position[i] = g_fPosition[i];
    //    startUV[i] = g_fImagesUV[i].xy;
    //    endUV[i] = g_fImagesUV[i].zw;
        
    //    Atlases[i] = startUV[i] + UV * (endUV[i] - startUV[i]);
    //}
    
    //float4 tex1 = g_Texture.Sample(DefaultSampler, UV);
    //float4 tex2 = g_Texture1.Sample(DefaultSampler, Atlases[0]);
    //float4 tex3 = g_Texture1.Sample(DefaultSampler, Atlases[1]);
    //float4 tex4 = g_Texture2.Sample(DefaultSampler, Atlases[2]);
    //float4 tex5 = g_Texture2.Sample(DefaultSampler, Atlases[3]);
    
    
    
    ////float2 atlasUV = startUV + In.vTexcoord * (endUV - startUV);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Potion(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(214.f / 255.f, 225 / 255.f, 36 / 255.f, 1.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    uv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;

    float4 tex1 = g_Texture.Sample(ClampSampler, Rotation);
    Color = tex1;
    
    float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    //Color += tex2;
    float4 tex3 = g_Texture2.Sample(ClampSampler, Rotation);
    Color = lerp(Color, tex3, tex3.a);
    
    float2 Potionuv = In.vTexcoord;
    float scale = 1.2;

    Potionuv = (Potionuv - center) * scale + center;
    float4 tex4 = g_Texture3.Sample(ClampSampler, Potionuv);
    Color = lerp(Color, tex4, tex4.a);
    
    float2 tex5pos = g_Pos / g_fCurrent_Size;
    float2 tex5size = g_fOrigin_Size / g_fCurrent_Size;
    float2 tex5local = (In.vTexcoord - tex5pos) / tex5size;
    bool inside = all(tex5local >= 0.0f && tex5local <= 1.0f);
    float4 tex5 = g_Texture4.Sample(ClampSampler, tex5local);
    if (inside)
        Color = lerp(Color, tex5, tex5.a);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Magic_Item(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    uv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;
            
    float2 startUV = g_fImageUV.xy;
    float2 endUV = g_fImageUV.zw;

    float2 atlasUV = startUV + In.vTexcoord * (endUV - startUV);
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Rotation);
    float4 tex2 = g_Texture3.Sample(ClampSampler, atlasUV);
        
    tex1.rgb *= 0.4f;
    color = tex1;
    tex2.rgb *= 0.3f;
        
    if (tex2.a >= 0.9f)
        color = tex2;
    float3 BGColor = float3(1.f, 1.f, 1.f);
    
    switch (g_iSpellType)
    {
        case 0:
            BGColor = float3(50.f, 50.f, 50.f) / 255.f;
            break;
        case 1:
            BGColor = float3(89.f, 32.f, 215.f) / 255.f;
            break;
        case 2:
            BGColor = float3(190.f, 46., 34.f) / 255.f;
            break;
        case 3:
            BGColor = float3(37.f, 129.f, 162.f) / 255.f;
            break;
        case 4:
            BGColor = float3(134.f, 171.f, 78.f) / 255.f;
            break;
    }
    
    float4 color1 = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 OverrayUV = In.vTexcoord;
    float CoolTime = 0.f + g_fDeltaV;
    float CoolTime2 = 0.f + g_fDeltaV + 0.1f;
    atlasUV = startUV + In.vTexcoord * (endUV - startUV);
    float4 tex3 = g_Texture1.Sample(ClampSampler, OverrayUV);
    tex3.rgb *= BGColor;
    float4 tex4 = g_Texture3.Sample(DefaultSampler, atlasUV);
    
    if (g_fDeltaV <= 1.f)
    {
        if (tex4.a >= 0.9f)
            tex3 = tex4;
    
        color1 = tex3;
        
        float wave1 = sin(OverrayUV.x * 10.f + g_fTime * 2.f) * 0.01f;
        float wave2 = sin(OverrayUV.x * 10.f + g_fTime * 2.f) * 0.01f;
   
        float waveThreshold1 = CoolTime + wave1;
        float waveThreshold2 = CoolTime2 + wave2;
        float2 waveUV = float2(waveUV.x = OverrayUV.x * 2.f + g_fTime * 0.3f, waveUV.y = (OverrayUV.y - waveThreshold1) / (waveThreshold2 - waveThreshold1));
        waveUV = saturate(waveUV);
        float Mask = g_MaskingTexture.Sample(DefaultSampler, waveUV).r;
        if (g_fDeltaV <= 1.f)
        {
            if (OverrayUV.y >= waveThreshold1 && OverrayUV.y <= waveThreshold2)
            {
                color1.rgb += float3(1.f, 1.f, 1.f) * Mask * 1.f;
            }
        }
    
        float waveThreshold = CoolTime + wave1;

        if (OverrayUV.y <= waveThreshold)
        {
            color1.a = 0.f;
        }
    
        color = lerp(color, color1, color1.a);
    }
    else if (g_fDeltaV >= 1.f)
        color = lerp(color, tex4, tex4.a);
    
    float4 tex5 = g_Texture2.Sample(ClampSampler, Rotation);
    color = lerp(color, tex5, tex5.a);
    
    float2 texpos1 = g_fItemPosition1 / g_fCurrent_Size;
    float2 texpos2 = g_fItemPosition2 / g_fCurrent_Size;
    float2 texsize1 = g_fItemImageSizes1 / g_fCurrent_Size;
    float2 texsize2 = g_fItemImageSizes2 / g_fCurrent_Size;
    float2 texlocal1 = (In.vTexcoord - texpos1) / texsize1;
    float2 texlocal2 = (In.vTexcoord - texpos2) / texsize2;

    
    bool inside1 = all(texlocal1 >= 0.0f && texlocal1 <= 1.0f);
    float4 tex6 = g_Texture4.Sample(DefaultSampler, texlocal1);
    
    if (inside1)
        color = lerp(color, tex6, tex6.a);
    
    bool inside2 = all(texlocal2 >= 0.0f && texlocal2 <= 1.0f);
    
    float4 tex7 = g_Texture5.Sample(DefaultSampler, texlocal2);
    if (inside2)
        color = lerp(color, tex7, tex7.a);
    
    color.a *= Alpha;
    
    Out.vColor = color;
    return Out;
}

PS_OUT PS_Spell_Preview(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
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
    float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    float4 tex3 = g_Texture2.Sample(DefaultSampler, Finaluv);
    Color = tex1;
 
    tex3.rgb = tex2.rgb;
    
    Color = lerp(Color, tex3, tex3.a);
    
    float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
    float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
    float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
    bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
    float4 tex4 = g_Texture3.Sample(DefaultSampler, Finaluv);
    if (inside1)
    {
        Color = lerp(Color, tex4, tex4.a);
    }
    
    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Spell_Header(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float3 BGColor = float3(1.f, 1.f, 1.f);
    
    switch (g_iSpellType)
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
        case 6:
            BGColor = float3(0.f, 0.f, 0.f);
            break;
    }
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Color.rgb = tex1.rgb * 0.8f;
    Color.a = tex1.a;
    
    Color.rgb *= BGColor;
    
    Color.a *= Alpha * 0.5f;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Spell_Header_Line(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
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
    float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    Color = tex1;
 
    //Color.rgb = tex2.rgb;
    
    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Spell_Drag(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;

    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord).r;
    float4 tex2Color = g_Texture2.Sample(DefaultSampler, In.vTexcoord);
    
    tex2Color.a = tex2;
    Color = lerp(tex1, tex2Color, tex2Color.a);
    
    float2 atlasUV = g_fImageUV.xy + In.vTexcoord * (g_fImageUV.zw - g_fImageUV.xy);
    //float2 center = (g_fImageUV.xy + g_fImageUV.zw) * 0.5f;
    //float2 uv = atlasUV - center;
    //uv *= 0.7f;
    //uv += center;
    float4 tex3 = g_Texture3.Sample(ClampSampler, atlasUV);

    Color = lerp(Color, tex3, tex3.a);
 
    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Camera_LockOn(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;

    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 center = float2(0.5, 0.5);
    float2 uv = In.vTexcoord - center;
    float angle = -g_fTime;
    float s = sin(angle);
    float c = cos(angle);
    float2 maskuv = float2(uv.x * c - uv.y * s, uv.x * s + uv.y * c) + center;
    
    float tex1 = g_Texture.Sample(ClampSampler, maskuv).r;
    
    Color.a = tex1 * 0.3f;
    
    if (g_iHover == 0)
    {
        float4 tex2 = g_Texture1.Sample(ClampSampler, maskuv);

        Color.rgb += (tex2.rgb * tex2.a) * 2.f;
    }
    
    float2 UpUV = In.vTexcoord;
    UpUV.y += g_fTime;
    
    float4 tex3 = g_Texture2.Sample(DefaultSampler, UpUV);
    float tex4 = g_Texture1.Sample(ClampSampler, In.vTexcoord).r;
    
    tex3.a *= tex4;
    
    Color = lerp(Color, tex3, tex3.a);
    
    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Loding_Screen(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;

    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float4 tex2 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    Color = tex1;
    Color = lerp(Color, tex2, tex2.a);
    
    //float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
    //float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
    //float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
    //bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
    //if (inside1)
    //{
    //    float2 uv = saturate(Imagelocal);
    //    float4 tex2 = g_Texture1.Sample(DefaultSampler, uv);
        
    //    Color = lerp(Color, tex2, tex2.a);
    //}
    
    Color.a *= Alpha;
    
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Enemy_HpBer(PS_IN In)
{
    PS_OUT Out;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 Color1 = float4(1.f, 1.f, 1.f, 1.f);
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
    float4 tex2 = g_Texture1.Sample(DefaultSampler, Finaluv);
    
    Color = tex1;
    
    tex2.rgb *= float3(1.f, 0.f, 0.f);
    
    if (In.vTexcoord.x >= g_fHp)
    {
        tex2.rgb = float3(0.f, 0.f, 0.f);
    }
    
    Color = lerp(Color, tex2, tex2.a);
    
    if (Color.a <= 0.f)
        discard;

    if (g_iHover != 0)
    {
        float4 tex3 = g_Texture2.Sample(DefaultSampler, In.vTexcoord);
        
        tex3.rgb *= float3(1.f, 0.f, 0.f);
        
        if (g_fTime >= tex3.r)
        {
            Color.a *= tex3.r;
            Color.rgb = float3(1.f, 0.f, 0.f);
        }
    }
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Boss_HpBer(PS_IN In)
{
    PS_OUT Out;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 Color1 = float4(1.f, 1.f, 1.f, 1.f);
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
    float4 tex2 = g_Texture1.Sample(DefaultSampler, Finaluv);
    
    Color = tex1;
    
    tex2.rgb *= float3(1.f, 0.f, 0.f);
    
    if (In.vTexcoord.x >= g_fHp)
    {
        tex2.rgb = float3(0.f, 0.f, 0.f);
    }
    
    if (all(Color.rgb >= (float3(55.f, 55.f, 55.f) / 255.f)))
        Color = tex2;
    
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Quest_Border(PS_IN In)
{
    PS_OUT Out;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 Color1 = float4(1.f, 1.f, 1.f, 1.f);
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
    Color = tex1;
    
    if (g_iColor == 1)
        Color.rgb *= float3(100.f, 0.f, 100.f) / 255.f;
    
    if (g_iColor == 2)
    {
        Color.rgb *= 0.3f;
    }
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Quest_Info(PS_IN In)
{
    PS_OUT Out;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 Color1 = float4(1.f, 1.f, 1.f, 1.f);
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
    float4 tex2 = g_Texture1.Sample(DefaultSampler, Finaluv);
    
    Color = tex1;

    Color = lerp(Color, tex2, tex2.a);
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Quest_Status(PS_IN In)
{
    PS_OUT Out;

    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (g_iClick == 0)
    {
        if (g_iHover == 0)
        {
            float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
            tex1.rgb *= float3(60.f, 80.f, 150.f) / 255.f;
            Color = tex1;
            float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
            float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
            float2 Imagelocal1 = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
            bool inside1 = all(Imagelocal1 >= 0.0f && Imagelocal1 <= 1.0f);
            if (inside1)
            {
                float2 atlasUV = g_fImageUV.xy + Imagelocal1 * (g_fImageUV.zw - g_fImageUV.xy);
                float2 safeUV = saturate(atlasUV);
                float4 tex2 = g_Texture1.Sample(DefaultSampler, safeUV);
                Color = lerp(Color, tex2, tex2.a);
            }
        }
        else
        {
            float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
            Color = tex1;
            float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
            float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
            float2 Imagelocal1 = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
            bool inside1 = all(Imagelocal1 >= 0.0f && Imagelocal1 <= 1.0f);
            if (inside1)
            {
                float2 atlasUV = g_fImageUV.xy + Imagelocal1 * (g_fImageUV.zw - g_fImageUV.xy);
                float2 safeUV = saturate(atlasUV);
                float4 tex2 = g_Texture1.Sample(DefaultSampler, safeUV);
                Color = lerp(Color, tex2, tex2.a);
            }
        }
    }
    else
    {
        float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
        Color = tex1;
        float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
        float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
        float2 Imagelocal1 = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
        bool inside1 = all(Imagelocal1 >= 0.0f && Imagelocal1 <= 1.0f);
        if (inside1)
        {
            float2 atlasUV = g_fImageUV.xy + Imagelocal1 * (g_fImageUV.zw - g_fImageUV.xy);
            float2 safeUV = saturate(atlasUV);
            float4 tex2 = g_Texture1.Sample(DefaultSampler, safeUV);
            Color = lerp(Color, tex2, tex2.a);
        }
        float mask = g_Texture2.Sample(DefaultSampler, In.vTexcoord).r;
        float3 glowColor = float3(1.0f, 1.0f, 1.0f);
        Color.rgb += glowColor * mask * 0.5f;
        Color.rgb *= float3(140.f, 140.f, 20.f) / 255.f;
    }
  
   
    
    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_Broomstick(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    uv *= sqrt(2.0f);
    float2 Rotation = In.vTexcoord;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Rotation);
    float4 tex2 = g_Texture1.Sample(ClampSampler, Rotation);
    tex1.rgb *= 0.4f;
    color = tex1;
    tex2.rgb *= 0.3f;
    
    color = lerp(color, tex2, tex2.a);

    float2 texpos1 = g_fImageSipos1.xy / g_fCurrent_Size * 0.5f;
    float2 texsize1 = g_fImageSipos1.zw / g_fCurrent_Size * 0.5;
    float2 texlocal1 = (In.vTexcoord - texpos1) / texsize1;
    bool inside1 = all(texlocal1 >= 0.0f && texlocal1 <= 1.0f);
    if (inside1)
    {
        float4 tex3 = g_Texture2.Sample(ClampSampler, texlocal1);
        color = lerp(color, tex3, tex3.a);
    }
    
    float2 texpos2 = g_fImageSipos2.xy / g_fCurrent_Size;
    float2 texsize2 = g_fImageSipos2.zw / g_fCurrent_Size;
    float2 texlocal2 = (In.vTexcoord - texpos2) / texsize2;
    bool inside2 = all(texlocal2 >= 0.0f && texlocal2 <= 1.0f);
    if (inside2)
    {
        float2 startUV = g_fImageUV.xy;
        float2 endUV = g_fImageUV.zw;
        float2 atlasUV = atlasUV = startUV + texlocal2 * (endUV - startUV);
        float4 tex4 = g_Texture3.Sample(DefaultSampler, atlasUV);
        color = lerp(color, tex4, tex4.a);
    }

    color.a *= Alpha;
    
    Out.vColor = color;
    return Out;
}


PS_OUT PS_SpellLearn(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);

    float4 tex = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float tex1 = g_Texture1.Sample(DefaultSampler, In.vTexcoord).r;
    Color = tex;
    
    tex1 *= 0.95f;
           
    Color.a = tex1;

    Color.a *= Alpha;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_SpellLearnColor(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);

    float4 tex = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float4 tex1 = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    Color = tex;
    
    Color.rgb *= tex1.rgb;

    float2 rectPos = g_fPosition - float2(256.f, 259.f);
    float2 rectSize = g_fCurrent_Size;

    float2 itemPos = g_fItemPosition1;
    float2 itemSize = g_fItemImageSizes1;

    float2 pixelPos;
    pixelPos.x = rectPos.x + In.vTexcoord.x * rectSize.x;
    pixelPos.y = rectPos.y + (1.0f - In.vTexcoord.y) * rectSize.y;

    if (pixelPos.x >= itemPos.x &&
    pixelPos.x <= itemPos.x + itemSize.x &&
    pixelPos.y >= itemPos.y &&
    pixelPos.y <= itemPos.y + itemSize.y)
    {
        Color.rgb = float3(0.f, 0.f, 1.f);
    }

    for (int i = 0; i < g_Count; ++i)
    {
        float2 trailTopLeft = g_Trail[i];
        float2 trailCenter = trailTopLeft + itemSize / 2.0f;

        float halfWidth = itemSize.x / 2.0f + 12.f;
        float halfHeight = itemSize.y / 2.0f + 12.f;

        if (pixelPos.x >= trailCenter.x - halfWidth &&
        pixelPos.x <= trailCenter.x + halfWidth &&
        pixelPos.y >= trailCenter.y - halfHeight &&
        pixelPos.y <= trailCenter.y + halfHeight)
        {
            Color.rgb = float3(20.f, 100.f, 180.f) / 255.f;
            break;
        }
    }
    
    for (int j = 0; j < g_ChaseCount; ++j)
    {
        float2 ChaseTopLeft = g_ChaseTrail[j];
        float2 ChaseCenter = ChaseTopLeft + itemSize / 2.0f;

        float halfWidth = itemSize.x * 0.7f;
        float halfHeight = itemSize.y * 0.7f;

        if (pixelPos.x >= ChaseCenter.x - halfWidth &&
        pixelPos.x <= ChaseCenter.x + halfWidth &&
        pixelPos.y >= ChaseCenter.y - halfHeight &&
        pixelPos.y <= ChaseCenter.y + halfHeight)
        {
            Color.rgb = float3(110.f, 5.f, 5.f) / 255.f;
            break;
        }
    }

    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_SpellLearnBooster(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    float4 tex = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Color = tex;
    
    float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
    float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
    float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
    bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
    
    if (g_iBoosterOn == 0 && g_iBoosterOff == 0)
    {
        if (inside1)
        {
            float4 tex1 = g_Texture1.Sample(DefaultSampler, Imagelocal);
            Color = lerp(Color, tex1, tex1.a);
        }
    }
    else if (g_iBoosterOn != 0 && g_iBoosterOff == 0)
    {

        float4 tex3 = g_Texture3.Sample(DefaultSampler, In.vTexcoord);
        Color = lerp(Color, tex3, tex3.a);
    }
    else if (g_iBoosterOn == 0 && g_iBoosterOff != 0)
    {
        float4 tex2 = g_Texture2.Sample(DefaultSampler, In.vTexcoord);
    
        tex2.rgb *= float3(1.f, 0.f, 0.f);
        Color = tex;
        
        Color = lerp(Color, tex2, tex2.a);
    }

    Color.a *= Alpha;
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_NPCInteraction(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
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
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Finaluv);

    Color = tex1;
    
    float2 texpos1 = g_fImageSipos1.xy / g_fCurrent_Size * 0.5f;
    float2 texsize1 = g_fImageSipos1.zw / g_fCurrent_Size * 0.5;
    float2 texlocal1 = (In.vTexcoord - texpos1) / texsize1;
    bool inside1 = all(texlocal1 >= 0.0f && texlocal1 <= 1.0f);
    if (inside1)
    {
        float2 startUV = g_fImageUV.xy;
        float2 endUV = g_fImageUV.zw;
        float2 atlasUV = atlasUV = startUV + texlocal1 * (endUV - startUV);
        float4 tex2 = g_Texture1.Sample(DefaultSampler, atlasUV);
        Color = lerp(Color, tex2, tex2.a);

    }

    Color.a *= Alpha;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_CanvasFade(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha;
    float4 Color = float4(0.f, 0.f, 0.f, 1.f);
  
    float4 tex = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    Color = tex;
    
    Color.a = Alpha;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_FlagWave(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha;
    float4 Color = float4(0.f, 0.f, 0.f, 1.f);
    
    float2 uv = In.vTexcoord;

    float phase = uv.y * 1.5f;

    float waveX =
    sin(uv.y * 20.f + g_fTime * 1.0f + phase) * 0.007f;

    float waveY =
    sin(uv.x * 12.f + g_fTime * 0.8f) * 0.002f;

    uv.x += waveX;
    uv.y += waveY;
    
    float4 tex = g_Texture.Sample(ClampSampler, uv);

    Color = tex;
    
    if (Color.r <= 0.f)
        discard;
    
    Color.a = Alpha;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_BroomCircle(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha * g_fOwnerAlpha * g_fCanvasAlpha;
   
    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = In.vTexcoord - center;
    float2 Rotation;
    Rotation.x = uv.x * cos(g_fAngle) - uv.y * sin(g_fAngle);
    Rotation.y = uv.x * sin(g_fAngle) + uv.y * cos(g_fAngle);
    Rotation += center;
            
    float4 tex = g_Texture.Sample(BorderZeroLinearSampler, Rotation);
    color = tex;
    
    float Diffuse = g_Texture1.Sample(DefaultSampler, In.vTexcoord).r;
    
    
    if (g_iHover != 0)
    {
        if (g_fTime >= Diffuse)
        {
            discard;
        }
    }
    
    color.a *= Alpha;
    
    Out.vColor = color;
    return Out;
}

struct VS_IN3D
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT3D
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT3D VS_MAIN3D(VS_IN3D In)
{
    VS_OUT3D Out;
    
    matrix matViewProj = mul(g_ViewMatrix, g_ProjMatrix);
    
    float2 Offset = In.vTexcoord - 0.5f;
    float3 worldCenter = float3(g_WorldMatrix._41, g_WorldMatrix._42, g_WorldMatrix._43);

    
    float3 vLook = normalize(g_CamaraPosition - worldCenter);
    
    vLook.y = 0.f;
    vLook = normalize(vLook);
    
    float3 upDir = float3(0.f, 1.f, 0.f);

    float3 rightDir = normalize(cross(vLook, upDir));
        
    float3 dirRight = rightDir * g_fCurrent_Size.x * Offset.x;
    float3 dirUp = upDir * g_fCurrent_Size.y * Offset.y;

    float3 vertexPos = worldCenter + dirRight + dirUp;

    float2 TexCoord = In.vTexcoord;
    TexCoord.y = 1.f - TexCoord.y;
    
    Out.vPosition = mul(float4(vertexPos, 1.f), matViewProj);
    Out.vTexcoord = TexCoord;
    return Out;
}

PS_OUT PS_Enemy_Detection(PS_IN In)
{
    PS_OUT Out;

    float4 Color = float4(1.f, 1.f, 1.f, 0.f * g_fAlpha);
    
    if (g_iStep1 != 0)
    {
        float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
        float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
        float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
        bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
        if (inside1)
        {
            float4 tex3 = g_Texture1.Sample(ClampSampler, Imagelocal);
        
            float2 Imagetexpos2 = g_fImageSipos1.xy / g_fCurrent_Size;
            float2 Imagetexsize2 = g_fImageSipos1.zw / g_fCurrent_Size;
            float2 Imagelocal2 = (In.vTexcoord - Imagetexpos2) / Imagetexsize2;
            
            Color = lerp(Color, tex3, tex3.a);
            
            float4 tex4 = g_Texture2.Sample(ClampSampler, Imagelocal2);
            float4 tex5 = g_Texture3.Sample(ClampSampler, Imagelocal2);
            
            float4 Color1 = tex4;
            
            Color1 = lerp(tex4, tex5, tex5.a);
            
            Imagelocal2.y = 1.f - Imagelocal2.y;
            
            if (Imagelocal2.y >= g_fTime)
            {
                Color1.a = 0.f;
            }
            
            Color = lerp(Color, Color1, Color1.a);
            
            Color.a *= g_fAlpha;
            Out.vColor = Color;
    
            return Out;
        }
    }
    if (g_iStep2 != 0)
    {
        float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
        float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
        float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
        bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
        if (inside1)
        {
            float4 tex3 = g_Texture1.Sample(ClampSampler, Imagelocal);
        
            float2 Imagetexpos2 = g_fImageSipos1.xy / g_fCurrent_Size;
            float2 Imagetexsize2 = g_fImageSipos1.zw / g_fCurrent_Size;
            float2 Imagelocal2 = (In.vTexcoord - Imagetexpos2) / Imagetexsize2;
            
            float4 tex4 = g_Texture2.Sample(ClampSampler, Imagelocal2);
            float4 tex6 = g_Texture4.Sample(ClampSampler, Imagelocal2);
            Color = lerp(Color, tex3, tex3.a);
            float4 Color1 = tex4;
            Color1 = lerp(tex4, tex6, tex6);
            Color1.rgb *= float3(1.f, 1.f, 0.f);
            Imagelocal2.y = 1.f - Imagelocal2.y;
            if (Imagelocal2.y >= g_fTime)
            {
                Color1.a = 0.f;
            }
            Color = lerp(Color, Color1, Color1.a);
            
            Color.a *= g_fAlpha;
            
            Out.vColor = Color;
    
            return Out;
        }
    }
    if (g_iStep3 != 0)
    {
        float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
        Color = tex1;
        Color.rgb *= float3(1.f, 0.f, 0.f);
        
        float2 Imagetexpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
        float2 Imagetexsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
        float2 Imagelocal = (In.vTexcoord - Imagetexpos1) / Imagetexsize1;
        bool inside1 = all(Imagelocal >= 0.0f && Imagelocal <= 1.0f);
        if (inside1)
        {
            float4 tex7 = g_Texture5.Sample(ClampSampler, Imagelocal);

            Color = lerp(Color, tex7, tex7.a);
            
            Color.a *= g_fAlpha;
            Out.vColor = Color;
    
            return Out;
        }
       
    }

    Color.a *= g_fAlpha;
    Out.vColor = Color;
    
    return Out;
}

VS_OUT3D VS_NONESIZE3D(VS_IN3D In)
{
    VS_OUT3D Out;

    float3 worldCenter = float3(
        g_WorldMatrix._41,
        g_WorldMatrix._42,
        g_WorldMatrix._43
    );

    float4 clipPos = mul(
        float4(worldCenter, 1.f),
        mul(g_ViewMatrix, g_ProjMatrix)
    );

    float w = clipPos.w;

    clipPos.xy /= w;

    float2 offset = In.vTexcoord - 0.5f;
    float2 pixelOffset = offset * g_fCurrent_Size.xy;

    float2 ndcOffset;
    ndcOffset.x = pixelOffset.x / g_fViewport.x * 2.f;
    ndcOffset.y = pixelOffset.y / g_fViewport.y * 2.f;

    clipPos.xy += ndcOffset;

    clipPos.xy *= w;
    clipPos.w = w;

    Out.vPosition = clipPos;

    float2 TexCoord = In.vTexcoord;
    TexCoord.y = 1.f - TexCoord.y;
    Out.vTexcoord = TexCoord;

    return Out;
}

PS_OUT PS_Interaction_Object(PS_IN In)
{
    PS_OUT Out;

    float4 Color = float4(1.f, 1.f, 1.f, 0.f * g_fAlpha);
    
    float2 atlasUV = g_fImageUV.xy + In.vTexcoord * (g_fImageUV.zw - g_fImageUV.xy);
    float2 safeUV = saturate(atlasUV);
    float4 tex1 = g_Texture.Sample(DefaultSampler, safeUV);
    
    Color = tex1;

    Color.a *= g_fAlpha;
    Out.vColor = Color;
    
    return Out;
}

PS_OUT PS_Interaction_Npc(PS_IN In)
{
    PS_OUT Out;
    float Alpha = g_fAlpha;
    float4 Color = float4(1.f, 1.f, 1.f, 1.f);
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
    
    float4 tex1 = g_Texture.Sample(ClampSampler, Finaluv);

    Color = tex1;

    float2 texpos1 = g_fImageSipos1.xy / g_fCurrent_Size;
    float2 texsize1 = g_fImageSipos1.zw / g_fCurrent_Size;
    float2 texlocal1 = (In.vTexcoord - texpos1) / texsize1;
    bool inside1 = all(texlocal1 >= 0.0f && texlocal1 <= 1.0f);
    if (inside1)
    {
        float2 startUV = g_fImageUV.xy;
        float2 endUV = g_fImageUV.zw;
        float2 atlasUV = atlasUV = startUV + texlocal1 * (endUV - startUV);
        float4 tex2 = g_Texture1.Sample(DefaultSampler, atlasUV);
        Color = lerp(Color, tex2, tex2.a);

    }
    

    Color.a *= Alpha;
    
    Out.vColor = Color;
    return Out;
}

PS_OUT PS_BroomGateTarget(PS_IN In)
{
    PS_OUT Out;

    float4 Color = float4(1.f, 1.f, 1.f, 0.f * g_fAlpha);
    
    float4 tex1 = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    Color = tex1;

    Color.a *= g_fAlpha;
    Out.vColor = Color;
    
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

    pass Texture_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Texture_Color();
    }

    pass Logo
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_UIBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Logo();
    }

    pass Logo_Text
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_UIBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Logo_Text();
    }

    pass Logo_Glow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Logo_Glow();
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

    pass Mission
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Mission();
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

    pass Spell_Anim
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Anim();
    }

    pass NineSlice
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NineSlice();
    }

    pass Rotation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Rotation();
    }

    pass Slot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Slot();
    }

    pass SpellAnim
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SpellAnim();
    }
    pass HpBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HpBar();
    }
    pass Magic_Meter
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Magic_Meter();
    }

    pass Magic_Icon
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Magic_Icon();
    }

    pass RemapUV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RemapUV();
    }

    pass Revelio
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Revelio();
    }

    pass Potion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Potion();
    }

    pass Magic_Item
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Magic_Item();
    }

    pass Spell_Preview
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Preview();
    }

    pass Spell_Header
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Header();
    }

    pass Spell_Header_Line
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Header_Line();
    }

    pass Spell_Drag
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Spell_Drag();
    }

    pass Camera_LockOn
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Camera_LockOn();
    }

    pass Loding_Screen
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Loding_Screen();
    }

    pass Enemy_HpBer
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Enemy_HpBer();
    }

    pass Boss_HpBer
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Boss_HpBer();
    }

    pass Quest_Border
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Quest_Border();
    }

    pass Quest_Info
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Quest_Info();
    }

    pass Quest_Status
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Quest_Status();
    }

    pass Broomstick
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Broomstick();
    }

    pass SpellLearn
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SpellLearn();
    }

    pass SpellLearnColor
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SpellLearnColor();
    }

    pass SpellLearnBooster
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SpellLearnBooster();
    }

    pass NPCInteraction
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NPCInteraction();
    }

    pass CanvasFade
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CanvasFade();
    }

    pass FlagWave
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FlagWave();
    }

    pass BroomCircle
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BroomCircle();
    }

    pass Enemy_Detection
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN3D();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Enemy_Detection();
    }

    pass Interaction_Object
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_NONESIZE3D();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Interaction_Object();
    }

    pass Interaction_Npc
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_NONESIZE3D();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Interaction_Npc();
    }

    pass BroomGateTarget
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_NONESIZE3D();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BroomGateTarget();
    }
}
