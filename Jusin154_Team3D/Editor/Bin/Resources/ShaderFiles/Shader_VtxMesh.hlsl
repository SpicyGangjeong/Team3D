
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;
float4x4 g_PrevWorldMatrix, g_PrevViewMatrix, g_PrevProjMatrix;

int g_bUVTargetDiffuse;
int g_bAdditiveDisolve;
int g_bDisolve;

float g_LifeRatio;
float g_fFar;
float g_fTime;
float g_fDeltaU;
float g_fDeltaV;
float4 g_vOutLineColor;
float g_fOutLineScale;
float g_fOutLineThickness;
float g_fOutLinePower;
vector g_vCamPosition;
uint g_iIndexU;
uint g_iIndexV;
float2 g_vTime;
float g_fMBIntensity = 1.f;

float g_fDisolveRatio;
float g_fUsingSurfaceParams;

float2 g_vClipBoxSize;
float2 g_vClipBoxLTPos;

float g_fDisolveAmount;
float g_fDisolveEdgeWidth;

Texture2D g_DAOTexture;
Texture2D g_THVTexture;
Texture2D g_SurfaceParamsTexture;
Texture2D g_NoiseTexture;
Texture2D g_CausticsTexture;

float2 g_vSRVFlag;
float3 g_vPBR_Flag;

Texture2D g_DiffuseTexture;
Texture2D g_SpecularTexture;
Texture2D g_AmbientTexture;
Texture2D g_EmissiveTexture;
Texture2D g_MossDiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_MossNormalTexture;
Texture2D g_NormalBlendTexture;
Texture2D g_SROBlendTexture;
Texture2D g_MROBlendTexture;
Texture2D g_ReflectionTexture;
Texture2D g_DiffuseBlend;
Texture2D g_MossSROTexture;
Texture2D g_MossMROTexture;
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


int g_iBinded_Texture[AI_TEXTURE_TYPE_MAX];

Texture2D g_MaskingTexture;
Texture2D g_ClippingTexture;
Texture2D g_DisolveTexture;
Texture2D g_GlowTexture;
Texture2D g_GlassTexture;

TextureCube g_CubeTexture;
Texture2D g_NormalLargeTexture;
Texture2D g_NormalSubTexture;
Texture2D g_DeadDisolveTexture;

Texture2D g_DepthTexture;
Texture2D g_NormalMapTexture;
Texture2D g_MaskNoiseTexture;
Texture2D g_DiffsueMaskTexture;

float g_fNormalValue1;
float g_fNormalValue2;
float g_fNormalValue3;

float2 g_vUVSpeed1;
float2 g_vUVSpeed2;
float2 g_vUVSpeed3;

float g_fBloomStrength;
float g_fGlassRatio;
float g_fSpecularIntensity;

float g_fRefractionStrength;
float g_fRefractionPow;
float4 g_vDiffuseColor;
float4 g_vSurfaceColor;

float   g_fRimPower;
float   g_fRimStrength;
float4  g_vRimColor;

float g_fWinSizeX;
float g_fWinSizeY;

float4 g_vMaskColorRed;
float4 g_vMaskColorGreen;
float4 g_vMaskColorBlue;

float   g_fFogDensity;
float   g_fFogPow;
float   g_fNormalThreshold;
bool    g_bFogVisible;
vector  g_vFogColor;

float g_fSoftMask;
float g_fSoftMaskEdge;

float g_fUVTiling;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPrevProjPos : TEXCOORD3;
};

struct VS_OUT_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
};

struct VS_OUT_DECAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};


VS_OUT_BLUR VS_MAIN_BLUR(VS_IN In)
{
    VS_OUT_BLUR Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;

    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

    return Out;
}
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    
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
    Out.vPrevProjPos = mul(float4(In.vPosition, 1.f), matPrevWVP);
    return Out;
}

VS_OUT_DECAL VS_MAIN_DECAL(VS_IN In)
{
    VS_OUT_DECAL Out;
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;

    return Out;
}

VS_OUT_BLUR VS_BLOOM(VS_IN In)
{
    VS_OUT_BLUR Out;
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct VS_OUT_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPrevProjPos : TEXCOORD3;
};

