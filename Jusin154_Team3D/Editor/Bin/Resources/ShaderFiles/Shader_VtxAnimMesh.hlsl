
#include "Engine_Shader_Defines.hlsli" 

struct BoneOut
{
    row_major float4x4 Local;
    row_major float4x4 Combined;
    row_major float4x4 LocalCombined;
};

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_OffsetMatrix[256];

int g_bRimLight;
int g_bUseNormalMap;
int g_bDisolve;

float g_fFar;
float g_LifeRatio;
float g_fRimStrength;
float g_fRimPower;
float3 g_vRimColor;
float4 g_TestColor;
float4 g_vOutLineColor;
float g_fOutLineScale;
float g_fOutLineThickness;
float g_fOutLinePower;
float g_fDisolveRatio;
float g_fDisolveAmount;
float g_fDisolveEdgeWidth;

float4 g_vDisolveEdgeColor;
float4 g_vCamPosition;


float2 g_vSRVFlag;
float3 g_vPBR_Flag;

Texture2D g_DAOTexture : register(t0);
Texture2D g_THVTexture : register(t1);
Texture2D g_SurfaceParamsTexture : register(t2);
Texture2D g_DeadDisolveTexture : register(t3);
Texture2D g_DeadDisolveBurnTexture : register(t4);

Texture2D g_DiffuseTexture : register(t5);
Texture2D g_SpecularTexture : register(t6);
Texture2D g_AmbientTexture : register(t7);
Texture2D g_EmissiveTexture : register(t8);
Texture2D g_MossDiffuseTexture : register(t9);
Texture2D g_NormalTexture : register(t10);
Texture2D g_MossNormalTexture : register(t11);
Texture2D g_NormalBlendTexture : register(t12);
Texture2D g_SROBlendTexture : register(t13);
Texture2D g_MROBlendTexture : register(t14);
Texture2D g_ReflectionTexture : register(t15);
Texture2D g_DiffuseBlend : register(t16);
Texture2D g_MossSROTexture : register(t17);
Texture2D g_MossMROTexture : register(t18);
Texture2D g_MetalnessTexture : register(t19);
Texture2D g_Diffuse_RoughnessTexture : register(t20);
Texture2D g_AmbientOcclusionTexture : register(t21);
Texture2D g_UnknownTexture : register(t22);
Texture2D g_SheenTexture : register(t23);
Texture2D g_ClearcoadTexture : register(t24);
Texture2D g_TransmissionTexture : register(t25);
Texture2D g_Maya_BaseTexture : register(t26);
Texture2D g_Maya_SpecularTexture : register(t27);
Texture2D g_Maya_Specular_ColorTexture : register(t28);
Texture2D g_Maya_Specular_RoughnessTexture : register(t29);
Texture2D g_AnisotropyTexture : register(t30);


StructuredBuffer<BoneOut> g_BoneBuffer : register(t31);

int g_iBinded_Texture[27];

float3 g_RootColor;
float3 g_TipColor;
float3 g_DyeColor;

float g_DyeOpacity;
float g_HairRoughness;
float g_fUsingSurfaceParams;


matrix g_BoneMatrices[512];





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
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float4 w = In.vBlendWeight;
    float sumW = max(dot(w, 1.0f), 1e-6f);
    w /= sumW;

    matrix BoneMatrix =
        mul(g_OffsetMatrix[In.vBlendIndex.x],
            g_BoneBuffer[In.vBlendIndex.x].LocalCombined) * w.x
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            g_BoneBuffer[In.vBlendIndex.y].LocalCombined) * w.y
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            g_BoneBuffer[In.vBlendIndex.z].LocalCombined) * w.z
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            g_BoneBuffer[In.vBlendIndex.w].LocalCombined) * w.w;
    

    
    float4 skinnedPos = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float3 skinnedNormal = mul(float4(In.vNormal, 0.f), BoneMatrix).xyz;
    float3 skinnedTangent = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
    float3 skinnedBinormal = mul(float4(In.vBinormal, 0.f), BoneMatrix).xyz;

    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(skinnedPos, matWVP);
    Out.vWorldPos = mul(skinnedPos, g_WorldMatrix);

    Out.vNormal = normalize(mul(float4(skinnedNormal, 0.f), g_WorldMatrix).xyz);
    Out.vTangent = normalize(mul(float4(skinnedTangent, 0.f), g_WorldMatrix).xyz);
    Out.vBinormal = normalize(mul(float4(skinnedBinormal, 0.f), g_WorldMatrix).xyz);

    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}


