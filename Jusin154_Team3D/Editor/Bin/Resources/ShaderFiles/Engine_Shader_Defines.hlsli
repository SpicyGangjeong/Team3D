
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

    RenderTargetWriteMask[0] = 0x0F; // RGBA 紐⑤몢 異쒕젰
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
    float2 UV = vUV; // ?대?吏??UV媛?
    
    int iTotalFrame = vUVCutting.x * vUVCutting.y; // ?대?吏??理쒕? ?꾨젅??(紐?怨깊븯湲?紐뉗씤吏)
    
    int iFrameX = iCurrentFrame % (int) vUVCutting.x; // ?꾩옱 x異뺤쓽 ?꾩튂(?꾩옱 ?대?吏??紐뉖쾲吏?移몄쓣 蹂댁뿬以?吏)
    int iFrameY = iCurrentFrame / (int) vUVCutting.x; // ?꾩옱 y異뺤쓽 ?꾩튂(?꾩옱 ?대?吏??紐뉖쾲吏?以꾩쓣 蹂댁뿬以?吏)
    
    float fFreamWidth = 1.0 / vUVCutting.x; // 1.0 ?섎늻湲??대?吏 媛?닔瑜??댁꽌 ?쒖뭏???쇰쭏??媛덉? ?뺥빐以??
    float fFreamHeight = 1.0 / vUVCutting.y; // 1.0 ?섎늻湲??대?吏 媛?닔瑜??댁꽌 ?쒖쨪???쇰쭏??媛덉? ?뺥빐以??
    
    UV.x = UV.x * fFreamWidth + iFrameX * fFreamWidth; // 癒쇱? uv瑜?0~1???꾨땶 0~fFrameWidth濡?留뚮뱺 ?ㅼ쓬???쒖뭏???놁쑝濡?諛?댁???
    UV.y = UV.y * fFreamHeight + iFrameY * fFreamHeight; // 癒쇱? uv瑜?0~1???꾨땶 0~fFrameHeight濡?留뚮뱺 ?ㅼ쓬???쒖쨪??諛묒쑝濡??대젮以??
    
    return UV;
}
