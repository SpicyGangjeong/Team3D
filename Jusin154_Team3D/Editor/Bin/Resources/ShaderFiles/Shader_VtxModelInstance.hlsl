#include "Engine_Shader_Defines.hlsli"

float PI = 3.141592;

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
    float2 vNoiseUVMoveTime;
    float2 vAniTime;
    
    float2 vAniIndex;
};


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture : register(t0);

Texture2D g_NormalTexture : register(t1);
Texture2D g_MaskingTexture : register(t2);
Texture2D g_DissolveTexture : register(t3);
Texture2D g_NoiseTexture : register(t4);

StructuredBuffer<ParticleValue> g_ParticleValue : register(t5);


Texture2D g_EmissiveTexture : register(t6);
Texture2D g_DepthStencilTexture : register(t7);



float4 g_vCamPosition;

vector g_vColor;

float2 g_vDiffuseUVGainAmount;
float2 g_vMaskingUVGainAmount;

float2 g_vDiffuseNoiseUVGainAmount;
float2 g_vMaskNoiseUVGainAmount;

//
int    g_iMaskMoveLerpOption;
int    g_iDiffuseMoveLerpOption;
int    g_iMaskNoiseMoveLerpOption;
int    g_iDiffuseNoiseMoveLerpOption;
//

float2 g_vUVCutting;
float2 g_vUVMaskCutting;


int   g_iBlurWeight; // 블러 강도
float g_fBlurIntensity; //블러 세기
float g_fNoiseDistortionIntensity; // 노이즈 왜곡 세기

float4 g_vEmissive;
float g_fColorOption;
float g_fEmissiveCutAlpha;

bool g_isDiffuse;
bool g_isMasking;
bool g_isDissolve;
bool g_isNoise;
bool g_isEmissive;

bool g_isDiffuseUVMove;
bool g_isMaskUVMove;

float g_fFar;

float2 SelectLerpUV(float2 fAmount, float _fRatio, int iSelectOption)
{
    if (iSelectOption < 0)
        return float2(0 , 0);
    
    float fRatio = _fRatio;
    switch (iSelectOption)
    {
        case 0:
            fRatio = fRatio; // Linear 
            break;
        case 1:
            fRatio = fRatio * fRatio; // EaseInQuad 후반에 속도 증가
            break;
        case 2:
            fRatio = 1 - (1 - fRatio * fRatio); // EaseOutQuad 초반에 속도 증가
            break;
        case 3:
            fRatio = fRatio * fRatio * fRatio; // EaseInCubic  더 강하게 후반 속도 증가
            break;
        case 4:
            fRatio = 1 - (1 - fRatio * fRatio * fRatio); // EaseOutCubic 더 강하게 초반 속도 증가
            break;
        case 5:
            fRatio = 0.5f * (1 - cos(PI * fRatio)); // EaseInOutSin 사인 곡선 
            break;
        case 6:
            fRatio = sin(13 * PI * fRatio) * (1 - fRatio) * (1 - fRatio); // EaseInBack 뒤로갔다가 앞으로
            break;
        case 7:
            fRatio = pow(2, 10 * (fRatio - 1)); // Expo 지수 함수
            break;
        case 8:
            fRatio = 1 - pow(1 - fRatio * fRatio, 0.5); // 원형 궤적     
            break;
    }
    
    return fAmount * fRatio;

}

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

    row_major matrix matW, matWV, matWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = mul(g_WorldMatrix, TransformMatrix);
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


