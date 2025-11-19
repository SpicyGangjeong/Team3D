#ifndef ENGINE_SHADER_STATES_HLSLI
#define ENGINE_SHADER_STATES_HLSLI

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

#endif // ENGINE_SHADER_STATES_HLSLI
