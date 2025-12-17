
#include "Engine_Shader_Defines.hlsli" 

struct BoneOut
{
    row_major float4x4 Local;
    row_major float4x4 Combined;
    row_major float4x4 LocalCombined;

};
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;
matrix g_OffsetMatrix[256];


int g_bRimLight;
int g_bUseNormalMap;
int g_bDisolve;

float4 g_vDisolveEdgeColor;
float4 g_vCamPosition;
float4 g_vOutLineColor;
float g_fFar;
float g_fOutLineScale;
float g_fOutLineThickness;
float g_fOutLinePower;
float g_fUsingSurfaceParams;


Texture2D g_DiffuseTexture;
Texture2D g_SpecularTexture;
Texture2D g_AmbientTexture;
Texture2D g_EmissiveTexture;
Texture2D g_HeightTexture;
Texture2D g_NormalTexture;
Texture2D g_ShininessTexture;
Texture2D g_OpacityTexture;
Texture2D g_DisplacementTexture;
Texture2D g_LightMapTexture;
Texture2D g_ReflectionTexture;
Texture2D g_BaseColorTexture;
Texture2D g_NormalCameraTexture;
Texture2D g_EmissionColorTexture;
Texture2D g_MetalnessTexture;
Texture2D g_Diffuse_RoughnessTexture;
Texture2D g_AmbientOcclusionTexture;
Texture2D g_UnknownTexture;
Texture2D g_SheenTexture;
Texture2D g_ClearcoadTexture;
Texture2D g_TransmissionTexture;
Texture2D g_Maya_BaseTexture;
Texture2D g_Maya_SpecularTexture;
Texture2D g_Maya_Specular_ColorTexture;
Texture2D g_Maya_Specular_RoughnessTexture;
Texture2D g_AnisotropyTexture;

StructuredBuffer<BoneOut> g_BoneBuffer : register(t26);

Texture2D g_SurfaceParamsTexture;


int g_iBinded_Texture[27];
matrix g_BoneMatrices[512];

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
    VS_OUT Out;
    
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
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
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
    Out.vPrevProjPos = mul(vector(In.vPosition, 1.f), matPrevWVP);
    return Out;
}

VS_OUT VS_MAIN_OUTLINE_READ(VS_IN In)
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
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    vector vBinormal = mul(vector(In.vBinormal, 0.f), BoneMatrix);
    vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
    vPosition.xyz += (vNormal.xyz * g_fOutLineThickness).xyz;


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
    Out.vPrevProjPos = mul(vPosition, matPrevWVP);

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
            g_BoneBuffer[In.vBlendIndex.x].LocalCombined) * w.x
      + mul(g_OffsetMatrix[In.vBlendIndex.y],
            g_BoneBuffer[In.vBlendIndex.y].LocalCombined) * w.y
      + mul(g_OffsetMatrix[In.vBlendIndex.z],
            g_BoneBuffer[In.vBlendIndex.z].LocalCombined) * w.z
      + mul(g_OffsetMatrix[In.vBlendIndex.w],
            g_BoneBuffer[In.vBlendIndex.w].LocalCombined) * w.w;
    
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
struct PS_OUT_OUTLINE
{
    float4 vOutLine : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float2 vVelocityUV : SV_TARGET3;
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

    Out.vOutLine = float4(g_vOutLineColor.rgb, fRim);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos);

    return Out;
}