PS_OUT BlendedWeight(vector fDiffuse, float fLinearZ , PS_IN In , float fProjZ)
{
    PS_OUT Out;
    

    
    float3 vColor = fDiffuse.rgb;
    float fAlpha = fDiffuse.a;
    

//    float fWeight =
//max(
//    min(1.0, max(max(vColor.r, vColor.g), vColor.b) * fAlpha),
//    fAlpha
//) *
//clamp(
//    0.01 / (1e-4 + pow(fLinearZ / 200, 4.0)),
//   0.01f, 1000
//);

    
    
    float fWeight = clamp(0.03f / (1e-5 + pow(fLinearZ, 2.0f)), 0.01, 3e3);
    fWeight = max(fWeight, 1.0);
    
    //이게 오리지널 웨이트 함수
    
    //float fWeight = pow(fLinearZ, -2.5);
    //float fWeight = clamp(pow(fLinearZ, -2.5f), 1.0f, 1000.0f);

    Out.vDiffuse = vector(vColor.rgb * fAlpha, fAlpha) * fWeight;
    

    Out.vRevealage.a = fAlpha;

    return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
   
    PS_OUT Out;
    vector vMtrlDiffuse;

    vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3x3 NormalMap_WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
    
    float3 vNormal = mul(vMtrlNormal.xyz * 2.f - 1.f, NormalMap_WorldMatrix);

    
    //Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse.a = saturate(In.vLifeTime.y - In.vLifeTime.x);
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_NON_NORMALMAP(PS_IN In)
{
   
    PS_OUT Out;
    
    vector vMtrlDiffuse;
    vector vMtrlMask;
    vector vMtrlNoise;
    vector vMtrlDissolve;
    
    float  fAnimIndex = g_ParticleValue[In.iGPUIndex].vAniIndex.x;
    float2 vDiffuseTime = g_ParticleValue[In.iGPUIndex].vDiffuseUVMoveTime;
    float2 vMaskingTime = g_ParticleValue[In.iGPUIndex].vMaskingUVMoveTime;
    float2 vNoiseUVMoveTime = g_ParticleValue[In.iGPUIndex].vNoiseUVMoveTime;
    
    if (g_isDiffuse == true)
    {
        float2 UV; // 이미지의 UV값
   
        UV = UV_Cutting(In.vTexcoord, g_vUVCutting, int(fAnimIndex.x));
        
        if (g_isDiffuseUVMove)
        {   
            UV += SelectLerpUV((g_vDiffuseUVGainAmount / g_vUVCutting), (vDiffuseTime.x / vDiffuseTime.y), g_iDiffuseMoveLerpOption);
        }

        /* 노이즈 */
        if (g_isNoise == true)
        {
            
            float2 vNoiseUV = In.vTexcoord + SelectLerpUV(g_vDiffuseNoiseUVGainAmount, (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y), g_iDiffuseNoiseMoveLerpOption);
            
            vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);

            //0.5f를 빼는 이유는 중심을 0으로 옮겨서 양음수 방향으로 offset을 줄수 있다.
            vMtrlNoise.rg -= 0.5f;
                        
            //아마 분할되었다면 noise도 분할해야할듯 
            vMtrlNoise.rg /= g_vUVCutting;
            
            //노이즈 텍스쳐로 uv를 왜곡함

            UV = UV + (vMtrlNoise).rg * g_fNoiseDistortionIntensity;
        }
        
 
        /* 최종 색깔  */
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, UV);
        
        if (g_vColor.a > 0)
            vMtrlDiffuse += g_vColor;
        
        if (vMtrlDiffuse.a < 0.3f)
            discard;

    }
    else
    {
        vMtrlDiffuse = g_vColor;
    }
    
    if (g_isMasking == true)
    {
        float2 vMaskTexcoord = In.vTexcoord;
        
        vMaskTexcoord = UV_Cutting(In.vTexcoord, g_vUVMaskCutting, int(fAnimIndex.x));
        
        if (g_isNoise == true)
        {
            float2 UV; // 이미지의 UV값
               
            float2 vNoiseUV = In.vTexcoord + SelectLerpUV(g_vMaskNoiseUVGainAmount, (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y), g_iMaskNoiseMoveLerpOption);
            
            vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);
            
            //노이즈 텍스쳐로 uv를 왜곡함
            vMaskTexcoord = vMaskTexcoord + (vMtrlNoise - 0.5f).rg * g_fNoiseDistortionIntensity;
        }
        
        if (g_isMaskUVMove)
        {
            if (vMaskingTime.y > 0)
                vMaskTexcoord += SelectLerpUV(g_vMaskingUVGainAmount / g_vUVMaskCutting, (vMaskingTime.x / vMaskingTime.y), g_iMaskMoveLerpOption);
        }
        
        vMtrlMask = g_MaskingTexture.Sample(PointSampler, vMaskTexcoord);
    }
    else
    {
        vMtrlMask.r = 1.f;
    }
    

    
    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlMask.r);
    
    if (vMtrlDiffuse.a <= 0.f)
        discard;
    
    if (g_isDissolve == true)
    {
        vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
        
        if (vMtrlDissolve.r < (In.vLifeTime.x / In.vLifeTime.y))
            discard;
    }
    

    
    // 색깔 추가할 처리 (이미시브)
    
    float4 vEmissiveMtrl;
    
    if (g_isEmissive == true)
    {
        vEmissiveMtrl = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    }

    int2 texel = int2(In.vPosition.xy);
    
    float fDepthStencilValue = g_DepthStencilTexture.Load(int3(texel, 0));
    

    float bias = 0.0005f;
    
    if (fDepthStencilValue + bias < In.vProjPos.z / In.vProjPos.w)
        discard;
    
    Out = BlendedWeight(vMtrlDiffuse, In.vProjPos.w, In, In.vProjPos.z / In.vProjPos.w);
    
        
    //if (vMtrlDiffuse.a >= g_fEmissiveCutAlpha)
    //    Out.vColorTarget = vector(g_vEmissive.rgb + vEmissiveMtrl.rgb, g_fColorOption / 10.f);
    //else
    //    Out.vColorTarget = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

