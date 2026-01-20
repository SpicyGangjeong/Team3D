#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;

Texture2D g_DiffuseTexture;
Texture2D g_GlassTexture;
Texture2D g_NormalTexture;
Texture2D g_SurfaceParamsTexture;
Texture2D g_EmissiveTexture;

float4 g_vCamPosition;
float g_fFar;
float g_fUsingSurfaceParams;
float g_fMBIntensity = 1.f;
float g_fGlassRatio = 1.f;
float g_fBloomStrength = 1.f;


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
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
    float4 vPrevProjPos : TEXCOORD4;
};

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matW, matWV, matWVP;
    matrix matPrevW, matPrevWV, matPrevWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = TransformMatrix;
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matPrevW = TransformMatrix;
    matPrevWV = mul(matW, g_ViewMatrix);
    matPrevWVP = mul(matWV, g_ProjMatrix);

    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matW));
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matW)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), matW)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), matW);
    Out.vProjPos = vPosition;
    Out.vPrevProjPos = mul(vector(In.vPosition, 1.f), matPrevWVP);
    
    return Out;
}

VS_OUT_SHADOW VS_SHADOW(VS_IN In, uint iGPUIndex : SV_InstanceID)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    matrix matW, matWV, matWVP;
    matrix matPrevW, matPrevWV, matPrevWVP;
    
    row_major matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matW = TransformMatrix;
    matWV = mul(matW, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    Out.vPosition = vPosition;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
    float4 vPrevProjPos : TEXCOORD4;
};

struct PS_OUT
{
    float4 vAlbedo : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vColor : SV_Target3;
    float4 vSurface : SV_Target4;
    float2 vVelocityUV : SV_Target5;
};

struct PS_OUT_SHADOW
{
    float fShadowLightDepth : SV_TARGET0;
};

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BLOOM
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.4f)
        discard;
    
    vector vMtrlNormal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal.xyz);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, g_fUsingSurfaceParams, 0.0f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(g_SurfaceParamsTexture.Sample(DefaultSampler, In.vTexcoord).xy, 1.f, 1.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

PS_OUT_SHADOW PS_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    clip(g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord).a - 0.4f);
    
    Out.fShadowLightDepth = In.vPosition.z;
    return Out;
}

PS_OUT PS_MAIN_LIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vGlassDiffuse = g_GlassTexture.Sample(DefaultSampler, In.vTexcoord);
    
    
    Out.vAlbedo = lerp(vGlassDiffuse, vMtrlDiffuse, g_fGlassRatio);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 15.f / 27.f, 1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(g_SurfaceParamsTexture.Sample(DefaultSampler, In.vTexcoord));
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

PS_OUT_BLOOM PS_BLOOM(PS_IN In)
{
    PS_OUT_BLOOM Out;
    
    vector vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = float4((vEmissive * g_fBloomStrength).xyz, 2.f / 255.f);
    
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
        
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Light
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT();
    }

    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass Shadow // 3
    {
        SetRasterizerState(RS_Shadow);
        SetDepthStencilState(DSS_ShadowWrite, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SHADOW();
        PixelShader = compile ps_5_0 PS_SHADOW();
    }

    pass NonCullModel // 4
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}