PS_OUT PS_EYELASH_DAOTHV_ToSRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDAO_Mask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fDiffuseMask = vDAO_Mask.r;
    float AlphaMask = vDAO_Mask.g;
    float AoMask_Dao = vDAO_Mask.b;
    
    if (AlphaMask < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vTHV_Mask = g_DisplacementTexture.Sample(DefaultSampler, In.vTexcoord);
    float RootTip = vTHV_Mask.r;
    float SpecMask = vTHV_Mask.g;
    float AoMask_Thv = vTHV_Mask.b;
    
    float3 vColorRoot = vDAO_Mask * 0.6f;
    float3 vColorTip = vDAO_Mask;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    
    Out.vAlbedo = float4(fDiffuseMask, fDiffuseMask, fDiffuseMask, AlphaMask);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_TEETH_SRXO_ToSRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRXO_MASK = g_AnisotropyTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRXO_MASK.r, vSRXO_MASK.g, vSRXO_MASK.a, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_EYE_DN_SRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(0.5f, 0.5f, 1.f, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_EYE_OCC(PS_IN In)
{
    PS_OUT Out;
    discard;
    return Out;
}

PS_OUT PS_FACIAL_HAIR_DAOTHV_ToSRO(PS_IN In)
{

    PS_OUT Out;
    
    float4 vDAO_Mask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fDiffuseMask = vDAO_Mask.r;
    float AlphaMask = vDAO_Mask.g;
    float AoMask_Dao = vDAO_Mask.b;
    
    if (AlphaMask < 0.2f)
    {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vTHV_Mask = g_DisplacementTexture.Sample(DefaultSampler, In.vTexcoord);
    float RootTip = vTHV_Mask.r;
    float SpecMask = vTHV_Mask.g;
    float AoMask_Thv = vTHV_Mask.b;
    
    float3 vColorRoot = vDAO_Mask * 0.6f;
    float3 vColorTip = vDAO_Mask;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    
    Out.vAlbedo = float4(fDiffuseMask, fDiffuseMask, fDiffuseMask, AlphaMask);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_HEAD_HAIR_DAOTHV_ToSRO(PS_IN In)
{

    PS_OUT Out;
    
    float4 vDAO_Mask = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fDiffuseMask = vDAO_Mask.r;
    float AlphaMask = vDAO_Mask.g;
    float AoMask_Dao = vDAO_Mask.b;
    
    if (AlphaMask < 0.2f)
    {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vTHV_Mask = g_DisplacementTexture.Sample(DefaultSampler, In.vTexcoord);
    float RootTip = vTHV_Mask.r;
    float SpecMask = vTHV_Mask.g;
    float AoMask_Thv = vTHV_Mask.b;
    
    float3 vColorRoot = vDAO_Mask * 0.6f;
    float3 vColorTip = vDAO_Mask;
    float3 vBaseColor = lerp(vColorRoot, vColorTip, RootTip);
    
    
    Out.vAlbedo = float4(fDiffuseMask, fDiffuseMask, fDiffuseMask, AlphaMask);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(lerp(0.7f, 0.3f, SpecMask), SpecMask * 0.5f, AoMask_Dao * AoMask_Thv, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_HEADwtHAND_DSRXON_ToSRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRXO_Mask = g_AnisotropyTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRXO_Mask.r, vSRXO_Mask.g, vSRXO_Mask.a, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_LOWER_DSRON_ToSRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRO_Mask = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSRO_Mask.r, vSRO_Mask.g, vSRO_Mask.b, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_UPPER_DMRON_ToMRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    // MRO
    return Out;
}

PS_OUT PS_GLASSES_DMRON_ToMRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_EmissiveMetalness_DENMRO_ToMRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    vDiffuse.rgb += vEmissive.rgb;
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vMRO_Mask = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_Mask.r, vMRO_Mask.g, vMRO_Mask.b, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_MI_ClothSim_DSEN_ToSRO(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    vDiffuse.rgb += vEmissive.rgb;
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSpecular_Mask = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);

    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
       (float) AI_TEXTURE_TYPE_SPECULAR / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vSpecular_Mask.r, 0.5f, 1.f, 0.f);
    // SRO
    return Out;
}

PS_OUT PS_Troll_Club_DAENMROSRXO_ToMROX(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vAmbient = g_AmbientTexture.Sample(DefaultSampler, In.vTexcoord);
    vDiffuse.rgb += vEmissive.rgb;
    vDiffuse.rgb += vAmbient.rgb;
    if (vDiffuse.a < 0.2f) {
        discard;
    }
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    float4 vSRXO_Mask = g_AnisotropyTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vMRO_MASK = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float Roughness = vSRXO_Mask.g;

    float Occlusion = saturate(vSRXO_Mask.a * vMRO_MASK.b);
    
    Out.vAlbedo = vDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = float4(vMRO_MASK.r, Roughness, Occlusion, vSRXO_Mask.b);
    // SRO
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
        SetDepthStencilState(DSS_Default, 0);
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
    pass TEETH_SRXO_ToSRO__OUTLINE_WRITE // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEETH_SRXO_ToSRO();
    }
    pass EYE_DN_ToSRO__OUTLINE_WRITE // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EYE_DN_SRO();
    }
    pass HEADwtHAND_DSRXON_ToSRO__OUTLINE_WRITE // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HEADwtHAND_DSRXON_ToSRO();
    }
    pass UPPER_DMRON_ToMRO__OUTLINE_WRITE // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UPPER_DMRON_ToMRO();
    }
    pass EmissiveMetalness_DENMRO_ToMRO__OUTLINE_WRITE // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EmissiveMetalness_DENMRO_ToMRO();
    }
    pass MI_ClothSim_DSEN_ToSRO__OUTLINE_WRITE // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MI_ClothSim_DSEN_ToSRO();
    }
///////
}