struct VS_OUT_OUTLINE
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT_OUTLINE VS_MAIN_OUTLINE(VS_IN In)
{
    VS_OUT_OUTLINE Out;
    
    float4 w = In.vBlendWeight;
    float sumW = max(dot(w, 1.0f), 1e-6f);
    w /= sumW;

    matrix BoneMatrix =
        mul(g_OffsetMatrix[In.vBlendIndex.x],
            g_BoneBuffer[In.vBlendIndex.x].LocalCombined) * w.x
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            g_BoneBuffer[In.vBlendIndex.y].LocalCombined) * w.y
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            g_BoneBuffer[In.vBlendIndex.z].LocalCombined) * w.z
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            g_BoneBuffer[In.vBlendIndex.w].LocalCombined) * w.w;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    vector vBinormal = mul(vector(In.vBinormal, 0.f), BoneMatrix);
    vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
    vPosition.xyz += (vNormal.xyz * g_fOutLineThickness).xyz;


    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vBinormal, g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
}
struct VS_OUT_CAPTUREDMODEL
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_CAPTUREDMODEL VS_CAPTUREDMODEL(VS_IN In)
{
    VS_OUT_CAPTUREDMODEL Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        mul(g_BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x) +
        mul(g_BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y) +
        mul(g_BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z) +
        mul(g_BoneMatrices[In.vBlendIndex.w], fWeightW);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    return Out;
}

VS_OUT_BLUR VS_MAIN_BLUR(VS_IN In)
{
    VS_OUT_BLUR Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix =
        mul(g_BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x) +
        mul(g_BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y) +
        mul(g_BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z) +
        mul(g_BoneMatrices[In.vBlendIndex.w], fWeightW);
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    //float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    //matrix BoneMatrix = mul(g_BoneMatrices[In.vBlendIndex.x], In.vBlendWeight.x) +
    //                    mul(g_BoneMatrices[In.vBlendIndex.y], In.vBlendWeight.y) +
    //                    mul(g_BoneMatrices[In.vBlendIndex.z], In.vBlendWeight.z) +
    //                    mul(g_BoneMatrices[In.vBlendIndex.w], fWeightW);
    
    float4 w = In.vBlendWeight;
    float sumW = max(dot(w, 1.0f), 1e-6f);
    w /= sumW;

    matrix BoneMatrix =
        mul(g_OffsetMatrix[In.vBlendIndex.x],
            g_BoneBuffer[In.vBlendIndex.x].LocalCombined) * w.x
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            g_BoneBuffer[In.vBlendIndex.y].LocalCombined) * w.y
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            g_BoneBuffer[In.vBlendIndex.z].LocalCombined) * w.z
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            g_BoneBuffer[In.vBlendIndex.w].LocalCombined) * w.w;
    
    /* ?ㅽ궎??*/
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
   
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float4 vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    
    //if (vMtrlDiffuse.a < 0.3f){
    //    discard;
    //}
        if (true == g_bDisolve)
        {
            float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
            vMtrlDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vMtrlDiffuse, In.vTexcoord);
        }
    if (g_iBinded_Texture[21] != 0)
    {
        float4 vTransmission = g_TransmissionTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
        vMtrlDiffuse.a *= vTransmission.r;
    }
    if (g_iBinded_Texture[4] != 0)
    {
        float4 vEmissive = g_EmissiveTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
        vMtrlDiffuse += vEmissive;
    }
    if (vMtrlDiffuse.a < 0.2f) { discard; }
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    if (true == g_bRimLight)
    {
        float fRimLight = GetRimLight(g_vCamPosition.xyz, In.vWorldPos.xyz, vNormal, g_fRimPower, g_fRimStrength);
        vMtrlDiffuse.xyz = (vMtrlDiffuse.xyz * (1 - fRimLight)) + g_vRimColor * fRimLight;
    }
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    float fSurfaceParam = g_fUsingSurfaceParams;
    if (true == AlmostEqual7(g_fUsingSurfaceParams, 0.f))
    {
        fSurfaceParam = 0;
    }
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        fSurfaceParam, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    
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

struct PS_IN_OUTLINE
{
    float4 vPosition : SV_Position;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};
