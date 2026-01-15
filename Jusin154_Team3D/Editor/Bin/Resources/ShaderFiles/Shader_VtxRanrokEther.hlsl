#include "Engine_Shader_Defines.hlsli"
#include "Engine_Shader_Functions.hlsli"

struct BoneOut
{
    row_major float4x4 Combined;
    row_major float4x4 Local;
    row_major float4x4 LocalCombined;
};

StructuredBuffer<BoneOut> g_BoneBuffer : register(t26);
StructuredBuffer<BoneOut> g_PrevBoneBuffer : register(t27);

matrix g_OffsetMatrix[256];

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_WorldMatrixInv, g_ProjMatrixInv, g_ViewMatrixInv;

matrix g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;

Texture2D g_DiffuseTexture : register(t0);

Texture2D g_NormalTexture : register(t1);
Texture2D g_MaskingTexture : register(t2);
Texture2D g_DissolveTexture : register(t3);
Texture2D g_NoiseTexture : register(t4);

Texture2D g_EmissiveTexture : register(t6);
Texture2D g_DepthStencilTexture : register(t7);
Texture2D g_DistortionTexture : register(t8);
Texture2D g_DepthTexture : register(t9);
Texture2D g_SurfaceParamsTexture : register(t10);
Texture2D g_NormalMapTexture : register(t11);


float4 g_vCamPosition;
float  g_fFar;
float  g_fUsingSurfaceParams;
float  g_fMBIntensity = 1.f;

float g_fWinSizeX;
float g_fWinSizeY;

/* 소프트 이펙트 */
bool g_isNonSoftEffect;
bool g_isScreenFX;
/* 디퓨즈 */
float4 g_vColor;
float  g_fDiffuseAlpha;

float2 g_vDiffuseUVGainAmount;
float2 g_vUVCutting;

int    g_iDiffuseMoveLerpOption;

bool   g_isDiffuseUVMove;

bool   g_isDiffuse_R;
bool   g_isDiffuse_G;
bool   g_isDiffuse_B;

/* 노이즈 */
bool   g_isNoiseColor;
bool   g_isNoiseAlpha;
bool   g_isNoiseUVMove;

bool   g_isNoise_B;
bool   g_isNoise_G;

float4 g_vNoiseColor;
float2 g_vNoiseUVGainAmount;
float2 g_vUVNoiseCutting;

int    g_iNoiseMoveLerpOption;
/* 마스크 */

bool   g_isMask_G;
bool   g_isMask_B;

bool   g_isMaskClampSample;
bool   g_isMaskUVMove;

float2 g_vMaskingUVGainAmount;
float2 g_vMaskDistortionUVGainAmount;
float2 g_vUVMaskCutting;

int    g_iMaskMoveLerpOption;
float2 g_vMaskOffset;
/* 디졸브 */


float  g_fDissolveDelay;
float  g_fReverseDissolveDelay;
float2 g_vDissolveUVGainAmount;

bool   g_isDissolve;
bool   g_isDissolveMove;
bool   g_isReverseDissolve;
bool   g_isNomalDissolve;

bool   g_isNoDissolveSmoothStep;

bool   g_isDissolve_B;
bool   g_isDissolve_G;

float  g_fDissolveMaskEdge;
float  g_fDissolveSoftMask;
float  g_fDissolveCutRatio;
float2 g_vDissolveSmoothRange;

/* 디스토션 */

int     g_iMaskDistortionMoveLerpOption;
int     g_iDiffuseDistortionMoveLerpOption;

float2  g_vDistortionTime;

/* 블러 */
float g_fBlurIntensity; //블러 세기
float g_fNoiseDistortionIntensity; // 디스토션 왜곡 세기

bool  g_isBlurNoEmissive;
bool  g_isBlurColor;
bool  g_isBlurDissolve;

float4  g_vBlurColor;

/* 이미시브 */
float4 g_vEmissive;
float g_fEmissiveStrength;
    
float g_fSoftenExp;
float g_fSoftStrength;
float g_fCoreBoost;
float g_fRadius;
float g_fEmissiveColorCut;

