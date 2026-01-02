
#include "Engine_Shader_Defines.hlsli" 

struct BoneOut
{
    row_major float4x4 Combined;
    row_major float4x4 Local;
    row_major float4x4 LocalCombined;
};
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;
matrix g_OffsetMatrix[256];

int g_bRimLight;
int g_bUseNormalMap;
int g_bDisolve;
uint g_iPackedBlendColor;
int g_iColorMixerMethod;

float4 g_vDisolveEdgeColor;
float4 g_vCamPosition;
float4 g_vOutLineColor;
float g_fFar;
float g_fOutLineScale;
float g_fOutLineThickness;
float g_fOutLinePower;
float g_fUsingSurfaceParams;
float g_fEtherealRatio;
float g_fMixerFactor;
float g_fMBIntensity = 1.f;
float g_fDisolveEdgeWidth;
float g_fDisolveAmount;
float g_fDisolveRatio;


float g_TempWeight;



Texture2D g_DiffuseTexture : register(t00);
Texture2D g_SpecularTexture : register(t01);
Texture2D g_AmbientTexture : register(t02);
Texture2D g_EmissiveTexture : register(t03);
Texture2D g_HeightTexture : register(t04);
Texture2D g_NormalTexture : register(t05);
Texture2D g_ShininessTexture : register(t06);
Texture2D g_NormalBlendTexture : register(t07); // OPACITY
Texture2D g_SROBlendTexture : register(t08);
Texture2D g_LightMapTexture : register(t09);
Texture2D g_ReflectionTexture : register(t10);
Texture2D g_DiffuseBlend : register(t11); // BASE_COLOR
Texture2D g_NormalCameraTexture : register(t12);
Texture2D g_EmissionColorTexture : register(t13);
Texture2D g_MetalnessTexture : register(t14);
Texture2D g_Diffuse_RoughnessTexture : register(t15);
Texture2D g_AmbientOcclusionTexture : register(t16);
Texture2D g_UnknownTexture : register(t17);
Texture2D g_SheenTexture : register(t18);
Texture2D g_ClearcoatTexture : register(t19);
Texture2D g_TransmissionTexture : register(t20);
Texture2D g_Maya_BaseTexture : register(t21);
Texture2D g_Maya_SpecularTexture : register(t22);
Texture2D g_Maya_Specular_ColorTexture : register(t23);
Texture2D g_Maya_Specular_RoughnessTexture : register(t24);
Texture2D g_AnisotropyTexture : register(t25);

StructuredBuffer<BoneOut> g_BoneBuffer : register(t26);
StructuredBuffer<BoneOut> g_PrevBoneBuffer : register(t27);




Texture2D g_SurfaceParamsTexture;
Texture2D g_DeadDisolveTexture;
Texture2D g_DeadDisolveBurnTexture;

int g_iBinded_Texture[27];
matrix g_BoneMatrices[256];
matrix g_PrevBoneMatrices[256];
int g_RobeBoneMask[256];

