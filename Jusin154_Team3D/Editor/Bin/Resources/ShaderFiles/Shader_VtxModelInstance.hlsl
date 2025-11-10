#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_MaskingTexture;
Texture2D g_DissolveTexture;
Texture2D g_NoiseTexture;

float4 g_vCamPosition;

vector g_vColor;

float2 g_vUVGainAmount;
float2 g_vUVCutting;

float  g_fBlurIntensity; //블러 세기

float4 g_vEmissive;
float  g_fColorOption;
float  g_fEmissiveCutAlpha;

bool g_isDiffuse;
bool g_isMasking;
bool g_isDissolve;
bool g_isNoise;
bool g_isUVMove;

float g_fFar;

struct ParticleValue
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    
    float2 vMaskingUVMoveTime;
    float2 vDiffuseUVMoveTime;
    float2 vAniTime;
    
    float2 vAniIndex;
   
};

StructuredBuffer<ParticleValue> g_ParticleValue : register(t0);

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

VS_OUT VS_MAIN(VS_IN In, uint iGPUIndex : SV_VertexID)
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
    vector vMtrlDisolve;
    
    float fAnimIndex = g_ParticleValue[In.iGPUIndex].vAniIndex.x;
    float2 vDiffuseTime = g_ParticleValue[In.iGPUIndex].vDiffuseUVMoveTime;
    float2 vMaskingTime = g_ParticleValue[In.iGPUIndex].vMaskingUVMoveTime;
    
    
    float2 vTexcoord = In.vTexcoord;
    
    float2 vUVPerCell   = In.vTexcoord / g_vUVCutting; //이게 한 셀당 크기
    
    vTexcoord.x = vTexcoord.x * vUVPerCell.x + (fAnimIndex.x % g_vUVCutting.x) * vUVPerCell.x;
    vTexcoord.y = vTexcoord.y * vUVPerCell.y + (fAnimIndex.x / g_vUVCutting.y) * vUVPerCell.y;
    
    if (g_isUVMove)
    {
        vTexcoord += (g_vUVGainAmount / g_vUVCutting) * (vDiffuseTime.x / vDiffuseTime.y);
    }
    
    if (g_isDiffuse == true)
    {
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, vTexcoord);
        
        if (vMtrlDiffuse.a < 0.3f)
            discard;

    }
    else
    {
        vMtrlDiffuse = g_vColor;
    }
    
    if (g_isMasking == true)
    {
        vMtrlMask = g_MaskingTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    else
    {
        vMtrlMask.r = 1.f;
    }
    
    if (g_isDissolve == true)
    {
        vMtrlDisolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    else
    {
        vMtrlDisolve.r = 0.f;
    }
    
    if (g_isNoise == true)
    {
        vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    

    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlMask.r - vMtrlDisolve.r * (In.vLifeTime.x / In.vLifeTime.y));
        
    
    Out.vDiffuse = vMtrlDiffuse; 
    
    // 색깔 추가할 처리 (이미시브)
    if (vMtrlDiffuse.a >= g_fEmissiveCutAlpha )
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
};

VS_BLUR_OUT VS_BLUR(VS_IN In)
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


    
    return Out;
}

struct PS_BLUR_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;

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
    vector vMtrlDisolve;
    
    float2 vTexcoord = In.vTexcoord / g_vUVCutting;
    
    if (g_isUVMove)
    {
        vTexcoord += g_vUVGainAmount / g_vUVCutting  * (In.vLifeTime.x / In.vLifeTime.y);
    }
    
    if (g_isDiffuse == true)
    {
        vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, vTexcoord);
        
        if (vMtrlDiffuse.a < 0.3f)
            discard;

    }
    else
    {
        vMtrlDiffuse = g_vColor;
    }
    
    if (g_isMasking == true)
    {
        vMtrlMask = g_MaskingTexture.Sample(DefaultSampler, vTexcoord);
    }
    else
    {
        vMtrlMask.r = 1.f;
    }
    
    if (g_isDissolve == true)
    {
        vMtrlDisolve = g_DissolveTexture.Sample(DefaultSampler, vTexcoord);
    }
    else
    {
        vMtrlDisolve.r = 0.f;
    }
    
    if (g_isNoise == true)
    {
        vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vTexcoord);
    }
    

    vMtrlDiffuse.a = saturate(vMtrlDiffuse.a * vMtrlMask.r - vMtrlDisolve.r * (In.vLifeTime.x / In.vLifeTime.y));
        
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
