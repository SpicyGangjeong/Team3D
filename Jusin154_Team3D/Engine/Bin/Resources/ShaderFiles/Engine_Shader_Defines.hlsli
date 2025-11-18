
vector g_vMtrlDiffuse = 1.f;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;

float PI = 3.14592;

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler ClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

sampler MirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = mirror;
    AddressV = mirror;
};

sampler BorderZeroSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0, 0, 0, 0);
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
};

RasterizerState RS_Nocull
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
    DepthClipEnable = false;
};

RasterizerState RS_Nocull_Wireframe
{
    FillMode = Wireframe;
    CullMode = None;
    FrontCounterClockwise = false;
    DepthBias = 0;
    SlopeScaledDepthBias = 0.f;
    DepthBiasClamp = 0.f;
    DepthClipEnable = false;
    ScissorEnable = false;
    MultisampleEnable = false;
    AntialiasedLineEnable = false;
};

RasterizerState RS_Scissor
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
    ScissorEnable = true;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Effect
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
    StencilEnable = false;
};

BlendState BS_None
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend // ALPHA_BLEND
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = true;
    BlendEnable[6] = true;
    BlendEnable[7] = true;

    // Ac * Aa + Bc * (1 - Aa);
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Blend // ADDITIVE_BLEND
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = true;
    BlendEnable[6] = true;
    BlendEnable[7] = true;

    // Ac * (1) + Bc * (1);
    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

BlendState BS_UIBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = true;
    BlendEnable[6] = true;
    BlendEnable[7] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = Zero;
    BlendOpAlpha = Add;

    RenderTargetWriteMask[0] = 0x0F; // RGBA 모두 출력
};

BlendState BS_WB_Acc
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = true;
    BlendEnable[6] = true;
    BlendEnable[7] = true;

    SrcBlend[0] = one;
    DestBlend[0] = one;
    BlendOp[0] = Add;

    SrcBlend[1] = Zero;
    DestBlend[1] = Inv_Src_Alpha;
    BlendOp[1] = Add;


    SrcBlend[2] = one;
    DestBlend[2] = one;
    BlendOp[2] = Add;
};

BlendState BS_WeightBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = true;
    BlendEnable[6] = true;
    BlendEnable[7] = true;


    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    
    BlendOp[0] = Add;

    

};

float ShadowVisibility_hwPCF(Texture2D ShadowMap, float4 vLightClip, float2 vShadowMapSize, float bias)
{
    float2 uv = float2(vLightClip.x / vLightClip.w, -vLightClip.y / vLightClip.w) * 0.5f + 0.5f;
    float fDepthCenter = saturate(vLightClip.z / vLightClip.w);
    float2 vTexel = 1.f / vShadowMapSize;
    
    float fVisibility = 0.f;
    for (int v = -1; v <= 1; ++v)
    {
        for (int u = -1; u <= 1; ++u)
        {
            float2 vOffset = float2(u, v) * vTexel;
            float fDepth = ShadowMap.Sample(DefaultSampler, uv + vOffset).x;
            fVisibility += (fDepthCenter - bias > fDepth) ? 0.f : 1.f;
        }
    }
    return fVisibility / 9.f;
}

float GetRimLight(float3 vCamPosition, float3 vPosition, float3 vNormal, float fRimPower, float fRimStrength)
{
    float fRimLight = (1 - dot(normalize(vCamPosition - vPosition), vNormal));
    fRimLight = pow(fRimLight, fRimPower);
    fRimLight = fRimLight * fRimStrength;
    return fRimLight;
}

float2 Get_MovedUV(float2 vOriginalUV, float fDeltaU, float fDeltaV, uint iIndexU, uint iIndexV)
{
    float2 vDelta = float2(fDeltaU, fDeltaV);
    float2 vOffset = float2(iIndexU * fDeltaU, iIndexV * fDeltaV);
    return vOriginalUV * vDelta + vOffset;
}

float2 UV_Cutting(float2 vUV, float2 vUVCutting, int iCurrentFrame)
{
    float2 UV = vUV; // 이미지의 UV값
    
    int iTotalFrame = vUVCutting.x * vUVCutting.y; // 이미지의 최대 프레임 (몇 곱하기 몇인지)
    
    int iFrameX = iCurrentFrame % (int) vUVCutting.x; // 현재 x축의 위치(현재 이미지의 몇번째 칸을 보여줄 지)
    int iFrameY = iCurrentFrame / (int) vUVCutting.x; // 현재 y축의 위치(현재 이미지의 몇번째 줄을 보여줄 지)
    
    float fFreamWidth = 1.0 / vUVCutting.x; // 1.0 나누기 이미지 갯수를 해서 한칸에 얼마나 갈지 정해준다.
    float fFreamHeight = 1.0 / vUVCutting.y; // 1.0 나누기 이미지 갯수를 해서 한줄에 얼마나 갈지 정해준다.
    
    UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth; // 먼저 uv를 0~1이 아닌 0~fFrameWidth로 만든 다음에 한칸씩 옆으로 밀어준다.
    UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight; // 먼저 uv를 0~1이 아닌 0~fFrameHeight로 만든 다음에 한줄씩 밑으로 내려준다.
    
    return UV;
}