bool g_isEmissiveDissolve;
bool g_isEmissiveDissolveReverse;

/* 소프트 마스크*/
float g_fSoftMaskEdge;
float g_fSoftMask;

/* Flag */
bool g_isDiffuse;
bool g_isMasking;

bool g_isNoise;
bool g_isEmissive;
bool g_isDistortion;

/* 블룸 */
float g_fBloomStrength;
int   g_iBloomType;

bool  g_isBloomDissolve;
bool  g_isBloomReverseDissolve;


/* 림라이트 */
bool   g_isRimLight;

float  g_fRimLightPower;
float  g_fRimLightStrength;
float4 g_vRimLightColor;

/* 모델 디스토션 */

float g_fModelDistortIntensity;

/* 모델 블러 */

float g_fModelBlurIntensity;


/*  시간 값들 */
float  g_fAnimIndex;
float2 g_vLifeTime;
float2 g_vDiffuseUVMoveTime;
float2 g_vMaskingUVMoveTime;
float2 g_vNoiseUVMoveTime;
float2 g_vDistortionUVMoveTime;
float2 g_vDissolveUVMoveTime;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPrevProjPos : TEXCOORD3;
};

struct PS_IN
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPrevProjPos : TEXCOORD3;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vRevealage : SV_TARGET1;
};

struct PS_BLNED_OUT
{
    float4 vDiffuse : SV_TARGET0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4 w = In.vBlendWeight;
    float sumW = max(dot(w, 1.0f), 1e-6f);
    w /= sumW;