VS_OUT_OUTLINE VS_MAIN_OUTLINE(VS_IN In)
{
    VS_OUT_OUTLINE Out;
    
    vector vPosition = vector(In.vPosition, 1.f);
    vector vNormal = vector(In.vNormal, 0.f);
    vPosition.xyz += (vNormal.xyz * g_fOutLineThickness).xyz;


    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matPrevWV, matPrevWVP;
    matPrevWV = mul(g_PrevWorldMatrix, g_PrevViewMatrix);
    matPrevWVP = mul(matPrevWV, g_PrevProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vector(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vPrevProjPos = mul(vPosition, matPrevWVP);
    return Out;
}

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
VS_OUT_EFFECT VS_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}
struct VS_OUT_CAPTUREDMODEL
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_CAPTUREDMODEL VS_CAPTUREDMODEL(VS_IN In)
{
    VS_OUT_CAPTUREDMODEL Out;
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
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
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vSurface = g_SurfaceParamsTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, In.vTexcoord);
    
    if (1 == g_vSRVFlag.x)
    {
        vMtrlDiffuse = BlendDiffuse(vMtrlDiffuse, g_DiffuseBlend, In.vTexcoord, 0.3f);
      
    }
    if (2 == g_vSRVFlag.x)
    {
        vMtrlDiffuse = BlendDiffuse(vMtrlDiffuse, g_MossDiffuseTexture, In.vTexcoord, 0.3f);
      
    }
    if (3 == g_vSRVFlag.x)
    {
        vMtrlDiffuse = float4(vMtrlDiffuse.xyz * 0.4f
            + g_DiffuseBlend.Sample(DefaultSampler, In.vTexcoord).xyz * 0.4f
            + g_MossDiffuseTexture.Sample(DefaultSampler, In.vTexcoord).xyz * 0.1f, 1.f);
    }
    if (5 == g_vSRVFlag.x)
    {
        float4 vMask = g_Maya_BaseTexture.Sample(DefaultSampler, In.vTexcoord);
        float fTotalMask = vMask.r + vMask.g + vMask.b + vMask.a;
        
        float4 vDiffuseB = g_DiffuseBlend.Sample(DefaultSampler, In.vTexcoord);
        float4 vDiffuseC = g_MossDiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        float4 vDiffuseD = g_ClearcoadTexture.Sample(DefaultSampler, In.vTexcoord);
        float4 vFinalColor = (vMtrlDiffuse * vMask.r +
                                vDiffuseB * vMask.g +
                                vDiffuseC * vMask.b +
                                vDiffuseD * vMask.a) / fTotalMask;
        vMtrlDiffuse = vFinalColor;
        
        float4 vSurfaceB = g_SROBlendTexture.Sample(DefaultSampler, In.vTexcoord);
        float4 vSurfaceC = g_MossSROTexture.Sample(DefaultSampler, In.vTexcoord);
        float4 vSurfaceD = g_SheenTexture.Sample(DefaultSampler, In.vTexcoord);
        float4 vFinalSurface = (vSurface * vMask.r +
                                vSurfaceB * vMask.g +
                                vSurfaceC * vMask.b +
                                vSurfaceD * vMask.a) / fTotalMask;
        vSurface = vFinalSurface;
    }
    
    if (1 == g_vSRVFlag.y)
    {
        vNormalDecoded = normalize(vNormalDecoded + DecodeNormalFromRG(g_NormalBlendTexture, DefaultSampler, In.vTexcoord));
    }
    else if (2 == g_vSRVFlag.y)
    {
        vNormalDecoded = normalize(vNormalDecoded + DecodeNormalFromRG(g_MossNormalTexture, DefaultSampler, In.vTexcoord));
    }
    else if (3 == g_vSRVFlag.y)
    {
        vNormalDecoded = normalize(vNormalDecoded + DecodeNormalFromRG(g_NormalBlendTexture, DefaultSampler, In.vTexcoord) + DecodeNormalFromRG(g_MossNormalTexture, DefaultSampler, In.vTexcoord));
     }
    
    if (g_iBinded_Texture[AI_TEXTURE_TYPE_TRANSMISSION] != 0)
    {
        float4 vTransmission = g_TransmissionTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
        vMtrlDiffuse *= vTransmission;
    }
    if (g_iBinded_Texture[AI_TEXTURE_TYPE_EMISSIVE] != 0)
    {
        float4 vEmissive = g_EmissiveTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
        vMtrlDiffuse += vEmissive;
    }
    if (vMtrlDiffuse.a < 0.2f)
    {
        discard;
    }
   
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
   
    Out.vAlbedo = float4(vMtrlDiffuse.xyz, 1.f);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    float fSurfaceParam = g_fUsingSurfaceParams;
    if (true == AlmostEqual7(g_fUsingSurfaceParams, 0.f))
    {
        fSurfaceParam = 0.f;
    }
    
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        fSurfaceParam, // 서페이스 파라미터
        1.f);
    
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    
    return Out;
}

