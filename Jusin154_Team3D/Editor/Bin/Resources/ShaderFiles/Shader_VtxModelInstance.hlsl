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

float4 g_vEmissive;
float  g_fColorOption;

bool g_isDiffuse;
bool g_isMasking;
bool g_isDissolve;
bool g_isNoise;
bool g_isUVMove;

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
};

VS_OUT VS_MAIN(VS_IN In)
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
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vColor : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
   
    PS_OUT Out;
    vector vMtrlDiffuse;

   vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3x3 NormalMap_WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
    
    float3 vNormal = mul(vMtrlNormal.xyz * 2.f - 1.f, NormalMap_WorldMatrix);

    
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    
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
    
    float2 vTexcoord = In.vTexcoord;
    
    if (g_isUVMove)
    {
        vTexcoord += g_vUVGainAmount * (In.vLifeTime.x / In.vLifeTime.y);
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
        
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    
    Out.vDiffuse = vMtrlDiffuse;
    
    // 색깔 추가할 처리 
    Out.vColor = vector(g_vEmissive.rgb , g_fColorOption / 10.f);
    
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

}