struct PBR_LIGHT_OUT
{
    float3 vShade;
    float3 vSpecular;
};

// (Kd F(Lambert)) + (Ks F(cook-torrance))

//float LambertDiffuse(float c)
//{
//    return c / PI;
//}

// F(cook-torrance)
// DFG / (4 (Wo dot N) (Wi dot N))

// D(distribution, NormalDistribution function), 표면의 거칠기에 영향을 받는 halfWay vector
// G(Geometry, GeometryFunc), 에너지 보존법칙 적용
// F(Fresnel-Fresnel equation), 표면 각도에 따른 반사율

// F->D
float NDF_ggxtr(float3 vNormal, float3 vHalfWayVector, float fAlpha) // NormalDistributionGGXTR, (H, halfWay vector), (A, Roughness), (N, Normal)
{
    float a2 = fAlpha * fAlpha;
    float NdotH = saturate(dot(vNormal, vHalfWayVector));
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.f) + 1.f);
    
    denom = PI * denom * denom;
    return (nom / denom);
}

// F->G
// GschlickGGX(n, v, k) 
// k는 이하 두개 조명 조건에 따라 가변적임
// Direct //  Kdir -> ((a + 1) * (a + 1)) / 8 // direct lighting 추천
// IBL Lighting // Kibl -> a * a / 2 // 이미지 기반 조명기법

float Geometry_SchlickGGX(float NdotV, float fK)
{
    return NdotV / (NdotV * (1.0f - fK) + fK);
}

float Geometry_Smith(float3 vNormal, float3 vFromView, float3 vFromLight, float k)
{
    // Geometry Obstruction     
    // 철이 있는, 주로 튀어나온 장애물
    float NdotV = saturate(dot(vNormal, vFromView));
    
    // Geometry Shadowing       
    // 요가 있는, 주로 음푹 들어간 장애물 (그래서 셰도잉)
    float NdotL = saturate(dot(vNormal, vFromLight));

    // 요철이 골고루 있다고 가정하고 적당히 섞음
    return Geometry_SchlickGGX(NdotV, k) * Geometry_SchlickGGX(NdotL, k); 
}

// F -> F
float3 Fresnel_Schlick(float cosTheta, float3 F0)
{
    // 모서리 부분의 반사
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

PBR_LIGHT_OUT PBR_Lighting(
    float3 vNormal, float3 vFromView, float3 vFromLight,
    float3 vAlbedo, float fMetallic, float fRoughness,
    float3 vLightColor, float fAttenuation, float3 vFO
) {
    PBR_LIGHT_OUT Out;
    Out.vShade = 0;
    Out.vSpecular = 0;
    
    float3 vHalfVector = normalize(vFromView + vFromLight);
    float NdotL = saturate(dot(vNormal, vFromLight));
    float NdotV = saturate(dot(vNormal, vFromView));
    if (NdotL <= 0 || NdotV <= 0)
    {
        return Out;
    }
    float fAlpha = max(fRoughness * fRoughness, 0.04f);
    float k = ((fRoughness + 1.f) * (fRoughness + 1.f)) / 8.f;
    
    float D = NDF_ggxtr(vNormal, vHalfVector, fAlpha);
    float G = Geometry_Smith(vNormal, vFromView, vFromLight, k);
    float3 F = Fresnel_Schlick(saturate(dot(vHalfVector, vFromView)), vFO);
    
    float3 vNumerator = D * G * F;
    float fDenom = max(4.f * NdotL * NdotV, 1e-7);
    float3 specularBRDF = vNumerator / fDenom;
    
    float3 kS = F;
    float3 kD = (1.f - kS) * (1.f -fMetallic);
    
    //Out.vShade = float(1.f, 1.f, 1. 1.f);
    Out.vShade = (kD / PI) * (NdotL * fAttenuation) * vLightColor;
    
    Out.vSpecular = specularBRDF * vLightColor * fAttenuation * NdotL;
    
    return Out;
}