PS_OUT PS_MAIN_SELECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vNormalDesc = g_NormalTexture.Sample(MirrorSampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = mul(vNormalDesc.xyz * 2.f - 1.f, WorldMatrix);
    
    Out.vAlbedo = float4(0.f, 0.5f, 0.5f, 1.f);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 1.f);
    return Out;
}

PS_OUT PS_GLASS(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vGlassDiffuse = g_GlassTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    vector vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    
    Out.vAlbedo = lerp(vGlassDiffuse, vMtrlDiffuse, g_fGlassRatio);
    Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    Out.vNormal = float4(In.vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 15.f / 27.f, 1.f);
    Out.vSurface = vSurface;
    Out.vVelocityUV = float2(0.5f, 0.5f);
    //CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    
    return Out;
}

PS_OUT PS_GLASS_CUBE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    vector vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3 vNormalMap = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(normalize(vNormalMap), WorldMatrix));
    
    float3 vReflaction = reflect(normalize(In.vWorldPos.xyz - g_vCamPosition.xyz), vNormal);
    
    float4 vReflactionDiffuse = g_CubeTexture.Sample(DefaultSampler, vReflaction);
    
    float fFresnelScala = 0.02f + (1.f - 0.02f) * pow(1.f - max(0.f, dot(normalize(g_vCamPosition.xyz - In.vWorldPos.xyz), vNormal)), 5.f);
    
    Out.vAlbedo = (float4(vMtrlDiffuse.xyz, 1.f) * fFresnelScala + vReflactionDiffuse  * (1.f - fFresnelScala)) * 0.7f;
    Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    Out.vNormal = float4(In.vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 15.f / 27.f, 1.f);
    Out.vSurface = vSurface;
    Out.vVelocityUV = float2(0.5f, 0.5f);
    //CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);
    
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

struct PS_IN_BLUR
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_BLUR
{
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT_BLUR PS_MAIN_SKYBOX(PS_IN In)
{
    PS_OUT_BLUR Out;
    
    float4 vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //if (0.f == g_fFogPow)
    //{
    //    Out.vDiffuse = vColor;
    //    return Out;
    //}
    
    //vector vFinalColor;
    //float fRatio;
    
    //fRatio = pow(1.f - saturate(exp(-1.f * g_fFar * g_fFogDensity)), g_fFogPow);
    //vFinalColor = lerp(vColor, g_vFogColor, fRatio);
    //vFinalColor.a = 1.f;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_BLUR PS_MAIN_BLUR(PS_IN_BLUR In)
{
    PS_OUT_BLUR Out = (PS_OUT_BLUR) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vDiffuse = saturate(vMtrlDiffuse);
    
    return Out;
}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    
    Out.fShadowLightDepth = In.vPosition.z;
    
    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};
struct PS_OUT_EFFECT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT_EFFECT PS_EFFECT_UVMOVE(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out;
    
    float2 vMovedUV = Get_MovedUV(In.vTexcoord, g_fDeltaU, g_fDeltaV, g_iIndexU, g_iIndexV);
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, vMovedUV);
    
    if (vDiffuse.a < 0.01f)
    {
        discard;
    }
    
    Out.vColor = vDiffuse;
    
    return Out;
}

