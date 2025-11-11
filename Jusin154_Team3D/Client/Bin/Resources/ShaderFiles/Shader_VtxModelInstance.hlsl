#include "Engine_Shader_Defines.hlsli"


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

float4 g_vCamPosition;

vector g_vColor;

float2 g_vDiffuseUVGainAmount;
float2 g_vMaskingUVGainAmount;

float2 g_vDiffuseNoiseUVGainAmount;
float2 g_vMaskNoiseUVGainAmount;

float2 g_vUVCutting;

float  g_fBlurIntensity; //블러 세기
float  g_fNoiseDistortionIntensity; // 노이즈 왜곡 세기

float4 g_vEmissive;
float  g_fColorOption;
float  g_fEmissiveCutAlpha;

bool g_isDiffuse;
bool g_isMasking;
bool g_isDissolve;
bool g_isNoise;

bool g_isDiffuseUVMove;
bool g_isMaskUVMove;

float g_fFar;


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
    uint   iGPUIndex : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    
    float4 vProjPos = vector(In.vPosition, 1.f);
    
    Out.vPosition = mul(vProjPos, matWVP);
    Out.vProjPos = vProjPos;
    
    matWVP = mul(matWV, g_ProjMatrix);   
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), TransformMatrix));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), TransformMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), TransformMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;    
    Out.vLifeTime = In.vLifeTime;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.iGPUIndex = iGPUIndex;
    
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
    float4 vDiffuse: SV_TARGET0;
    float4 vColorTarget : SV_TARGET1;
};

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
    
    float fAnimIndex = g_ParticleValue[In.iGPUIndex].vAniIndex.x;
    float2 vDiffuseTime = g_ParticleValue[In.iGPUIndex].vDiffuseUVMoveTime;
    float2 vMaskingTime = g_ParticleValue[In.iGPUIndex].vMaskingUVMoveTime;
    float2 vNoiseUVMoveTime = g_ParticleValue[In.iGPUIndex].vNoiseUVMoveTime;
    
    if (g_isDiffuse == true)
    {
        float2 UV = In.vTexcoord; // 이미지의 UV값
    
        int iTotalFrame = g_vUVCutting.x * g_vUVCutting.y; // 이미지의 최대 프레임 (몇 곱하기 몇인지)
    
        int iCurrentFrame = int(fAnimIndex.x); // 현재 프레임이 어디 위치인지
    
        int iFrameX = iCurrentFrame % (int) g_vUVCutting.x; // 현재 x축의 위치(현재 이미지의 몇번째 칸을 보여줄 지)
        int iFrameY = iCurrentFrame / (int) g_vUVCutting.x; // 현재 y축의 위치(현재 이미지의 몇번째 줄을 보여줄 지)
    
        float fFreamWidth = 1.0 / g_vUVCutting.x; // 1.0 나누기 이미지 갯수를 해서 한칸에 얼마나 갈지 정해준다.
        float fFreamHeight = 1.0 / g_vUVCutting.y; // 1.0 나누기 이미지 갯수를 해서 한줄에 얼마나 갈지 정해준다.
    
        UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth; // 먼저 uv를 0~1이 아닌 0~fFrameWidth로 만든 다음에 한칸씩 옆으로 밀어준다.
        UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight; // 먼저 uv를 0~1이 아닌 0~fFrameHeight로 만든 다음에 한줄씩 밑으로 내려준다.
    
        
        if (g_isDiffuseUVMove)
        {
            UV += (g_vDiffuseUVGainAmount / g_vUVCutting) * (vDiffuseTime.x / vDiffuseTime.y);
        }

        /* 노이즈 */
        if (g_isNoise == true)
        {
            float2 vNoiseUV = In.vTexcoord + (g_vDiffuseNoiseUVGainAmount) * (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y);
            
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
        
        if (g_isNoise == true)
        {
         
            float2 vNoiseUV = In.vTexcoord + (g_vMaskNoiseUVGainAmount) * (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y);
            
            vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);
            
            //노이즈 텍스쳐로 uv를 왜곡함
            vMaskTexcoord = vMaskTexcoord + (vMtrlNoise - 0.5f).rg * g_fNoiseDistortionIntensity;
        }
        
        if (g_isMaskUVMove)
        {
            if (vMaskingTime.y > 0)
                vMaskTexcoord += g_vMaskingUVGainAmount  * (vMaskingTime.x / vMaskingTime.y);
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
    
    Out.vDiffuse = vMtrlDiffuse; 
    
    // 색깔 추가할 처리 (이미시브)
    if (vMtrlDiffuse.a >= g_fEmissiveCutAlpha)
        Out.vColorTarget = vector(g_vEmissive.rgb, g_fColorOption / 10.f);
    else
        Out.vColorTarget = vector(0.f, 0.f, 0.f, 0.f);
    
    
    return Out;
}

struct VS_BLUR_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    uint   iGPUIndex : TEXCOORD2;
};