struct PS_OUT_OUTLINE
{
    float4 vOutLine : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT_OUTLINE PS_MAIN_OUTLINE(PS_IN_OUTLINE In)
{
    PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE) 0;
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float3 vToView = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);

    float fNdotV = saturate(dot(vNormal, vToView));
    float fRim = saturate((1.0f - fNdotV) * g_fOutLineScale);
    fRim = pow(fRim, g_fOutLinePower);

    Out.vOutLine = float4(g_vOutLineColor.rgb, fRim);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
    (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
    (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
    1.f);
    
    return Out;
}

struct PS_IN_CAPTUREDMODEL
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_CAPTUREDMODEL
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_CAPTUREDMODEL PS_CAPTUREDMODEL(PS_IN_CAPTUREDMODEL In)
{
    PS_OUT_CAPTUREDMODEL Out = (PS_OUT_CAPTUREDMODEL) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = saturate(1 - g_LifeRatio) * 0.5f;
    return Out;
}

struct PS_IN_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_BLUR
{
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT_BLUR PS_MAIN_BLUR(PS_IN_BLUR In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = saturate(vMtrlDiffuse);
    
    return Out;
}

PS_OUT PS_HAIR(PS_IN In)
{
    PS_OUT Out;

    float4 DAO = g_DAOTexture.Sample(DefaultSampler, In.vTexcoord);
    float alpha = DAO.a;
    if (alpha < 0.1f)
        discard;

    float4 THV = g_THVTexture.Sample(DefaultSampler, In.vTexcoord);
    float rootMask = THV.r;
    float tipMask = THV.g;
    float variation = THV.b;

    float3 baseColor = lerp(g_RootColor, g_TipColor, tipMask);

    baseColor *= lerp(0.8, 1.2, variation);

    baseColor = lerp(baseColor, g_DyeColor, g_DyeOpacity);

    float4 vNormalDesc = g_NormalTexture.Sample(MirrorSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 normal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);

    if (g_bRimLight)
    {
        float fRim = GetRimLight(g_vCamPosition.xyz, In.vWorldPos.xyz, normal, g_fRimPower, g_fRimStrength);
        baseColor = lerp(baseColor, g_vRimColor.rgb, fRim);
    }

    Out.vAlbedo = float4(baseColor, alpha);
    Out.vNormal = float4(normal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.f);

    return Out;
}

PS_OUT PS_EYELASH(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.4f)
    {
        discard;
    }
    
    vector vNormalDesc = g_NormalTexture.Sample(MirrorSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);
    
    if (true == g_bRimLight)
    {
        float fRimLight = GetRimLight(g_vCamPosition.xyz, In.vWorldPos.xyz, vNormal, g_fRimPower, g_fRimStrength);
        vMtrlDiffuse.xyz = (vMtrlDiffuse.xyz * (1 - fRimLight)) + g_vRimColor * fRimLight;
    }
    
    
    Out.vAlbedo = float4(vMtrlDiffuse.rgb, vMtrlDiffuse.g);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.f);
    
    
    return Out;
}

PS_OUT PS_SPECTOR_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float4 vMtrlDiffuse = g_EmissiveTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float4 vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vMtrlDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vMtrlDiffuse, In.vTexcoord);
    }
    if (vMtrlDiffuse.a < 0.2f)
    {
        discard;
    }
    if (vMtrlDiffuse.r > 0.3f)
    {
        vMtrlDiffuse.rgb *= float3(2.f, 1.f, 1.f);
    }
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    if (true == g_bRimLight)
    {
        float fRimLight = GetRimLight(g_vCamPosition.xyz, In.vWorldPos.xyz, vNormal, g_fRimPower, g_fRimStrength);
        vMtrlDiffuse.xyz = (vMtrlDiffuse.xyz * (1 - fRimLight)) + g_vRimColor * fRimLight;
    }
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    float fSurfaceParam = g_fUsingSurfaceParams;
    if (true == AlmostEqual7(g_fUsingSurfaceParams, 0.f))
    {
        fSurfaceParam = 0;
    }
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        fSurfaceParam, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ShadowPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_Less, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass CAPTUREDMODELPASS // 2
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTUREDMODEL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CAPTUREDMODEL();
    }
    pass BlurPass // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_BLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }

    pass HairPass // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HAIR();
    }

    pass EyeLashPass // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EYELASH();
    }

    pass OutLine_Write_Pass // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass OutLine_Read_Pass // 7
    {
        SetRasterizerState(RS_Front);
        SetDepthStencilState(DSS_Default_OutLine_SRead, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass SpectorPass // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPECTOR_MAIN();
    }
}