    matrix BoneMatrix =
        mul(g_OffsetMatrix[In.vBlendIndex.x],
            mul(g_BoneBuffer[In.vBlendIndex.x].LocalCombined, w.x))
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            mul(g_BoneBuffer[In.vBlendIndex.y].LocalCombined, w.y))
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            mul(g_BoneBuffer[In.vBlendIndex.z].LocalCombined, w.z))
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            mul(g_BoneBuffer[In.vBlendIndex.w].LocalCombined, w.w));

    matrix PrevBoneMatrix =
        mul(g_OffsetMatrix[In.vBlendIndex.x],
            mul(g_PrevBoneBuffer[In.vBlendIndex.x].LocalCombined, w.x))
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            mul(g_PrevBoneBuffer[In.vBlendIndex.y].LocalCombined, w.y))
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            mul(g_PrevBoneBuffer[In.vBlendIndex.z].LocalCombined, w.z))
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            mul(g_PrevBoneBuffer[In.vBlendIndex.w].LocalCombined, w.w));
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vPrevPosition = mul(vector(In.vPosition, 1.f), PrevBoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    vector vBinormal = mul(vector(In.vBinormal, 0.f), BoneMatrix);
    vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
    

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matPrevWV, matPrevWVP;
    matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
    matPrevWVP = mul(matPrevWV, g_PrevProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vBinormal, g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vPrevProjPos = mul(vPrevPosition, matPrevWVP);
    return Out;
}



float4 DrawEffect(PS_IN In)
{
    float4 vMtrlDiffuse = float4(0.f ,0.f ,0.f ,0.f);
    float4 vMtrlMask;
    float4 vMtrlNoise;
    float4 vMtrlDissolve;
    float4 vMtrlDistortion;
    
    float fAnimIndex = g_fAnimIndex;
    float2 vDiffuseTime = g_vDiffuseUVMoveTime;
    float2 vMaskingTime = g_vMaskingUVMoveTime;
    float2 vNoiseUVMoveTime = g_vNoiseUVMoveTime;
    float2 vDistortionUVMoveTime = g_vDistortionUVMoveTime;
    float2 vDissolveUVMoveTime = g_vDissolveUVMoveTime;
    float2 vLifeTime = g_vLifeTime;
    
    clip(vLifeTime.y - vLifeTime.x);
    
    
    if (g_isDiffuse == true)
    {
        float2 UV; // 이미지의 UV값
   
        UV = UV_Cutting(In.vTexcoord, g_vUVCutting, uint(fAnimIndex));
        
        if (g_isDiffuseUVMove)
        {
            UV += SelectLerpUV((g_vDiffuseUVGainAmount / g_vUVCutting), (vDiffuseTime.x / vDiffuseTime.y), g_iDiffuseMoveLerpOption);
        }

        if (g_isDistortion == true)
        {

            float2 vDistortionUV = In.vTexcoord + SelectLerpUV(g_vMaskDistortionUVGainAmount, (vDistortionUVMoveTime.x / vDistortionUVMoveTime.y), g_iMaskDistortionMoveLerpOption);
            
            vMtrlDistortion = g_DistortionTexture.Sample(DefaultSampler, vDistortionUV);
            
            
            float fDistortionIntensity = g_fNoiseDistortionIntensity;
            
            if (g_vDistortionTime.y != 0)
            {
                fDistortionIntensity = g_fNoiseDistortionIntensity * ((vLifeTime.x / vLifeTime.y));
            }
            
            //디스토션(노이즈) 텍스쳐로 uv를 왜곡함
            UV = UV + (vMtrlDistortion - 0.5f).r * fDistortionIntensity;

            
        }
        
        float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, UV);
        
        if (g_isDiffuse_R)
            vMtrlDiffuse.r = vDiffuse.r;
        
        if (g_isDiffuse_G)
            vMtrlDiffuse.g = vDiffuse.g;
        
        if (g_isDiffuse_B)
            vMtrlDiffuse.b = vDiffuse.b;
        
        if ((g_isDiffuse_R || g_isDiffuse_G || g_isDiffuse_B) == false) // RGB중 하나도 켜지지 않았더라면 
            vMtrlDiffuse = vDiffuse;
       
        if (g_vColor.a > 0)
            vMtrlDiffuse = saturate(g_vColor + vMtrlDiffuse);
        
    }
    else
    {
        vMtrlDiffuse = g_vColor;
    }
    
    
    /*  노이즈 */
   
    if (g_isNoise)
    {
        float2 vNoiseCut = g_vUVNoiseCutting;
        
        if (length(vNoiseCut) < FLT_EPSILON3)
            vNoiseCut = float2(1.f, 1.f);
        
        float2 vNoiseUV = UV_Cutting(In.vTexcoord, vNoiseCut, uint(fAnimIndex));
        
        if (g_isNoiseUVMove == true)
            vNoiseUV = In.vTexcoord + SelectLerpUV(g_vNoiseUVGainAmount, (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y), g_iNoiseMoveLerpOption);
        
        vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);
        
        if (g_isNoiseAlpha)
            {
                    
            float fNoiseValue = vMtrlNoise.r;
        
            if (g_isNoise_G == true)
                fNoiseValue = vMtrlNoise.g;
        
            if (g_isNoise_B == true)
                fNoiseValue = vMtrlNoise.b;
            
            vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * fNoiseValue.r);
        }
        
        if (g_isNoiseColor)
        {
            float fNoiseValue = vMtrlNoise.r;
        
            if (g_isNoise_G == true)
                fNoiseValue = vMtrlNoise.g;
        
            if (g_isNoise_B == true)
                fNoiseValue = vMtrlNoise.b;
            
            vMtrlDiffuse.rgb *= fNoiseValue;
        }
        
        if (length(g_vNoiseColor) > FLT_EPSILON5)
            vMtrlDiffuse.rgb *= g_vNoiseColor.rgb;


    }
    
    /* 마스크 */ 
    if (g_isMasking == true)
    {
        float2 vMaskTexcoord = In.vTexcoord;
        
        vMaskTexcoord = UV_Cutting(In.vTexcoord, g_vUVMaskCutting, uint(fAnimIndex));
        
        vMaskTexcoord += g_vMaskOffset;
        // 마스크 디스토션 
        if (g_isDistortion == true)
        {

            float2 vDistortionUV = In.vTexcoord + SelectLerpUV(g_vMaskDistortionUVGainAmount, (vDistortionUVMoveTime.x / vDistortionUVMoveTime.y), g_iMaskDistortionMoveLerpOption);
            
            vMtrlDistortion = g_DistortionTexture.Sample(DefaultSampler, vDistortionUV);
            
            
            
            float fDistortionIntensity = g_fNoiseDistortionIntensity;
            
            if (g_vDistortionTime.y != 0)
            {
                fDistortionIntensity = g_fNoiseDistortionIntensity * ((vLifeTime.x / vLifeTime.y));
            }
            
            //디스토션(노이즈) 텍스쳐로 uv를 왜곡함
            vMaskTexcoord = vMaskTexcoord + (vMtrlDistortion - 0.5f).r * fDistortionIntensity;
          
        }
        
        if (g_isMaskUVMove)
        {
            if (vMaskingTime.y > 0)
                vMaskTexcoord += SelectLerpUV(g_vMaskingUVGainAmount / g_vUVMaskCutting, (vMaskingTime.x / vMaskingTime.y), g_iMaskMoveLerpOption);
        }
        
        if (g_isMaskClampSample == true)
            vMtrlMask = g_MaskingTexture.Sample(ClampLinearSampler, vMaskTexcoord);
        else
            vMtrlMask = g_MaskingTexture.Sample(DefaultSampler, vMaskTexcoord);
        
        float fMaskValue = vMtrlMask.r;
        
        if (g_isMask_G == true)
            fMaskValue = vMtrlMask.g;
        
        if (g_isMask_B == true)
            fMaskValue = vMtrlMask.b;
        
        float fSoftMask;
    
        if (g_fSoftMask > FLT_EPSILON5)
            fSoftMask = saturate((fMaskValue - g_fSoftMaskEdge) * g_fSoftMask);
        else
            fSoftMask = fMaskValue;
        
        vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * fSoftMask);
        
   
    }
    else
    {        
        vMtrlMask.r = 1.f;
    }

    
    vMtrlDiffuse.a *= g_fDiffuseAlpha;
      
 
    clip(vMtrlDiffuse.a - FLT_EPSILON7);
    
    if (g_isDissolve == true)
    {
        float fTimeX = (vLifeTime.x - g_fDissolveDelay);
        float fTimeY = (vLifeTime.y - g_fDissolveDelay);
        float fTimeRatio = 0;
        
        if (g_fDissolveDelay <= FLT_EPSILON5)
        {
            fTimeRatio = vLifeTime.x / vLifeTime.y;
        }
        else if (fTimeX > 0 && fTimeY != 0)
        {
            fTimeRatio = saturate(fTimeX / fTimeY);
        }
      

        if (g_isNomalDissolve)
        {
            if (g_isReverseDissolve == false)
            {
                vMtrlDiffuse.a *= saturate((0.96f - fTimeRatio));
            }
            else
            {
                vMtrlDiffuse.a *= vLifeTime.x / vLifeTime.y;

            }
           
        }
        
        float2 vDissolveUV = In.vTexcoord + SelectLerpUV(g_vDissolveUVGainAmount, (vDissolveUVMoveTime.x / vDissolveUVMoveTime.y), 0);
            
        
        if (g_fDissolveCutRatio > FLT_EPSILON5)
        {
            if (g_isDissolveMove)
                vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, vDissolveUV);
            else
                vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
            
            float fDissolveValue = vMtrlDissolve.r;
        
            if (g_isDissolve_G == true)
                fDissolveValue = vMtrlDissolve.g;
        
            if (g_isDissolve_B == true)
                fDissolveValue = vMtrlDissolve.b;
        
            float fSoftDissolve;
    
            if (g_fDissolveSoftMask > FLT_EPSILON5)
                fSoftDissolve = saturate((fDissolveValue - g_fDissolveMaskEdge) * g_fDissolveSoftMask);
            else
                fSoftDissolve = fDissolveValue;
        
            vMtrlDissolve.r = fSoftDissolve;
       
            if (g_isReverseDissolve == true)
            {
                
                clip((vLifeTime.x / vLifeTime.y) - vMtrlDissolve.r);
            }
            else
            {
            
                        
                if (g_fDissolveCutRatio <= fTimeRatio) // 일정 수준 이상 넘어가지 못하도록 막기
                {
                    fTimeRatio = g_fDissolveCutRatio;

                }
            
                float fFade = smoothstep(fTimeRatio - g_vDissolveSmoothRange.x, fTimeRatio + g_vDissolveSmoothRange.y, vMtrlDissolve.r);

            
                
                if (g_isNoDissolveSmoothStep == true)
                {
                    fFade = smoothstep(fTimeRatio, fTimeRatio + FLT_EPSILON5, vMtrlDissolve.r);

                }
            
                vMtrlDiffuse.a *= fFade;

            }
        }

     
        /* */
        

       
        clip(vMtrlDiffuse.a - FLT_EPSILON7);
    }
    

    return vMtrlDiffuse;
}