struct VS_IN_MESH
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

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
VS_OUT VS_MAIN_MESH(VS_IN_MESH In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matPrevWV, matPrevWVP;
    matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
    matPrevWVP = mul(matPrevWV, g_PrevProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vPrevProjPos = mul(vector(In.vPosition, 1.f), matPrevWVP);
    return Out;
}

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

VS_OUT VS_MAIN_LEGACY(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4 w = In.vBlendWeight;
    float sumW = max(dot(w, 1.0f), 1e-6f);
    w /= sumW;

    uint4 idx = In.vBlendIndex;
    
    matrix BoneMatrix = (matrix) 0;
    matrix TempBoneMatrix = (matrix) 0;
    
    int MinIdx = 30;
    int MaxIdx = 45;
    
    if (idx.x >= MinIdx && idx.x <= MaxIdx)
        //g_RobeBoneMask[idx.x] == 1)
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.x],
            mul(g_BoneMatrices[In.vBlendIndex.x], w.x));
    }
    else
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.x],
            mul(g_BoneBuffer[In.vBlendIndex.x].LocalCombined, w.x));
    }
    if (idx.y >= MinIdx && idx.y <= MaxIdx)
    //if (g_RobeBoneMask[idx.y]  == 1)
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.y],
            mul(g_BoneMatrices[In.vBlendIndex.y], w.y));
    }
    else
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.y],
            mul(g_BoneBuffer[In.vBlendIndex.y].LocalCombined, w.y));
    }
    if (idx.z >= MinIdx && idx.z <= MaxIdx)
    //if (g_RobeBoneMask[idx.z] == 1)
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.z],
            mul(g_BoneMatrices[In.vBlendIndex.z], w.z));
    }
    else
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.z],
            mul(g_BoneBuffer[In.vBlendIndex.z].LocalCombined, w.z));
    }
    if (idx.w >= MinIdx && idx.w <= MaxIdx)
    //if (g_RobeBoneMask[idx.w] == 1)
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.w],
            mul(g_BoneMatrices[In.vBlendIndex.w], w.w));
    }
    else
    {
        TempBoneMatrix += mul(g_OffsetMatrix[In.vBlendIndex.w],
            mul(g_BoneBuffer[In.vBlendIndex.w].LocalCombined, w.w));
    }
    
    BoneMatrix = TempBoneMatrix;
    
    //matrix BoneMatrix = (matrix) 0;
    //{
    //    uint iIndex = idx.x;
    //    float fWeight = w.x;
    //    if (isRobe)
    //    {
    //        if (29 == iIndex)
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));

    //        }
    //        else
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneMatrices[iIndex]));
    //        }
    //    }
    //    else
    //    {
    //        BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));
    //    }
    //}
    //{
    //    uint iIndex = idx.y;
    //    float fWeight = w.y;
    //    if (isRobe)
    //    {
    //        if (29 == iIndex)
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));

    //        }
    //        else
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneMatrices[iIndex]));
    //        }
    //    }
    //    else
    //    {
    //        BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));
    //    }
    //}
    //{
    //    uint iIndex = idx.z;
    //    float fWeight = w.z;
    //    if (isRobe)
    //    {
    //        if (29 == iIndex)
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));

    //        }
    //        else
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneMatrices[iIndex]));
    //        }
    //    }
    //    else
    //    {
    //        BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));
    //    }
    //}
    //{
    //    uint iIndex = idx.w;
    //    float fWeight = w.w;
    //    if (isRobe)
    //    {
    //        if (29 == iIndex)
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));

    //        }
    //        else
    //        {
    //            BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneMatrices[iIndex]));
    //        }
    //    }
    //    else
    //    {
    //        BoneMatrix += mul(fWeight, mul(g_OffsetMatrix[iIndex], g_BoneBuffer[iIndex].LocalCombined));
    //    }
    //}

    //float robeWeight =
    //      g_RobeBoneMask[idx.x] * w.x
    //    + g_RobeBoneMask[idx.y] * w.y
    //    + g_RobeBoneMask[idx.z] * w.z
    //    + g_RobeBoneMask[idx.w] * w.w;

    //bool isRobe = robeWeight > g_TempWeight;

    //matrix BoneMatrix = (matrix) 0;

    //if (isRobe)
    //{
    //    BoneMatrix += w.x * mul(g_OffsetMatrix[idx.x], g_BoneMatrices[idx.x]);
    //    BoneMatrix += w.y * mul(g_OffsetMatrix[idx.y], g_BoneMatrices[idx.y]);
    //    BoneMatrix += w.z * mul(g_OffsetMatrix[idx.z], g_BoneMatrices[idx.z]);
    //    BoneMatrix += w.w * mul(g_OffsetMatrix[idx.w], g_BoneMatrices[idx.w]);
    //}
    //else
    //{
    //    BoneMatrix += w.x * mul(g_OffsetMatrix[idx.x], g_BoneBuffer[idx.x].LocalCombined);
    //    BoneMatrix += w.y * mul(g_OffsetMatrix[idx.y], g_BoneBuffer[idx.y].LocalCombined);
    //    BoneMatrix += w.z * mul(g_OffsetMatrix[idx.z], g_BoneBuffer[idx.z].LocalCombined);
    //    BoneMatrix += w.w * mul(g_OffsetMatrix[idx.w], g_BoneBuffer[idx.w].LocalCombined);
    //}


    matrix PrevBoneMatrix =
        mul(g_OffsetMatrix[In.vBlendIndex.x],
            mul(g_PrevBoneMatrices[In.vBlendIndex.x], w.x))
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            mul(g_PrevBoneMatrices[In.vBlendIndex.y], w.y))
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            mul(g_PrevBoneMatrices[In.vBlendIndex.z], w.z))
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            mul(g_PrevBoneMatrices[In.vBlendIndex.w], w.w));
    
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

