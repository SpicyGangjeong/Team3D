
#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_WorldMatrixInv, g_ViewMatrixInv, g_ProjMatrixInv;

float g_fFar;
float g_fNormalThreshold;
float3 g_vContainerMin;
float3 g_vContainerMax;

Texture2D g_DepthTexture;
Texture2D g_DiffuseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_DECAL(PS_IN In)
{
    PS_OUT Out;
    
    float2 vDepthUV;
    vDepthUV.x = In.vPosition.x / 1920.f;
    vDepthUV.y = In.vPosition.y / 1080.f;
    
    float4 vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vDepthUV);

    float fViewZ = vDepthDesc.y * g_fFar;
    
    float4 vPosition;
    
    /* (로컬위치 * 월드 * 뷰 * 투영 / w) -> (로컬위치 * 월드)   */
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
    
    float2 vDecalUV = vLocalPos.xz + 0.5f;
    
    float4 Color = g_DiffuseTexture.Sample(DefaultSampler, vDecalUV);
    if (0.3f > Color.a)
        discard;
    Out.vColor = Color; //float4(1.f, vDecalUV.x, vDecalUV.y, 1.f);
    
    
    return Out;
}

PS_OUT PS_DEBUG(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = float4(1.f, 0.5f, 0.5f, 1.f);
    
    return Out;
}

technique11 NorTechnique11
{
    pass BoundingBoxPass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Occlusion, 0);
        SetBlendState(BS_WhiteMask, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass BoundingDebugPass
    {
        SetRasterizerState(RS_Nocull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEBUG();
    }
 
    pass Decal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DECAL();
    }
}
