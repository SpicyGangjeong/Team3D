#include "Engine_Shader_Defines.hlsli"
#include "Engine_Shader_Functions.hlsli"

struct ParticleValue
{
    float fSpeed;
    float fRotaionSpeed;

    float4 vOriginRight;
    float4 vOriginUp;
    float4 vOriginLook;
    float4 vOriginTranslation;
    
    float2 vMaskingUVMoveTime;
    float2 vDiffuseUVMoveTime;
    float2 vDistortionUVMoveTime;
    float2 vNoiseUVMoveTime;
    float2 vAniTime;
    float2 vDissolveUVMoveTime;
    
    float2 vAniIndex;
    float  fGravity;
    
    float3 vSinAmount;
    float3 vDeltaAngle;
    float3 vDeltaAxisAngle;
    
    float  fDrag;
    float3 vPivot;
    
    float  fSizeDrag;
    float3 vDeltaSize;
    float2 vDelay;
    
    bool  isCompareStop;
    float fCollisionTime;
    float fDropAttenuation;
    
    float3 vVelocity;

};


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ProjMatrixInv, g_ViewMatrixInv;

Texture2D g_DiffuseTexture : register(t0);

Texture2D g_NormalTexture : register(t1);
Texture2D g_MaskingTexture : register(t2);
Texture2D g_DissolveTexture : register(t3);
Texture2D g_NoiseTexture : register(t4);

StructuredBuffer<ParticleValue> g_ParticleValue : register(t5);


Texture2D g_EmissiveTexture : register(t6);
Texture2D g_DepthStencilTexture : register(t7);
Texture2D g_DistortionTexture : register(t8);
Texture2D g_DepthTexture : register(t9);


float4 g_vCamPosition;

vector g_vColor;

float2 g_vDiffuseUVGainAmount;
float2 g_vMaskingUVGainAmount;
float2 g_vNoiseUVGainAmount;

int    g_iMaskMoveLerpOption;
int    g_iDiffuseMoveLerpOption;
int    g_iNoiseMoveLerpOption;

//
float2 g_vDistortionTime;
float2 g_vMaskDistortionUVGainAmount;


int    g_iMaskDistortionMoveLerpOption;
int    g_iDiffuseDistortionMoveLerpOption;
//

float2 g_vUVCutting;
float2 g_vUVMaskCutting;

float g_fDiffuseAlpha;

int   g_iBlurWeight; // 블러 강도
float g_fBlurIntensity; //블러 세기
float g_fNoiseDistortionIntensity; // 디스토션 왜곡 세기

float4 g_vEmissive;
float  g_fEmissiveStrength;
    
float g_fSoftenExp;
float g_fSoftStrength;
float g_fCoreBoost;
float g_fRadius;

    
float g_fSoftMaskEdge;
float g_fSoftMask;

bool g_isDiffuse;
bool g_isMasking;
bool g_isDissolve;
bool g_isNoise;
bool g_isEmissive;
bool g_isDistortion;

bool g_isDiffuseUVMove;
bool g_isMaskUVMove;
bool g_isNoiseUVMove;

bool g_isReverseDissolve;

bool g_isEmissiveDissolve;
bool g_isEmissiveDissolveReverse;

bool g_isMaskClampSample;

bool g_isNoiseColor;
bool g_isNoiseAlpha;
bool g_isNomalDissolve;


bool g_isBlurNoEmissive;
bool g_isTexBlur;

bool g_isBlurDissolve;
bool g_isBlurReverseDissolve;

float g_fBluringStrength;

bool    g_isBloomDissolve;
bool    g_isBloomReverseDissolve;

float   g_fBloomStrength;
int     g_iBloomType;

float g_fFar;

float   g_fDissolveDelay;
float   g_fReverseDissolveDelay;
float2  g_vDissolveUVGainAmount;
bool    g_isDissolveMove;

float g_fDissolveMaskEdge;
float g_fDissolveSoftMask;
float g_fDissolveCutRatio;