VS_OUT VS_MAIN_OUTLINE_READ(VS_IN In)
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
    vector vPrevNormal = mul(vector(In.vNormal, 0.f), PrevBoneMatrix);
    vector vBinormal = mul(vector(In.vBinormal, 0.f), BoneMatrix);
    vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
    vPosition.xyz += (vNormal.xyz * g_fOutLineThickness).xyz;
    vPrevPosition.xyz += (vPrevNormal.xyz * g_fOutLineThickness).xyz;


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
struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
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
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
   
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

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
    float4 vPrevProjPos : TEXCOORD3;
};

struct PS_OUT
{
    float4 vAlbedo : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vColor : SV_Target3;
    float4 vSurface : SV_Target4;
    float2 vVelocityUV : SV_TARGET5;
};

struct PS_OUT_BLEND
{
    float4 vAlbedo : SV_TARGET0;
    //float4 vNormal : SV_TARGET1;
    //float4 vDepth : SV_TARGET2;
    //float2 vVelocityUV : SV_TARGET3;
};
struct PS_OUT_OUTLINE
{
    float4 vAlbedo : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vColor : SV_Target3;
    float4 vSurface : SV_Target4;
    float2 vVelocityUV : SV_TARGET5;
};

PS_OUT_OUTLINE PS_MAIN_OUTLINE_READ(PS_IN In)
{
    PS_OUT_OUTLINE Out = (PS_OUT_OUTLINE) 0;
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float3 vToView = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);

    float fNdotV = saturate(dot(vNormal, vToView));
    float fRim = saturate((1.0f - fNdotV) * g_fOutLineScale);
    fRim = pow(fRim, g_fOutLinePower);

    Out.vAlbedo = float4(g_vOutLineColor.rgb * 5.f, fRim);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    Out.vSurface = float4(0.5f, 1.f, 1.f, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

PS_OUT PS_EYELASH_DAOTHV_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDAO_Mask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDAO_Mask = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDAO_Mask, In.vTexcoord);
    }
    float fDiffuseMask = vDAO_Mask.r;
    float AlphaMask = vDAO_Mask.g;
    float AoMask_Dao = vDAO_Mask.b;
    
    clip(AlphaMask - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vTHV_Mask = g_SROBlendTexture.Sample(DefaultSampler, In.vTexcoord);
    float RootTip = vTHV_Mask.r;
    float SpecMask = vTHV_Mask.g;
    float AoMask_Thv = vTHV_Mask.b;
    
    float3 vColorRoot = vDAO_Mask.rgb.rgb * 0.6f;
    float3 vColorTip = vDAO_Mask.rgb;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    
    Out.vAlbedo = float4(vBaseColor, AlphaMask);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}

PS_OUT PS_TEETH_SRXO_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }

    clip(vDiffuse.a - 0.2f);
    
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRXO_MASK = g_AnisotropyTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRXO_MASK.r, vSRXO_MASK.g, vSRXO_MASK.a, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_EYE_DN_SRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(0.5f, 0.5f, 1.f, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_EYE_OCC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    discard;
    return Out;
}

PS_OUT PS_FACIAL_HAIR_DAOTHV_ToSRO(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDAO_Mask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDAO_Mask = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDAO_Mask, In.vTexcoord);
    }
    float fDiffuseMask = vDAO_Mask.r;
    float AlphaMask = vDAO_Mask.g;
    float AoMask_Dao = vDAO_Mask.b;

    clip(AlphaMask - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vTHV_Mask = g_SROBlendTexture.Sample(DefaultSampler, In.vTexcoord);
    float RootTip = vTHV_Mask.r;
    float SpecMask = vTHV_Mask.g;
    float AoMask_Thv = vTHV_Mask.b;
    
    float3 vColorRoot = vDAO_Mask.rgb * 0.6f;
    float3 vColorTip = vDAO_Mask.rgb;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    
    Out.vAlbedo = float4(vBaseColor, AlphaMask);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}

PS_OUT PS_HEAD_HAIR_DAOTHV_ToSRO(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDAO_Mask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDAO_Mask = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDAO_Mask, In.vTexcoord);
    }
    float fDiffuseMask = vDAO_Mask.r;
    float AlphaMask = vDAO_Mask.g;
    float AoMask_Dao = vDAO_Mask.b;

    clip(AlphaMask - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vTHV_Mask = g_SROBlendTexture.Sample(DefaultSampler, In.vTexcoord);
    float RootTip = vTHV_Mask.r;
    float SpecMask = vTHV_Mask.g;
    float AoMask_Thv = vTHV_Mask.b;
    
    float3 vColorRoot = vDAO_Mask.rgb * 0.6f;
    float3 vColorTip = vDAO_Mask.rgb;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    
    Out.vAlbedo = float4(vBaseColor, AlphaMask);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}

