
#include "Engine_Shader_Defines.hlsli"
#include "Engine_Shader_Functions.hlsli"

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
Texture2D g_DistortionTexture;
Texture2D g_DepthStencilTexture;

/* 디퓨즈 */
bool    g_isDiffuse;
float   g_fDiffuseAlpha;
float4  g_vColor;

/* 마스크 */
bool g_isMask;
float g_fSoftMaskEdge;
float g_fSoftMask;

/* 노이즈 */
bool    g_isNoise;
bool    g_isNoiseColor;
bool    g_isNoiseAlpha;
float   g_fNoiseStrength;

/* 디스토션 */

bool    g_isDistortion;
float2  g_vDistortionTime;
float2  g_vDiffuseDistortionUVGainAmount;
float2  g_vMaskDistortionUVGainAmount;
float   g_fDistortionIntensity; // 디스토션 왜곡 세기

/* 이미시브 */
float4  g_vEmissive;
float   g_fEmissiveStrength;
float   g_fSoftenExp;
float   g_fSoftStrength;
float   g_fCoreBoost;
float   g_fRadius;

/*  블러  */
float   g_fBlurIntensity;
float   g_iBlurWeight;

/* 블룸 */
bool  g_isBloomDissolve;
bool  g_isBloomReverseDissolve;

float g_fBloomStrength;
int   g_iBloomType;

float2 g_vBloomTime;



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


vector Draw_Trail(PS_IN In)
{
    vector vMtrlDiffuse;
    vector vMtrlMask;
    vector vMtrlNoise;
    vector vMtrlDistortion;
    
    /* 디퓨즈 */
    
    if (g_isDiffuse == true)
    {
        float2 vDiffuseTexcoord = In.vTexcoord;
            
        /*  디퓨즈 디스토션 */
        if (g_isDistortion == true)
        {
            float2 vDistortion = In.vTexcoord + SelectLerpUV(g_vDiffuseDistortionUVGainAmount, (g_vDistortionTime.x / g_vDistortionTime.y), 0);
            
            vMtrlDistortion = g_DistortionTexture.Sample(DefaultSampler, vDistortion);

            //0.5f를 빼는 이유는 중심을 0으로 옮겨서 양음수 방향으로 offset을 줄수 있다.
            vMtrlDistortion.rg -= 0.5f;
                        
            
            //디스토션(노이즈) 텍스쳐로 uv를 왜곡함

            vDiffuseTexcoord = vDiffuseTexcoord + (vMtrlDistortion).rg * g_fDistortionIntensity;
            
        }
        
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, vDiffuseTexcoord);
        
        
        vMtrlDiffuse.a *= g_fDiffuseAlpha;
        
        if (g_vColor.a > 0)
            vMtrlDiffuse += g_vColor;
    }
    else
        vMtrlDiffuse = g_vColor;
    
    /* 노이즈 */
    
    if (g_isNoise == true)
    {
        vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord);
       
        if (g_isNoiseAlpha)
            vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlNoise.r) * g_fNoiseStrength;
        
        if (g_isNoiseColor)
            vMtrlDiffuse.rgb *= vMtrlNoise.rgb * g_fNoiseStrength;
        
    }
    

    /* 마스크 */
    
    if (g_isMask == true)
    {
        float2 vMaskTexcoord = In.vTexcoord;
        
        // 마스크 디스토션 
        if (g_isDistortion == true)
        {
            float2 vDistortionUV = In.vTexcoord + SelectLerpUV(g_vMaskDistortionUVGainAmount, (g_vDistortionTime.x / g_vDistortionTime.y), 0);
            
            vMtrlDistortion = g_DistortionTexture.Sample(DefaultSampler, vDistortionUV);
            
            //디스토션(노이즈) 텍스쳐로 uv를 왜곡함
            vMaskTexcoord = vMaskTexcoord + (vMtrlDistortion - 0.5f).r * g_fDistortionIntensity;

            vMaskTexcoord = saturate(vMaskTexcoord);
        }
        
        vMtrlMask = g_MaskingTexture.Sample(DefaultSampler, vMaskTexcoord);
        
        float fSoftMask;
    
        if (g_fSoftMask > FLT_EPSILON5)
            fSoftMask = saturate((vMtrlMask.r - g_fSoftMaskEdge) * g_fSoftMask);
        else
            fSoftMask = vMtrlMask.r;
    
        vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * fSoftMask);
    }
    else
        vMtrlMask.r = 1.f;
    


    
    /* 디퓨즈 알파 컷*/
    if (vMtrlDiffuse.a <= FLT_EPSILON5)
        discard;

    return vMtrlDiffuse;
     
}