PS_OUT_EFFECT PS_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out;
    
    float2 uv = In.vTexcoord;
    float2 vClipBoxUV = g_vClipBoxLTPos + g_vClipBoxSize * In.vTexcoord;
    uv.x += g_fDeltaU * g_fTime;
    uv.y += g_fDeltaV * g_fTime;

    vector vDiffuse;
    vector vMasking;
    vector vClipCell = g_ClippingTexture.Sample(DefaultSampler, vClipBoxUV);
    vector vDisolveCell = g_DisolveTexture.Sample(DefaultSampler, In.vTexcoord);
    if (true == g_bUVTargetDiffuse)
    {
        vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, uv);
        vMasking = g_MaskingTexture.Sample(DefaultSampler, In.vTexcoord);
    }
    else
    {
        vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
        vMasking = g_MaskingTexture.Sample(DefaultSampler, uv);
    }
    if (true == g_bAdditiveDisolve)
    {
        vDiffuse.a = saturate(vDiffuse.a * vMasking.r * vClipCell.r + vDisolveCell.r * g_fDisolveRatio);
    }
    else
    {
        vDiffuse.a = saturate(vDiffuse.a * vMasking.r * vClipCell.r - vDisolveCell.r * g_fDisolveRatio);
    }
    
    if (vDiffuse.a < 0.01f)
    {
        discard;
    }
    
    Out.vColor = vDiffuse;
    
    return Out;
}

PS_OUT_EFFECT PS_BLOOM(PS_IN_EFFECT In)
{
    PS_OUT_EFFECT Out;
    
    vector vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = float4((vEmissive * g_fBloomStrength).xyz, 2.f / 255.f);
    
    return Out;
}

struct PS_IN_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPrevProjPos : TEXCOORD3;
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
    
    Out.vAlbedo = float4(g_vOutLineColor.rgb * 5.f, fRim);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 源딆씠 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 酉??ㅽ럹?댁뒪 Z 
        (float) AI_TEXTURE_TYPE_METALNESS / (float) AI_TEXTURE_TYPE_MAX, // ?쒗럹?댁뒪 ?뚮씪誘명꽣
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    Out.vSurface = float4(0.5f, 1.f, 1.f, 0.f);
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

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

PS_OUT PS_LAKE(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float2 vUV = float2(In.vTexcoord.x + cos(g_fTime * g_vUVSpeed1.x), In.vTexcoord.y + cos(g_fTime * g_vUVSpeed1.y)) * g_fNormalValue1;
    float2 vUVLarge = float2(In.vTexcoord.x + cos(g_fTime * g_vUVSpeed2.x), In.vTexcoord.y + cos(g_fTime * g_vUVSpeed2.y)) * g_fNormalValue2;
    float2 vUVSub = float2(In.vTexcoord.x + cos(g_fTime * g_vUVSpeed3.x), In.vTexcoord.y + cos(g_fTime * g_vUVSpeed3.y)) * g_fNormalValue3;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord * 30.f);
    float3 vNoise = (g_NoiseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord + g_fTime * 0.1f).rgb - 0.5f) * 0.3f;
    float4 vCaustics = g_CausticsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord * 10.f + vNoise.g);
    
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, vUV + vNoise.g);
    float3 vNormalLarge = DecodeNormalFromRG(g_NormalLargeTexture, AnisoTropy_BLUR_Sampler, vUVLarge + vNoise.r);
    float3 vNormalSub = DecodeNormalFromRG(g_NormalSubTexture, AnisoTropy_BLUR_Sampler, vUVSub + vNoise.b);
    
    vMtrlDiffuse = (vMtrlDiffuse + vCaustics * 0.3f) * g_vDiffuseColor;
    
    vector vSurface = g_vSurfaceColor;
    
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(float3(normalize(vNormalDecoded * 0.5f + vNormalLarge * 0.2f + vNormalSub * 0.3f).xy, 1.f), WorldMatrix));
    
    
    float3 vViewDir = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    
    float3 vReflaction = reflect(-1.f * vViewDir, vNormal);
    
    float4 vCubeDiffuse = g_CubeTexture.Sample(AnisoTropy_BLUR_Sampler, vReflaction);
    
    float fFresnelScala = g_fRefractionStrength + (1.f - g_fRefractionStrength) * pow(1.f - saturate(dot(vViewDir, vNormal)), g_fRefractionPow);
    fFresnelScala = saturate(fFresnelScala * 0.7f);

    Out.vAlbedo = lerp(vMtrlDiffuse, vCubeDiffuse, fFresnelScala);
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
    (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
    2.f / 27.f, // 서페이스 파라미터
    1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = vSurface;
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

PS_OUT PS_LAND(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord * 10.f);
    float4 vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord * 10.f);
  
    if (vMtrlDiffuse.a < 0.2f)
    {
        discard;
    }


    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
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
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