float4 g_vDissolveColor;
float2 g_vDissolveColorCut;

float4 g_vNoiseColor;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
    uint iGPUIndex : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matW, matWV, matWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = mul(TransformMatrix, g_WorldMatrix);
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    


    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matW));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matW)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), matW)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), matW);
    Out.iGPUIndex = iGPUIndex;
    Out.vProjPos = vPosition;
    
    return Out;
}

VS_OUT VS_NOWORLD(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matW, matWV, matWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = TransformMatrix;
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    


    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matW));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matW)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), matW)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), matW);
    Out.iGPUIndex = iGPUIndex;
    Out.vProjPos = vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
    uint iGPUIndex : TEXCOORD4;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vRevealage : SV_TARGET1;
    float4 vColorTarget : SV_TARGET2;
};

float4 DrawEffect(PS_IN In)
{
    vector vMtrlDiffuse;
    vector vMtrlMask;
    vector vMtrlNoise;
    vector vMtrlDissolve;
    vector vMtrlDistortion;
    
    float  fAnimIndex = g_ParticleValue[In.iGPUIndex].vAniIndex.x;
    float2 vDiffuseTime = g_ParticleValue[In.iGPUIndex].vDiffuseUVMoveTime;
    float2 vMaskingTime = g_ParticleValue[In.iGPUIndex].vMaskingUVMoveTime;
    float2 vNoiseUVMoveTime = g_ParticleValue[In.iGPUIndex].vNoiseUVMoveTime;
    float2 vDistortionUVMoveTime = g_ParticleValue[In.iGPUIndex].vDistortionUVMoveTime;
    float2 vDissolveUVMoveTime = g_ParticleValue[In.iGPUIndex].vDissolveUVMoveTime;
    float2 vDelay = g_ParticleValue[In.iGPUIndex].vDelay;
    
    
    if (vDelay.x < vDelay.y)
    {
        discard;
    }
    
    if (g_isDiffuse == true)
    {
        float2 UV; // 이미지의 UV값
   
        UV = UV_Cutting(In.vTexcoord, g_vUVCutting, int(fAnimIndex));
        
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
                fDistortionIntensity = g_fNoiseDistortionIntensity * ((In.vLifeTime.x / In.vLifeTime.y));
            }
            
            //디스토션(노이즈) 텍스쳐로 uv를 왜곡함
            UV = UV + (vMtrlDistortion - 0.5f).r * fDistortionIntensity;

            
        }
               
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, UV);
       
        
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
        float2 vNoiseUV = In.vTexcoord;
        
        if (g_isNoiseUVMove == true)
            vNoiseUV = In.vTexcoord + SelectLerpUV(g_vNoiseUVGainAmount, (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y), g_iNoiseMoveLerpOption);
        
        vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);
        
        if (g_isNoiseAlpha)
            vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlNoise.r);
        
        if (g_isNoiseColor)
        {
            vMtrlDiffuse.rgb *= vMtrlNoise.rgb;
        }
        
        if (length(g_vNoiseColor) > FLT_EPSILON5)
            vMtrlDiffuse.rgb *= g_vNoiseColor;


    }
    
    /* 마스크 */ 
    if (g_isMasking == true)
    {
        float2 vMaskTexcoord = In.vTexcoord;
        
        vMaskTexcoord = UV_Cutting(In.vTexcoord, g_vUVMaskCutting, int(fAnimIndex));
        
        // 마스크 디스토션 
        if (g_isDistortion == true)
        {

            float2 vDistortionUV = In.vTexcoord + SelectLerpUV(g_vMaskDistortionUVGainAmount, (vDistortionUVMoveTime.x / vDistortionUVMoveTime.y), g_iMaskDistortionMoveLerpOption);
            
            vMtrlDistortion = g_DistortionTexture.Sample(DefaultSampler, vDistortionUV);
            
            
            
            float fDistortionIntensity = g_fNoiseDistortionIntensity;
            
            if (g_vDistortionTime.y != 0)
            {
                fDistortionIntensity = g_fNoiseDistortionIntensity * ((In.vLifeTime.x / In.vLifeTime.y));
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
            vMtrlMask = g_MaskingTexture.Sample(ClampSampler, vMaskTexcoord);
        else
            vMtrlMask = g_MaskingTexture.Sample(DefaultSampler, vMaskTexcoord);
        
            
        if (g_isTexBlur)
        {
            float4 fAccMask;

            
            float fTexel = g_fBluringStrength;
            
            if (g_isBlurDissolve)
            {
                fTexel = saturate(g_fBluringStrength * (In.vLifeTime.x / In.vLifeTime.y));
            }
            
            if (g_isBlurReverseDissolve)
            {
                fTexel = saturate(g_fBluringStrength * (1 - (In.vLifeTime.x / In.vLifeTime.y)));
            }
            
            for (int x = -2; x <= 2; x++)
            {
                for (int y = -2; y <= 2; y++)
                {
                    fAccMask += g_MaskingTexture.Sample(DefaultSampler, vMaskTexcoord + float2(x * fTexel, y * fTexel));
                }
            }
            
            fAccMask = fAccMask / 9.0;
            
             /* 연산 마스크 대입*/
            vMtrlMask.r = fAccMask;
        }
  
        
        float fSoftMask;
    
        if (g_fSoftMask > FLT_EPSILON5)
            fSoftMask = saturate((vMtrlMask.r - g_fSoftMaskEdge) * g_fSoftMask);
        else
            fSoftMask = vMtrlMask.r;
        
        vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * fSoftMask);
        
   
    }
    else
    {
        vMtrlMask.r = 1.f;
    }

    
    vMtrlDiffuse.a *= g_fDiffuseAlpha;
      
    if (vMtrlDiffuse.a <= FLT_EPSILON5)
        discard;
    
    
    
    if (g_isDissolve == true)
    {        
        float fTimeX = (In.vLifeTime.x - g_fDissolveDelay);
        float fTimeY = (In.vLifeTime.y - g_fDissolveDelay);
        float fTimeRatio = 0;
        
        if (g_fDissolveDelay <= FLT_EPSILON5)
        {
            fTimeRatio = In.vLifeTime.x / In.vLifeTime.y;
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
                vMtrlDiffuse.a *= In.vLifeTime.x / In.vLifeTime.y;

            }    
           
        }
        
        float2 vDissolveUV = In.vTexcoord + SelectLerpUV(g_vDissolveUVGainAmount, (vDissolveUVMoveTime.x / vDissolveUVMoveTime.y), 0);
            
        if (g_isDissolveMove)
            vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, vDissolveUV);
        else
            vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
         
        /* */
        float fSoftDissolve;
    
        if (g_fDissolveSoftMask > FLT_EPSILON5)
            fSoftDissolve = saturate((vMtrlDissolve.r - g_fDissolveMaskEdge) * g_fDissolveSoftMask);
        else
            fSoftDissolve = vMtrlDissolve.r;
        
        vMtrlDissolve.r = fSoftDissolve;
       
        if (g_isReverseDissolve == true)
        {
            if (vMtrlDissolve.r >= (In.vLifeTime.x / In.vLifeTime.y))
                discard;
        }
        else
        {
            if (g_fDissolveCutRatio <= fTimeRatio) // 일정 수준 이상 넘어가지 못하도록 막기
            {
                fTimeRatio = g_fDissolveCutRatio;

            }
            
            //if (vMtrlDissolve.r <= fTimeRatio + 0.1f)
            //    discard;
               
          
            //fTimeRatio = 1 - pow(1 - fTimeRatio, 3);
            
            float fFade = smoothstep(fTimeRatio - 0.1f, fTimeRatio + 0.1f, vMtrlDissolve.r);
            
            vMtrlDiffuse.a *= fFade;

        }
        
        if (g_vDissolveColorCut.y > 0.f)
        {
            if (vMtrlDiffuse.a > g_vDissolveColorCut.x && vMtrlDiffuse.a <= g_vDissolveColorCut.y)
            {
                vMtrlDiffuse.rgb += g_vDissolveColor.rgb;
            }
        }
        
        if (vMtrlDiffuse.a <= FLT_EPSILON5)
            discard;
    }
    
    /* 소프트 이펙트 */
    
    float2 vTexcoord;
    
    vTexcoord.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vTexcoord.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;
    
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    
    float fOldViewZ = vDepthDesc.y * g_fFar;
    
    float fDistance = fOldViewZ - In.vProjPos.w;
    
    vMtrlDiffuse.a = vMtrlDiffuse.a * saturate(fDistance);
    
    return vMtrlDiffuse;
}