VS_BLUR_OUT VS_BLUR(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_BLUR_OUT Out = (VS_BLUR_OUT) 0;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    
    float4 vProjPos = vector(In.vPosition, 1.f);
    
    Out.vPosition = mul(vProjPos, matWVP);

    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
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
        float2 UV = In.vTexcoord; // 이미지의 UV값
    
        int iTotalFrame = g_vUVCutting.x * g_vUVCutting.y; // 이미지의 최대 프레임 (몇 곱하기 몇인지)
    
        int iCurrentFrame = int(fAnimIndex.x); // 현재 프레임이 어디 위치인지
    
        int iFrameX = iCurrentFrame % (int) g_vUVCutting.x; // 현재 x축의 위치(현재 이미지의 몇번째 칸을 보여줄 지)
        int iFrameY = iCurrentFrame / (int) g_vUVCutting.x; // 현재 y축의 위치(현재 이미지의 몇번째 줄을 보여줄 지)
    
        float fFreamWidth = 1.0 / g_vUVCutting.x; // 1.0 나누기 이미지 갯수를 해서 한칸에 얼마나 갈지 정해준다.
        float fFreamHeight = 1.0 / g_vUVCutting.y; // 1.0 나누기 이미지 갯수를 해서 한줄에 얼마나 갈지 정해준다.
    
        UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth; // 먼저 uv를 0~1이 아닌 0~fFrameWidth로 만든 다음에 한칸씩 옆으로 밀어준다.
        UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight; // 먼저 uv를 0~1이 아닌 0~fFrameHeight로 만든 다음에 한줄씩 밑으로 내려준다.
    
        
        if (g_isDiffuseUVMove)
        {
            UV += (g_vDiffuseUVGainAmount / g_vUVCutting) * (vDiffuseTime.x / vDiffuseTime.y);
        }

        /* 노이즈 */
        if (g_isNoise == true)
        {
            float2 vNoiseUV = In.vTexcoord + (g_vDiffuseNoiseUVGainAmount) * (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y);
            
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
        
        if (g_isNoise == true)
        {
         
            float2 vNoiseUV = In.vTexcoord + (g_vMaskNoiseUVGainAmount) * (vNoiseUVMoveTime.x / vNoiseUVMoveTime.y);
            
            vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV);
            
            //노이즈 텍스쳐로 uv를 왜곡함
            vMaskTexcoord = vMaskTexcoord + (vMtrlNoise - 0.5f).rg * g_fNoiseDistortionIntensity;
        }
        
        if (g_isMaskUVMove)
        {
            if (vMaskingTime.y > 0)
                vMaskTexcoord += g_vMaskingUVGainAmount * (vMaskingTime.x / vMaskingTime.y);
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
    
    Out.vDiffuse = vMtrlDiffuse * g_fBlurIntensity;
    
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
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
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