PS_OUT PS_HEADwtHAND_DSRXON_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRXO_Mask = g_AnisotropyTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRXO_Mask.r, vSRXO_Mask.g, vSRXO_Mask.a, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_LOWER_DSRON_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRO_Mask = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRO_Mask.r, vSRO_Mask.g, vSRO_Mask.b, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_UPPER_DMRON_ToMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // MRO
    return Out;
}

PS_OUT PS_GLASSES_DMRON_ToMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_EmissiveMetalness_DENMRO_ToMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    vDiffuse.rgb += vEmissive.rgb * 3.f;

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_DNMRO_ToMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    vDiffuse.rgb += vEmissive.rgb * 3.f;

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_MI_ClothSim_DSEN_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    vDiffuse.rgb += vEmissive.rgb;

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSpecular_Mask = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
       (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSpecular_Mask.r, 0.5f, 1.f, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_MI_DANSROMRO_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    float4 vAlpha = g_TransmissionTexture.Sample(DefaultSampler, In.vTexcoord);

    clip(vAlpha.r - 0.2f);
    
    vDiffuse.a = vAlpha.r;
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRO_Mask = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vMRO_MASK = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float Roughness = saturate(vSRO_Mask.g * vMRO_MASK.g);

    float Occlusion = saturate(vSRO_Mask.b * vMRO_MASK.b);
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRO_Mask.r, Roughness, Occlusion, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_Troll_Club_DAENMROSRXO_ToMROX(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vAmbient = g_AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vDiffuse.rgb += vEmissive.rgb;
    vDiffuse.rgb += vAmbient.rgb;

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRXO_Mask = g_AnisotropyTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vMRO_MASK = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float Roughness = vSRXO_Mask.g;

    float Occlusion = saturate(vSRXO_Mask.a * vMRO_MASK.b);
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_MASK.r, Roughness, Occlusion, vSRXO_Mask.b);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}
PS_OUT PS_Player_EyeLash_DAOTHV_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float2 center = float2(0.5f, 0.5f);
    float2 p = uv - center;
    float2 uvRot90 = frac(float2(p.x, p.y) + center);
    float4 vDAOColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDAOColor = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDAOColor, In.vTexcoord);
    }
    float4 vTHVColor = g_DiffuseBlend.Sample(DefaultSampler, uv);
    float4 vNormalColor = g_NormalTexture.Sample(DefaultSampler, uv);
    float fAlpha = g_NormalBlendTexture.Sample(DefaultSampler, uvRot90).r;
    float3 vHairBaseColor = g_NormalBlendTexture.Sample(DefaultSampler, uvRot90).rrr;
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float AoMask_Dao = vDAOColor.b;
    float RootTip = vTHVColor.r;
    float SpecMask = vTHVColor.g;
    float AoMask_Thv = vTHVColor.b;
    float3 vColorHair = clamp(ColorMixer(vHairBaseColor, UnpackRGB8(g_iPackedBlendColor), g_fMixerFactor, g_iColorMixerMethod), 0.f, 1.f);
    float3 vColorRoot = vColorHair * 0.6f;
    float3 vColorTip = vColorHair;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    clip(fAlpha - 0.2f);
    
    Out.vAlbedo = float4(vBaseColor, fAlpha);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.1f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}
PS_OUT PS_Player_Eye_ToMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float4 vDiffuseColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuseColor = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuseColor, In.vTexcoord);
    }
    float4 vSubSurfaceColor = g_ReflectionTexture.Sample(DefaultSampler, uv);
    float4 vNormalColor = g_NormalTexture.Sample(DefaultSampler, uv);
    float4 vMROColor = g_MetalnessTexture.Sample(DefaultSampler, uv);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vDiffuseColor;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMROColor.r, vMROColor.g, vMROColor.b, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}