float4 EmissiveDraw(PS_IN In, float4 Diffuse)
{
    float2 vLifeTime = g_vLifeTime;
    
    float4 vEmissiveMtrl = float4(0.f, 0.f, 0.f, 0.f);
    float2 CenteredUV = In.vTexcoord - 0.5f;
    float fEmissive = 0.f;
    float fEmissiveStrength = g_fEmissiveStrength;
    
    if (g_fEmissiveColorCut >= FLT_EPSILON5)
    {
        if (g_fEmissiveColorCut > length(Diffuse.rgb))
        {
            return vEmissiveMtrl;
        }

    }
    
    if (g_fRadius >= FLT_EPSILON5)
    {
        float fDistance = saturate(length(CenteredUV) / g_fRadius); // 마스크의 크기를 어느정도할지 
    
        float fSoftEdge = pow(saturate(1.0f - fDistance), g_fSoftenExp) * g_fSoftStrength; //SoftenExp  엣지감쇠지수 , Strength
   
        float fCore = pow(saturate(1.0 - fDistance * 2.f), max(0.001, g_fCoreBoost)) * g_fCoreBoost; // CoreBust 중심강조
    
        fEmissive = fSoftEdge + fCore;
   
        if (g_isEmissive == true)
        {
            vEmissiveMtrl = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
        }
        else
        {
            vEmissiveMtrl = g_vEmissive;
        }
    
        if (g_isEmissiveDissolve == true)
        {
            fEmissiveStrength *= (1.f - vLifeTime.x / vLifeTime.y);
        
            if (fEmissiveStrength <= 0.05f)
                fEmissiveStrength = 0;
        
        }
    
        if (g_isEmissiveDissolveReverse == true)
        {
            fEmissiveStrength *= (vLifeTime.x / vLifeTime.y);
        
        }
    }
    
    return saturate(vEmissiveMtrl * fEmissive * fEmissiveStrength);
    
}