PS_OUT PS_NONMRO(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
  
    if (vMtrlDiffuse.a < 0.2f)
    {
        discard;
    }

    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    Out.vAlbedo = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4((In.vProjPos.z / In.vProjPos.w), // NDC 깊이 ( 0~ 1)
        (In.vProjPos.w / g_fFar), // 뷰 스페이스 Z 
        g_fUsingSurfaceParams, // 서페이스 파라미터
        1.f);
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vSurface = g_vSurfaceColor;
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

PS_OUT PS_SPECTOR_WEAPON_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vMtrlDiffuse = g_EmissiveTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float4 vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    
    if (true == g_bDisolve)
    {
        float4 vBurnColor = g_DisolveTexture.Sample(DefaultSampler, In.vTexcoord);
        vMtrlDiffuse = ApplyDissolve(g_DeadDisolveTexture, g_fDisolveRatio, g_fDisolveAmount, g_fDisolveEdgeWidth, vBurnColor, vMtrlDiffuse, In.vTexcoord);
    }
    
    if (vMtrlDiffuse.a < 0.2f)
    {
        discard;
    }


    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
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
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}
PS_OUT PS_LEVIOSO(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float4 vSurface = g_SurfaceParamsTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float fNoise = g_NoiseTexture.Sample(AnisoTropy_BLUR_Sampler, float2(In.vTexcoord.x + sin(g_fTime), In.vTexcoord.y)).r;
  
    if (g_iBinded_Texture[AI_TEXTURE_TYPE_TRANSMISSION] != 0)
    {
        float4 vTransmission = g_TransmissionTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
        vMtrlDiffuse *= vTransmission;
    }
    if (g_iBinded_Texture[AI_TEXTURE_TYPE_EMISSIVE] != 0)
    {
        float4 vEmissive = g_EmissiveTexture.Sample(AnisoTropy_BLUR_Sampler, In.vTexcoord);
        vMtrlDiffuse += vEmissive;
    }
    if (vMtrlDiffuse.a < 0.2f)
    {
        discard;
    }


    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, AnisoTropy_BLUR_Sampler, In.vTexcoord);
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(mul(vNormalDecoded, WorldMatrix));
    
    
    float4 vRimLight = 1.f - saturate(dot(normalize(g_vCamPosition - In.vWorldPos), float4(In.vNormal, 0.f)));
    
    vRimLight = pow(vRimLight, g_fRimPower);
    vRimLight = vRimLight * g_vRimColor * g_fRimStrength * fNoise;
    
    Out.vAlbedo = vMtrlDiffuse + vRimLight;
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
    Out.vVelocityUV = CalcVelocityUV(In.vProjPos, In.vPrevProjPos, g_fMBIntensity);

    return Out;
}

struct PS_IN_DECAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};
struct PS_OUT_DELCAL
{
    float4 vAlbedo : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vSurface : SV_Target2;
};