PS_OUT PS_Player_Robe_ToMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float4 vDiffuseColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuseColor = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuseColor, In.vTexcoord);
    }
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, uv);
    vDiffuseColor.rgb += vEmissive.r * vEmissive.g;
    
    float4 vNormalColor = g_NormalTexture.Sample(DefaultSampler, uv);
    float4 vMROColor = g_MetalnessTexture.Sample(DefaultSampler, uv);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vDiffuseColor;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMROColor.r, vMROColor.g, vMROColor.b, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}
PS_OUT PS_Player_Suit_DSRON_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float4 vDiffuseColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuseColor = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuseColor, In.vTexcoord);
    }
    float4 vSROColor = g_SpecularTexture.Sample(DefaultSampler, uv);
    float4 vNormalColor = g_NormalTexture.Sample(DefaultSampler, uv);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    vDiffuseColor.rgb = clamp(ColorMixer(vDiffuseColor.rrr, UnpackRGB8(g_iPackedBlendColor), g_fMixerFactor, g_iColorMixerMethod), 0.f, 1.f);
    Out.vAlbedo = vDiffuseColor;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSROColor.r, vSROColor.g, vSROColor.b, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}
PS_OUT PS_Player_HairDAOTHV_ToSRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float4 vDAOColor = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDAOColor = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDAOColor, In.vTexcoord);
    }
    float3 vHairBaseColor = g_NormalBlendTexture.Sample(DefaultSampler, uv).rrr;
    float4 vNormalColor = g_NormalTexture.Sample(DefaultSampler, uv);
    float4 vTHVColor = g_DiffuseBlend.Sample(DefaultSampler, uv);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float AlphaMask = vDAOColor.g;
    float AoMask_Dao = vDAOColor.b;
    float RootTip = vTHVColor.r;
    float SpecMask = vTHVColor.g;
    float AoMask_Thv = vTHVColor.b; // Volume
    
    float3 vColorHair = clamp(ColorMixer(vHairBaseColor, UnpackRGB8(g_iPackedBlendColor), g_fMixerFactor, g_iColorMixerMethod), 0.f, 1.f);
    float3 vColorRoot = vColorHair * 0.6f;
    float3 vColorTip = vColorHair;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    float fAlphaTip = AlphaMask * 0.6f;
    float fAlphaRoot = AlphaMask;
    float fBaseAlpha = lerp(fAlphaRoot, fAlphaTip, (1.f - RootTip));
    float fAlpha = (fBaseAlpha);

    clip(fAlpha - 0.2f);
    
    Out.vAlbedo = float4(vBaseColor, fAlpha);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w),
        (In.vProjPos.w / g_fFar),
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX,
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    return Out;
}

PS_OUT_BLEND PS_Dragon_Aura(PS_IN In)
{
    PS_OUT_BLEND Out;
    float2 uv = In.vTexcoord;
    float2 uvOppacity = uv;
    uvOppacity.y += g_fEtherealRatio;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    float fOppacity = g_NormalBlendTexture.Sample(DefaultSampler, uvOppacity).r;
    vDiffuse = vDiffuse * (1 - fOppacity);
    Out.vAlbedo = vDiffuse;
    return Out;
}

PS_OUT_BLEND PS_Dragon_EtherealEyes(PS_IN In)
{
    PS_OUT_BLEND Out;
    float2 uv = In.vTexcoord;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    float fOppacity = g_NormalBlendTexture.Sample(DefaultSampler, uv).a;
    float4 vBaseColor = g_DiffuseBlend.Sample(DefaultSampler, uv);
    //TODO ?덉븣 ???댁빞??
    Out.vAlbedo = lerp(vDiffuse, vBaseColor, fOppacity);
    return Out;
}

PS_OUT_BLEND PS_Dragon_EtherealWings(PS_IN In)
{
    PS_OUT_BLEND Out;
    float2 uv = In.vTexcoord;
    float2 uvDiffuse = uv;
    uvDiffuse.y -= g_fEtherealRatio;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uvDiffuse);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, uv);
    vDiffuse += vEmissive;
    float fOppacity = g_NormalBlendTexture.Sample(DefaultSampler, uvDiffuse).a;
    vDiffuse.a = (1 - fOppacity);
    Out.vAlbedo = vDiffuse;
    return Out;
}