PS_OUT BlendedWeight(float4 fDiffuse, float fLinearZ)
{
    PS_OUT Out = (PS_OUT)0;
    
    float3 vColor = fDiffuse.rgb;
    float fAlpha = fDiffuse.a;
    

    //    float fWeight =
    //max(
    //  min(1.0, max(max(vColor.r, vColor.g), vColor.b) * fAlpha), fAlpha)
    //    *
    //  clamp(0.01 / (1e-4 + pow(fLinearZ / 200, 4.0)), 0.01f, 1000);
    
    
    
    //float fWeight = clamp(0.03f / (1e-5 + pow(fLinearZ, 4.0f)), 0.01, 3e3);
    //fWeight = max(fWeight, 1.0);
    
    //이게 오리지널 웨이트 함수
    
    //float fWeight = pow(fLinearZ, -2.5);
    
    // float fWeight = clamp(pow(fLinearZ, -2.5f), 1.0f, 1000.0f);
    
    
    float linearZPositive = max(fLinearZ, 1e-6f);
    float fWeight = clamp(pow(linearZPositive, -2.5f), 1.0f, 10.0f);

    
    Out.vDiffuse = float4(vColor.rgb * fAlpha, fAlpha) * fWeight;
   
    Out.vRevealage.r = fAlpha;


    return Out;
}