float4 EmissiveDraw(PS_IN In)
{
    float4 vEmissiveMtrl = vector(0.f, 0.f, 0.f, 0.f);
    float2 CenteredUV = In.vTexcoord - 0.5f;
    float  fEmissive = 0.f;
    float  fEmissiveStrength = g_fEmissiveStrength;
    
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
            fEmissiveStrength *= (1.f - In.vLifeTime.x / In.vLifeTime.y);
        
            if (fEmissiveStrength <= 0.05f)
                fEmissiveStrength = 0;
        
        }
    
        if (g_isEmissiveDissolveReverse == true)
        {
            fEmissiveStrength *= (In.vLifeTime.x / In.vLifeTime.y);
        
        }
    }
    
    return saturate(vEmissiveMtrl * fEmissive * fEmissiveStrength);
    
}

PS_OUT BlendedWeight(vector fDiffuse, float fLinearZ)
{
    PS_OUT Out;
    
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
    
    float fWeight = clamp(pow(fLinearZ, -2.5f), 1.0f, 1000.0f);
    
    Out.vDiffuse = vector(vColor.rgb * fAlpha, fAlpha) * fWeight;
   
    Out.vRevealage.r = fAlpha;


    return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
   
    PS_OUT Out;
    vector vMtrlDiffuse;

    vMtrlDiffuse = DrawEffect(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;

    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_NON_NORMALMAP(PS_IN In)
{  
    PS_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = DrawEffect(In);
    
    int2 iTexel = int2(In.vPosition.xy);
    
    float fDepthStencilValue = g_DepthStencilTexture.Load(int3(iTexel, 0)).r;
    
    float fbias = 0.000005f;
    
    if (fDepthStencilValue <= In.vProjPos.z / In.vProjPos.w + fbias)
        discard;
    
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;
    
    Out = BlendedWeight(vMtrlDiffuse, In.vProjPos.w);
    
    //// 색깔 추가할 처리 (이미시브)
   
    //Out.vDiffuse  // 이미시브 스트랭스
    Out.vColorTarget = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}
   

VS_OUT VS_BLUR(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    row_major matrix matW, matWV, matWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = mul(TransformMatrix, g_WorldMatrix);
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.iGPUIndex = iGPUIndex;
    Out.vProjPos = vPosition;

    return Out;
}

VS_OUT VS_BLUR_NOWORLD(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    row_major matrix matW, matWV, matWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = TransformMatrix;
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.iGPUIndex = iGPUIndex;
    Out.vProjPos = vPosition;

    return Out;
}

struct PS_BLUR_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBlurWeight : SV_TARGET1;
};

struct PS_BLOOM_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

PS_BLUR_OUT PS_BLUR_NOEMISSIVE(PS_IN In)
{
    PS_BLUR_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse= DrawEffect(In);
    
    //// 색깔 추가할 처리 (이미시브)
    
    vMtrlDiffuse.a *= 5.f;
    
    //if (g_isDissolve == true)
    //{
    //    if (g_isNomalDissolve)
    //    {
    //        vMtrlDiffuse.a *= (0.95f - In.vLifeTime.x / In.vLifeTime.y);
    //    }
    //}
   
    Out.vDiffuse = vector(vMtrlDiffuse.rgb * g_fBlurIntensity , vMtrlDiffuse.a); 
    Out.vBlurWeight = g_iBlurWeight / 128.f;
    
    return Out;
}

PS_BLUR_OUT PS_BLUR(PS_IN In)
{
    PS_BLUR_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = DrawEffect(In);
    
    vMtrlDiffuse.a *= 5.f;
    
    //// 색깔 추가할 처리 (이미시브)
   
    //vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;
    
    //vMtrlDiffuse.a = 1.f;
    
    //if (g_isDissolve == true)
    //{
    //    if (g_isNomalDissolve)
    //    {
    //        vMtrlDiffuse.a *= (0.95f - In.vLifeTime.x / In.vLifeTime.y);
    //    }
    //}
   
    Out.vDiffuse = vector(vMtrlDiffuse.rgb * g_fBlurIntensity, vMtrlDiffuse.a);
    
    Out.vBlurWeight = g_iBlurWeight / 128.f;
    
    return Out;
}

PS_BLOOM_OUT PS_BLOOM(PS_IN In)
{
    PS_BLOOM_OUT Out;


    if (FLT_EPSILON5 >= (In.vLifeTime.x / In.vLifeTime.y))
        discard;
    
    vector vMtrlDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    
    vMtrlDiffuse = DrawEffect(In);
    
    
    //// 색깔 추가할 처리 (이미시브)
   
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;
    
    float fBloomStrength = g_fBloomStrength; 
    
    if (g_isBloomDissolve == true)
    {
        fBloomStrength = g_fBloomStrength * (1.f - (In.vLifeTime.x / In.vLifeTime.y));
    }
    
      
    if (g_isBloomReverseDissolve == true)
    {
        fBloomStrength = g_fBloomStrength * ((In.vLifeTime.x / In.vLifeTime.y));
    }
    
    if (vMtrlDiffuse.a <= 0.1f)
        discard;
   
    Out.vDiffuse = vector(vMtrlDiffuse.rgb * fBloomStrength, (float) g_iBloomType / 255.f);
    
    return Out;

}

PS_BLOOM_OUT PS_BLEND(PS_IN In)
{
    PS_BLOOM_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = DrawEffect(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;

    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_BLOOM_OUT PS_WEIGHTED_FOR_BLEND(PS_IN In)
{
    PS_BLOOM_OUT Out;
    
    vector vMtrlDiffuse;
    
    vMtrlDiffuse = DrawEffect(In);
    
    vMtrlDiffuse.rgb += EmissiveDraw(In).rgb;
    
    Out.vDiffuse = BlendedWeight(vMtrlDiffuse, In.vProjPos.w).vDiffuse;
    
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Model
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NON_NOMALMAP
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NON_NORMALMAP();
    }

    pass Blur
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR();
    }

    pass WEIGHTBLEND
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_WB_Acc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NON_NORMALMAP();
    }

    pass NO_WORLD
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_NOWORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NON_NORMALMAP();
    }

    pass BLUR_NO_WORLD
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BLUR_NOWORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR();
    }

 
    pass BLEND
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND();
    }

    pass BLEND_NOWORLD
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_NOWORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND();
    }

    pass BLOOM
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass BLOOM_NOWORLD
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_NOWORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM();
    }


    pass BLUR_NO_EMMISVE
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_NOEMISSIVE();
    }

    pass BLUR_NO_WORLD_NO_EMISSIVE
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BLUR_NOWORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_NOEMISSIVE();
    }

    pass WEIGHTBLEND_FOR_BLEND
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_WB_Acc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WEIGHTED_FOR_BLEND();
    }



    pass PARTICLE_DEPTH_STOP
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}