float4 EmissiveDraw(PS_IN In)
{
    float4 vEmissiveMtrl = g_vEmissive;
    float2 CenteredUV = In.vTexcoord - 0.5f;
    float fEmissive = 0.f;
    float fEmissiveStrength = g_fEmissiveStrength;
    
    if (g_fRadius >= FLT_EPSILON5)
    {
        float fDistance = saturate(length(CenteredUV) / g_fRadius); // 마스크의 크기를 어느정도할지 
    
        float fSoftEdge = pow(saturate(1.0f - fDistance), g_fSoftenExp) * g_fSoftStrength; //SoftenExp  엣지감쇠지수 , Strength
   
        float fCore = pow(saturate(1.0 - fDistance * 2.f), max(0.001, g_fCoreBoost)) * g_fCoreBoost; // CoreBust 중심강조
    
        fEmissive = fSoftEdge + fCore;
        
      
    }
    
    if (fEmissive == 0.f)
        fEmissive = 1.f;
 
    return saturate(vEmissiveMtrl * fEmissive * fEmissiveStrength);
}

PS_TRAILOUT PS_Trail(PS_IN In)
{
    PS_TRAILOUT Out;
    
    int2 iTexel = int2(In.vPosition.xy);
    
    float fDepthStencilValue = g_DepthStencilTexture.Load(int3(iTexel, 0)).r;
    
    float fbias = 0.000005f;
    
    if (fDepthStencilValue <= In.vProjPos.z / In.vProjPos.w + fbias)
        discard;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = Draw_Trail(In);
   
    
    /* 웨이트 블랜드 */
    
    float fWeight = clamp(pow(In.vProjPos.w, -2.5f), 1.0f, 1000.0f);
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, vMtrlDiffuse.a) * fWeight;
    Out.vRevealage.r = vMtrlDiffuse.a;

    /* 이미시브 */
    
    Out.vDiffuse.rgb += EmissiveDraw(In).rgb;

    Out.vColorTarget = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

struct PS_BLUR_IN
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_BLUR_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBlurWeight : SV_TARGET1;
};

PS_BLUR_OUT PS_TRAIL_BLUR(PS_IN In)
{
    PS_BLUR_OUT Out;
    
    vector vMtrlDiffuse;
 
    vMtrlDiffuse = Draw_Trail(In);
    
    //// 색깔 추가할 처리 (이미시브)
   
    //vMtrlDiffuse += EmissiveDraw(In);
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb * g_fBlurIntensity, vMtrlDiffuse.a);
    Out.vBlurWeight = g_iBlurWeight / 128.f;
    
    return Out;
}

struct PS_BLOOM_OUT
{
    float4 vDiffuse : SV_TARGET0;
};


PS_BLUR_OUT PS_TRAIL_BLOOM(PS_IN In)
{
    PS_BLUR_OUT Out;
    
    vector vMtrlDiffuse;
 
    vMtrlDiffuse = Draw_Trail(In);
    
    //// 색깔 추가할 처리 (이미시브)
   
    //vMtrlDiffuse += EmissiveDraw(In);
    
    float fBloomStrength = g_fBloomStrength;
    
    if (g_isBloomDissolve == true)
    {
        fBloomStrength = g_fBloomStrength * (1.f - (g_vBloomTime.x / g_vBloomTime.y));
    }
    
      
    if (g_isBloomReverseDissolve == true)
    {
        fBloomStrength = g_fBloomStrength * ((g_vBloomTime.x / g_vBloomTime.y));
    }
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb * g_fBloomStrength, (float) g_iBloomType / 255.f);
    
    return Out;
}

PS_BLOOM_OUT PS_BLEND(PS_IN In)
{
    PS_BLOOM_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = Draw_Trail(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;

    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_BLOOM_OUT PS_WEIGHTED_FOR_BLEND(PS_IN In)
{
    PS_BLOOM_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = Draw_Trail(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;
    
        
    float fWeight = clamp(pow(In.vProjPos.w, -2.5f), 1.0f, 1000.0f);
    
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, vMtrlDiffuse.a) * fWeight;
    
    
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

    pass TrailBlend
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND();
    }

    pass TrailWB_For_Blend
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_WB_Acc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHTED_FOR_BLEND();
    }

    pass TrailBlur
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL_BLUR();
    }


    pass TrailBloom
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL_BLOOM();
    }

}