struct VS_BLUR_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    uint iGPUIndex : TEXCOORD2;
};

VS_BLUR_OUT VS_BLUR(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_BLUR_OUT Out = (VS_BLUR_OUT) 0;

    row_major matrix matW, matWV, matWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = mul(g_WorldMatrix, TransformMatrix);
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.iGPUIndex = iGPUIndex;


    return Out;
}

struct PS_BLUR_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    uint iGPUIndex : TEXCOORD2;

};

struct PS_BLUR_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBlurWeight : SV_TARGET1;
};


PS_BLUR_OUT PS_BLUR(PS_BLUR_IN In)
{
   
    PS_BLUR_OUT Out;
    
       
    vector vMtrlDiffuse;
    vector vMtrlMask;
    vector vMtrlNoise;
    vector vMtrlDissolve;
    
    float fAnimIndex = g_ParticleValue[In.iGPUIndex].vAniIndex.x;
    float2 vDiffuseTime = g_ParticleValue[In.iGPUIndex].vDiffuseUVMoveTime;
    float2 vMaskingTime = g_ParticleValue[In.iGPUIndex].vMaskingUVMoveTime;
    float2 vNoiseUVMoveTime = g_ParticleValue[In.iGPUIndex].vNoiseUVMoveTime;
    
    if (g_isDiffuse == true)
    {
        float2 UV; // 이미지의 UV값
   
        UV = UV_Cutting(In.vTexcoord, g_vUVCutting, int(fAnimIndex.x));
        
        if (g_isDiffuseUVMove)
        {
            UV += SelectLerpUV((g_vDiffuseUVGainAmount / g_vUVCutting), (vDiffuseTime.x / vDiffuseTime.y), g_iDiffuseMoveLerpOption);
        }

        /* 노이즈 */
        if (g_isNoise == true)
        {
            
            float2 vNoiseUV = In.vTexcoord + SelectLerpUV(g_vDiffuseNoiseUVGainAmount, (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y), g_iDiffuseNoiseMoveLerpOption);
            
            vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);

            //0.5f를 빼는 이유는 중심을 0으로 옮겨서 양음수 방향으로 offset을 줄수 있다.
            vMtrlNoise.rg -= 0.5f;
                        
            //아마 분할되었다면 noise도 분할해야할듯 
            vMtrlNoise.rg /= g_vUVCutting;
            
            //노이즈 텍스쳐로 uv를 왜곡함

            UV = UV + (vMtrlNoise).rg * g_fNoiseDistortionIntensity;
        }
        
 
        /* 최종 색깔  */
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, UV);
        
        if (g_vColor.a > 0)
            vMtrlDiffuse += g_vColor;
        
        if (vMtrlDiffuse.a < 0.3f)
            discard;

    }
    else
    {
        vMtrlDiffuse = g_vColor;
    }
    
    if (g_isMasking == true)
    {
        float2 vMaskTexcoord = In.vTexcoord;
        
        vMaskTexcoord = UV_Cutting(In.vTexcoord, g_vUVMaskCutting, int(fAnimIndex.x));
        
        if (g_isNoise == true)
        {
            float2 UV; // 이미지의 UV값
               
            float2 vNoiseUV = In.vTexcoord + SelectLerpUV(g_vMaskNoiseUVGainAmount, (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y), g_iMaskNoiseMoveLerpOption);
            
            vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);
            
            //노이즈 텍스쳐로 uv를 왜곡함
            vMaskTexcoord = vMaskTexcoord + (vMtrlNoise - 0.5f).rg * g_fNoiseDistortionIntensity;
        }
        
        if (g_isMaskUVMove)
        {
            if (vMaskingTime.y > 0)
                vMaskTexcoord += SelectLerpUV(g_vMaskingUVGainAmount / g_vUVMaskCutting, (vMaskingTime.x / vMaskingTime.y), g_iMaskMoveLerpOption);
        }
        
        vMtrlMask = g_MaskingTexture.Sample(PointSampler, vMaskTexcoord);
    }
    else
    {
        vMtrlMask.r = 1.f;
    }
    

    
    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlMask.r);
    
    if (g_isDissolve == true)
    {
        vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
        
        if (vMtrlDissolve.r < (In.vLifeTime.x / In.vLifeTime.y))
            discard;
    }

    
    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlMask.r);
    
    if (g_isDissolve == true)
    {
        vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
        
        if (vMtrlDissolve.r < (In.vLifeTime.x / In.vLifeTime.y))
            discard;
    }
    
    Out.vDiffuse = vMtrlDiffuse * g_fBlurIntensity;
    Out.vBlurWeight.r = g_iBlurWeight / 32.f;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Model
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NON_NOMALMAP
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_WB_Acc, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NON_NORMALMAP();
    }

    pass Blur
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR();
    }



}