PS_OUT_DELCAL PS_MAIN_DECAL(PS_IN_DECAL In)
{
    PS_OUT_DELCAL Out;
    
    float2 vDepthUV;
    vDepthUV.x = In.vPosition.x / g_fWinSizeX;
    vDepthUV.y = In.vPosition.y / g_fWinSizeY;
    
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vDepthUV);
    float3 vNormalDesc = g_NormalMapTexture.Sample(DefaultSampler, vDepthUV).xyz;
    vNormalDesc = normalize(vNormalDesc * 2.f -1.f);
    
    float fViewZ = vDepthDesc.y * g_fFar;
    
    float4 vPosition;
    
    vPosition.x = vDepthUV.x * 2.f - 1.f;
    vPosition.y = vDepthUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
    
    float4 vViewPos = mul(vPosition, g_ProjMatrixInv);
    vViewPos /= vViewPos.w;
    
    float4 vWorldPos = mul(vViewPos, g_ViewMatrixInv);

    vector vLocalPos = mul(vWorldPos, g_WorldMatrixInv);

    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    
    clip(0.5f - ObjectAbsPos);
    
    
    float3 vAbsNormal = abs(vNormalDesc);
    
    int iAxisIndex = 0;
    
    if (vAbsNormal.y > vAbsNormal.x && vAbsNormal.y > vAbsNormal.z)
        iAxisIndex = 1;
    else if (vAbsNormal.z > vAbsNormal.x && vAbsNormal.z > vAbsNormal.y)
        iAxisIndex = 2;

    float2 vDecalUV;
    
    if (0 == iAxisIndex)
        vDecalUV = vLocalPos.zy + 0.5f * 2.f;
    else if (1 == iAxisIndex)
        vDecalUV = vLocalPos.xz + 0.5f * 2.f;
    else
        vDecalUV = vLocalPos.xy + 0.5f * 2.f;
    
    
    float2 vTileUV = vDecalUV * g_fUVTiling;
    
    float4 vMaskColor = g_MaskingTexture.Sample(DefaultSampler, vTileUV);
    if (vMaskColor.a < 0.2f)
    {
        discard;
    }
    
    vector vNoise = g_NoiseTexture.Sample(DefaultSampler, vTileUV + g_vTime);
    
    float4 vSurface = g_SurfaceParamsTexture.Sample(DefaultSampler, vTileUV);
    float3 vNormalDecoded = DecodeNormalFromRG(g_NormalTexture, DefaultSampler, vTileUV);
    
    float fMaskWeight = vMaskColor.r + vMaskColor.g + vMaskColor.b;
    
    float4 vDiffuseColor = vMaskColor.r * g_vMaskColorRed * 2.f * vNoise.r +
                            vMaskColor.g * g_vMaskColorGreen +
                            vMaskColor.b * g_vMaskColorBlue * 2.f * vNoise.r;
    
    vDiffuseColor /= fMaskWeight;
        
    vDiffuseColor.a = max(0.001f, vDiffuseColor.a);
 
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal * -1.f, In.vNormal);
    
    float3 vNormal = normalize(vNormalDesc);
    
    float fMask = g_DiffsueMaskTexture.Sample(DefaultSampler, vDecalUV *0.5f).r;
    
    if (0.f == fMask)
        discard;
    
    Out.vAlbedo = vDiffuseColor;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 1.f);
    
    float fSurfaceParam = g_fUsingSurfaceParams;
    if (true == AlmostEqual7(g_fUsingSurfaceParams, 0.f))
    {
        fSurfaceParam = 0.f;
    }
    
    Out.vSurface = vSurface;
    
    return Out;
}

technique11 MeshTechnique11
{
    pass MeshPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ShadowPass
    {
        SetRasterizerState(RS_Shadow);
        SetDepthStencilState(DSS_ShadowWrite, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
 
    pass SkyBoxPass // 2
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN_SKYBOX();
    }

    pass ALPHABlendPass // 3
    {
        SetRasterizerState(RS_Nocull_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN();
    }



    pass EffectPass_AlphaBlend // 4
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass EffectPass_Blend // 5
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass EffectPass_AlphaBlend_Cull // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass EffectPass_Blend_Cull // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        
        PixelShader = compile ps_5_0 PS_EFFECT();
    }
    pass CAPTUREDMODELPASS // 8
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_CAPTUREDMODEL();
        
        PixelShader = compile ps_5_0 PS_CAPTUREDMODEL();
    }
    pass BlurPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_BLUR();
        
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }
    pass UVMovePass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Effect, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_EFFECT();
        
        PixelShader = compile ps_5_0 PS_EFFECT_UVMOVE();
    }

    pass MapToolPass // 11
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN_SELECT();
    }

    pass GlassPass // 12
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_GLASS();
    }

    pass OutLine_Write_Pass // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default_OutLine_SWrite, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass OutLine_Read_Pass // 14
    {
        SetRasterizerState(RS_Front);
        SetDepthStencilState(DSS_Default_OutLine_SRead, 2);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_OUTLINE();
        
        PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
    }

    pass BloomPass // 15
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BLOOM();
        
        PixelShader = compile ps_5_0 PS_BLOOM();
    }

    pass WaterPass // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_LAKE();
    }

    pass LandPass // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_LAND();
    }

    pass NonMROPass // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_NONMRO();
    }

    pass SpectorWeaponPass // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_SPECTOR_WEAPON_MAIN();
    }

    pass GlassCubePass // 20
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_GLASS_CUBE();
    }

    pass LeviosoPass // 21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        
        PixelShader = compile ps_5_0 PS_LEVIOSO();
    }

    pass DecalPass // 22
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_DECAL();
        
        PixelShader = compile ps_5_0 PS_MAIN_DECAL();
    }
}