float4 RimLight(PS_IN In)
{
    float4 vRimLight = float4(0.f, 0.f, 0.f, 0.f);
    
    if (g_isRimLight == true)
    {
        float4 vDir = normalize(g_vCamPosition - In.vWorldPos);
        vDir.w = 0.f;
        
        vRimLight = saturate(dot(float4(In.vNormal.rgb, 0.f), vDir));

        vRimLight = pow(1.f - vRimLight, g_fRimLightPower);
        vRimLight = vRimLight * g_vRimLightColor * g_fRimLightStrength; 
    }
    
    return vRimLight;
}


PS_OUT PS_NON_NORMALMAP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    float4 vMtrlDiffuse;
    
    vMtrlDiffuse = DrawEffect(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In, vMtrlDiffuse).rgb;
    
    vMtrlDiffuse += RimLight(In);


    Out = BlendedWeight(vMtrlDiffuse, In.vProjPos.w);

    return Out;
}
  

PS_BLNED_OUT PS_BLEND(PS_IN In)
{
    PS_BLNED_OUT Out;
    
    float4 vMtrlDiffuse;
    
    vMtrlDiffuse = DrawEffect(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In, vMtrlDiffuse).rgb;
    
    vMtrlDiffuse += RimLight(In);

    Out.vDiffuse = vMtrlDiffuse;
    return Out;
}

PS_BLNED_OUT PS_BLUR(PS_IN In)
{
    PS_BLNED_OUT Out;
    
    float4 vMtrlDiffuse;
    float2 vLifeTime = g_vLifeTime;
    
    vMtrlDiffuse = DrawEffect(In);
    
    if (g_isBlurColor == true)
    {
        clip(length(vMtrlDiffuse.rgb) - FLT_EPSILON5);
        
        vMtrlDiffuse = float4(g_vBlurColor.rgb, vMtrlDiffuse.a);
    }

   
    vMtrlDiffuse.rgb += EmissiveDraw(In, vMtrlDiffuse).rgb;
    
    vMtrlDiffuse.a *= 5.f;
    
    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a);
    
    float fBulrIntensity = g_fBlurIntensity;
    
    if (g_isBlurDissolve == true)
    {
        fBulrIntensity *= 1.f - (vLifeTime.x / vLifeTime.y);
       
        fBulrIntensity = saturate(fBulrIntensity);
    }
   
    Out.vDiffuse = float4(vMtrlDiffuse.rgb * fBulrIntensity, vMtrlDiffuse.a);
    
    return Out;
}

PS_BLNED_OUT PS_BLOOM(PS_IN In)
{
    PS_BLNED_OUT Out;

    float2 vLifeTime = g_vLifeTime;
    
    float4 vMtrlDiffuse = float4(0.f, 0.f, 0.f, 0.f);
    
    vMtrlDiffuse = DrawEffect(In);
    
   
    vMtrlDiffuse.rgb += EmissiveDraw(In, vMtrlDiffuse).rgb;
    
    vMtrlDiffuse += RimLight(In);
    
    float fBloomStrength = g_fBloomStrength;
    
    if (g_isBloomDissolve == true)
    {
        fBloomStrength = g_fBloomStrength * (1.f - (vLifeTime.x / vLifeTime.y));
    }
    
      
    if (g_isBloomReverseDissolve == true)
    {
        fBloomStrength = g_fBloomStrength * ((vLifeTime.x / vLifeTime.y));
    }
    
    Out.vDiffuse = vMtrlDiffuse * fBloomStrength;
    
    return Out;
}

technique11 DefaultTechnique
{
//0
    pass NON_NOMALMAP
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_NON_NORMALMAP();
    }
//1
    pass WEIGHTBLEND
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_WB_Acc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_NON_NORMALMAP();
    }
//2
    pass BLEND
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_BLEND();
    }

//3
    pass Blur
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_BLUR();
    }

//4
    pass BLOOM
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_BLOOM();
    }
}




