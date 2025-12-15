
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fFar;
float g_fUsingSurfaceParams;
float g_fDiffuseMultiplier;
float g_fSurfaceMultiplier;
float g_fNormalMultiplier;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_SurfaceParamsTexture;
Texture2D g_DiffuseTextures[4];
Texture2D g_NormalTextures[4];
Texture2D g_SurfaceParamsTextures[4];
Texture2D g_MaskTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
        float4 vAlbedo : SV_TARGET0;
        float4 vNormal : SV_TARGET1;
        float4 vDepth : SV_TARGET2;
        float4 vColor : SV_Target3;
        float4 vSurface : SV_Target4;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    float2 tileUV = In.vWorldPos.xz / 16.f;
    
    vector vMask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);

    vector vMtrlDiffuse_0 = g_DiffuseTextures[0].Sample(DefaultSampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    vector vMtrlDiffuse_1 = g_DiffuseTextures[1].Sample(DefaultSampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    vector vMtrlDiffuse_2 = g_DiffuseTextures[2].Sample(DefaultSampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    vector vMtrlDiffuse_3 = g_DiffuseTextures[3].Sample(DefaultSampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    
    vector vMtrlDiffuse = float4(float3(vMtrlDiffuse_0.xyz * vMask.r +
                                vMtrlDiffuse_1.xyz * vMask.g +
                                vMtrlDiffuse_2.xyz * vMask.b + 
                                vMtrlDiffuse_3.xyz * vMask.a), 1.f);
    
    vector vSurface_0 = g_SurfaceParamsTextures[0].Sample(DefaultSampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    vector vSurface_1 = g_SurfaceParamsTextures[1].Sample(DefaultSampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    vector vSurface_2 = g_SurfaceParamsTextures[2].Sample(DefaultSampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    vector vSurface_3 = g_SurfaceParamsTextures[3].Sample(DefaultSampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    
    vector vSurface = (vSurface_0 * vMask.r +
                        vSurface_1 * vMask.g +
                        vSurface_2 * vMask.b + 
                        vSurface_3 * vMask.a);
    
    float3 vNormalDecoded_0 = DecodeNormalFromRG(g_NormalTextures[0], DefaultSampler, In.vWorldPos.xz * g_fNormalMultiplier);
    float3 vNormalDecoded_1 = DecodeNormalFromRG(g_NormalTextures[1], DefaultSampler, In.vWorldPos.xz * g_fNormalMultiplier);
    float3 vNormalDecoded_2 = DecodeNormalFromRG(g_NormalTextures[2], DefaultSampler, In.vWorldPos.xz * g_fNormalMultiplier);
    float3 vNormalDecoded_3 = DecodeNormalFromRG(g_NormalTextures[3], DefaultSampler, In.vWorldPos.xz * g_fNormalMultiplier);
    
    float3 vNormalDecoded = normalize(vNormalDecoded_0.xyz * vMask.r +
                                        vNormalDecoded_1.xyz * vMask.g + 
                                        vNormalDecoded_1.xyz * vMask.b +
                                        vNormalDecoded_1.xyz * vMask.a);
    
    float3 vBinormal = cross(In.vNormal.xyz, float3(1.f, 0.f, 0.f));
    
    float3 vTangent = cross(vBinormal, In.vNormal.xyz);
    
    float3x3 WorldMatrix = float3x3(vTangent, vBinormal, In.vNormal.xyz);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        g_fUsingSurfaceParams, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    
    return Out;
}

PS_OUT PS_MAIN_TERRAIN_ANISO(PS_IN In)
{
    PS_OUT Out;

    float2 tileUV = In.vWorldPos.xz / 16.f;
    
    vector vMask = g_MaskTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);

    vector vMtrlDiffuse_0 = g_DiffuseTextures[0].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    vector vMtrlDiffuse_1 = g_DiffuseTextures[1].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    vector vMtrlDiffuse_2 = g_DiffuseTextures[2].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    vector vMtrlDiffuse_3 = g_DiffuseTextures[3].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fDiffuseMultiplier);
    
    vector vMtrlDiffuse = float4(float3(vMtrlDiffuse_0.xyz * vMask.r +
                                vMtrlDiffuse_1.xyz * vMask.g +
                                vMtrlDiffuse_2.xyz * vMask.b + 
                                vMtrlDiffuse_3.xyz * vMask.a), 1.f);
    
    vector vSurface_0 = g_SurfaceParamsTextures[0].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    vector vSurface_1 = g_SurfaceParamsTextures[1].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    vector vSurface_2 = g_SurfaceParamsTextures[2].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    vector vSurface_3 = g_SurfaceParamsTextures[3].Sample(AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fSurfaceMultiplier);
    
    vector vSurface = (vSurface_0 * vMask.r +
                        vSurface_1 * vMask.g +
                        vSurface_2 * vMask.b + 
                        vSurface_3 * vMask.a);
    
    float3 vNormalDecoded_0 = DecodeNormalFromRG(g_NormalTextures[0], AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fNormalMultiplier);
    float3 vNormalDecoded_1 = DecodeNormalFromRG(g_NormalTextures[1], AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fNormalMultiplier);
    float3 vNormalDecoded_2 = DecodeNormalFromRG(g_NormalTextures[2], AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fNormalMultiplier);
    float3 vNormalDecoded_3 = DecodeNormalFromRG(g_NormalTextures[3], AnisoTropy_BLUR_Sampler, In.vWorldPos.xz * g_fNormalMultiplier);
    
    float3 vNormalDecoded = normalize(vNormalDecoded_0.xyz * vMask.r +
                                        vNormalDecoded_1.xyz * vMask.g + 
                                        vNormalDecoded_1.xyz * vMask.b +
                                        vNormalDecoded_1.xyz * vMask.a);
    
    float3 vBinormal = cross(In.vNormal.xyz, float3(1.f, 0.f, 0.f));
    
    float3 vTangent = cross(vBinormal, In.vNormal.xyz);
    
    float3x3 WorldMatrix = float3x3(vTangent, vBinormal, In.vNormal.xyz);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        g_fUsingSurfaceParams, // 서페이스 파라미터
        (1.f / 255.f)); // 지형은 1
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
};
VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

    return Out;
}
struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
};
struct PS_OUT_SHADOW
{
    float fShadowLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.fShadowLightDepth = In.vPosition.z;
    
    return Out;
}
technique11 NorTexTechnique11
{
    pass NorTexPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass TerrainPass // 1
    {
        SetRasterizerState(RS_Nocull_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Environment_Terrain_Pass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_Environment_SWrite, 1);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TERRAIN_ANISO();
    }
    pass ShadowPass // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}