PS_OUT PS_Dragon_Body(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    float fOppacity = g_NormalBlendTexture.Sample(DefaultSampler, uv).r;
    vDiffuse.a = fOppacity;

    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float3 vMRO = g_MetalnessTexture.Sample(DefaultSampler, uv).rgb;
    float4 vMask = g_UnknownTexture.Sample(DefaultSampler, uv);
    
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT PS_Dragon_Wings(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 uv = In.vTexcoord;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DeadDisolveBurnTexture.Sample(DefaultSampler, In.vTexcoord);
        vDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vDiffuse, In.vTexcoord);
    }
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, uv);
    vDiffuse += vEmissive;


    clip(vDiffuse.a - 0.2f);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float3 vMRO = g_MetalnessTexture.Sample(DefaultSampler, uv).rgb;
    float4 vMask = g_UnknownTexture.Sample(DefaultSampler, uv);
    
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    // SRO
    return Out;
}

PS_OUT_BLEND PS_Dragon_RedHot(PS_IN In)
{
    PS_OUT_BLEND Out;
    float2 uv = In.vTexcoord;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    Out.vAlbedo = vDiffuse;
    return Out;
}

PS_OUT_BLEND PS_Dragon_PinkHot(PS_IN In)
{
    PS_OUT_BLEND Out;
    float2 uv = In.vTexcoord;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    Out.vAlbedo = vDiffuse;
    return Out;
}

PS_OUT_BLEND PS_Dragon_YellowHot(PS_IN In)
{
    PS_OUT_BLEND Out;
    float2 uv = In.vTexcoord;
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
    Out.vAlbedo = vDiffuse;
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float fShadowLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.fShadowLightDepth = In.vProjPos.z;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass EYELASH_DAOTHV_ToSRO // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EYELASH_DAOTHV_ToSRO();
    }
    pass TEETH_SRXO_ToSRO // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEETH_SRXO_ToSRO();
    }
    pass EYE_DN_ToSRO // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EYE_DN_SRO();
    }
    pass EYE_OCC // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EYE_OCC();
    }
    pass FACIAL_HAIR_DAOTHV_ToSRO // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FACIAL_HAIR_DAOTHV_ToSRO();
    }
    pass HEAD_HAIR_DAOTHV_ToSRO // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HEAD_HAIR_DAOTHV_ToSRO();
    }
    pass HEADwtHAND_DSRXON_ToSRO // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HEADwtHAND_DSRXON_ToSRO();
    }
    pass LOWER_DSRON_ToSRO // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LOWER_DSRON_ToSRO();
    }
    pass UPPER_DMRON_ToMRO // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UPPER_DMRON_ToMRO();
    }
    pass GLASSES_DMRON_ToMRO // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASSES_DMRON_ToMRO();
    }
    pass EmissiveMetalness_DENMRO_ToMRO // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EmissiveMetalness_DENMRO_ToMRO();
    }
    pass MI_ClothSim_DSEN_ToSRO // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MI_ClothSim_DSEN_ToSRO();
    }
    pass MI_Troll_Club_DAENMROSRXO_To // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_MESH();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Troll_Club_DAENMROSRXO_ToMROX();
    }
/////
    pass TEETH_SRXO_ToSRO__OUTLINE_READ // 13
    {
        SetRasterizerState(RS_Front);
        SetDepthStencilState(DSS_Default_OutLine_SRead, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE_READ();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE_READ();
    }
    pass MI_DANSROMRO_ToSRO // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MI_DANSROMRO_ToSRO();
    }
//// DRAGON
    pass DragonAuraPass // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_Aura();
    }
    pass DragonEtherealEyesPass // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_EtherealEyes();
    }
    pass DragonEtherealWingsPass // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_EtherealWings();
    }
    pass DragonWingsPass // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_Wings();
    }
    pass DragonBodyPass // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_Body();
    }
    pass DragonRedHotPass // 20
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_RedHot();
    }
    pass DragonPinkHotPass // 21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_PinkHot();
    }
    pass DragonYellowHotPass // 22
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dragon_YellowHot();
    }
//// PLAYER_
    pass PLAYER_HAIR_DAOTHV_ToSRO // 23
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Player_HairDAOTHV_ToSRO();
    }
    pass PLAYER_Suit_DSRON_ToSRO // 24
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Player_Suit_DSRON_ToSRO();
    }
    pass PLAYER_EyeLash_DAOTHV_ToSRO // 25
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Player_EyeLash_DAOTHV_ToSRO();
    }
    pass PLAYER_Eye_ToSRO // 26
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Player_Eye_ToMRO();
    }
    pass PLAYER_Robe_ToMRO // 27
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_LEGACY();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Player_Robe_ToMRO();
    }
}
