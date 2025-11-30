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

RasterizerState RS_NocullOcclusion
{
    FillMode = Solid;
    CullMode = None;
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

RasterizerState RS_Front
{
    FillMode = Solid;
    CullMode = Front;
};


/*
typedef struct D3D10_DEPTH_STENCIL_DESC
    {
    BOOL                        DepthEnable;
    D3D10_DEPTH_WRITE_MASK      DepthWriteMask;
    D3D10_COMPARISON_FUNC       DepthFunc;
    BOOL                        StencilEnable;
    UINT8                       StencilReadMask;
    UINT8                       StencilWriteMask;
    D3D10_DEPTH_STENCILOP_DESC  FrontFace;
    D3D10_DEPTH_STENCILOP_DESC  BackFace;
    } 	D3D10_DEPTH_STENCIL_DESC;

*/
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

DepthStencilState DSS_Default_Environment_SWrite
{
    DepthEnable = true;
    DepthWriteMask = true;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Replace;
    FrontFaceStencilFunc = Always;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Replace;
    BackFaceStencilFunc = Always;
};
DepthStencilState DSS_Default_Environment_SRead
{
    DepthEnable = true;
    DepthWriteMask = true;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff; // <-- ff -> 1111 1111 씀
    StencilWriteMask = 0x00; // <-- 00 -> 0000 0000 안씀

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFunc = Equal;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Keep;
    BackFaceStencilFunc = NOT_Equal;
};

DepthStencilState DSS_Default_OutLine_SWrite
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Replace;
    FrontFaceStencilFunc = Always;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Replace;
    BackFaceStencilFunc = Always;
};

DepthStencilState DSS_Default_OutLine_SRead
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xff;     // <-- ff -> 1111 1111 씀
    StencilWriteMask = 0x00;    // <-- 00 -> 0000 0000 안씀

    FrontFaceStencilFail = Keep;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFunc = Equal;

    BackFaceStencilFail = Keep;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Keep;
    BackFaceStencilFunc = NOT_Equal;
};

DepthStencilState DSS_Occlusion
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

BlendState BS_None
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
    BlendEnable[2] = false;
    BlendEnable[3] = false;
    BlendEnable[4] = false;
    BlendEnable[5] = false;
    BlendEnable[6] = false;
    BlendEnable[7] = false;
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

    SrcBlend[1] = Inv_Src_Alpha;
    DestBlend[1] = zero;
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

BlendState BS_WhiteMask
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
    BlendEnable[2] = false;
    BlendEnable[3] = false;
    BlendEnable[4] = false;
    BlendEnable[5] = false;
    BlendEnable[6] = false;
    BlendEnable[7] = false;

    RenderTargetWriteMask[0] = 0;
    RenderTargetWriteMask[1] = 0;
    RenderTargetWriteMask[2] = 0;
    RenderTargetWriteMask[3] = 0;
    RenderTargetWriteMask[4] = 0;
    RenderTargetWriteMask[5] = 0;
    RenderTargetWriteMask[6] = 0;
    RenderTargetWriteMask[7] = 0;
  
};

#endif // ENGINE_SHADER_STATES_HLSLI
