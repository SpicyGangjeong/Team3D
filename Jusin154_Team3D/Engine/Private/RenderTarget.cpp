#include "pch.h"
#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    SAFE_ADDREF(m_pDevice);
    SAFE_ADDREF(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    D3D11_TEXTURE2D_DESC            TextureDesc{};

    m_vTargetSize.x = (_float)iSizeX;
    m_vTargetSize.y = (_float)iSizeY;
    TextureDesc.Width = iSizeX;
    TextureDesc.Height = iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = ePixelFormat;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;
    m_vClearColor = vClearColor;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D))) {
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV))) {
        return E_FAIL;
    }

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV))) {
        return E_FAIL;
    }


    m_vClearColor = vClearColor;


#ifdef _DEBUG
    Ready_Debug();
#endif

    return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    if (FAILED(pShader->Bind_SRV(pConstantName, m_pSRV))) {
        return E_FAIL;
    }

    return S_OK;
}
void CRenderTarget::Clear()
{
    m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

void CRenderTarget::Copy_ResourceTo(ID3D11Texture2D* pTexture2D)
{
    m_pContext->CopyResource(pTexture2D, m_pTexture2D);
}
void CRenderTarget::Copy_ResourceTo(const CRenderTarget& Target)
{
    m_pContext->CopyResource(Target.m_pTexture2D, m_pTexture2D);
}

void CRenderTarget::Copy_ResourceFrom(ID3D11Texture2D* pTexture2D)
{
    m_pContext->CopyResource(m_pTexture2D, pTexture2D);
}
void CRenderTarget::Copy_ResourceFrom(const CRenderTarget& Target)
{
    m_pContext->CopyResource(m_pTexture2D, Target.m_pTexture2D);
}
void CRenderTarget::Get_TextureDesc(D3D11_TEXTURE2D_DESC& Desc)
{
    m_pTexture2D->GetDesc(&Desc);
}
#ifdef _DEBUG

void CRenderTarget::Ready_Debug()
{

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity()); // 디버그 월드 메트릭스 초기화

    _uint		iNumViewports = { 1 };

    D3D11_VIEWPORT		Viewport{};
    m_pContext->RSGetViewports(&iNumViewports, &Viewport);

    m_ptWindowSize.x = (LONG)Viewport.Width;
    m_ptWindowSize.y = (LONG)Viewport.Height;

    return;
}

void CRenderTarget::Describe_Entity(const _char* pName)
{
    GUI::Selectable(pName, &m_isDraw);
}

void CRenderTarget::Toggle_RT_Debug()
{
    m_isDraw = !m_isDraw;
}

_bool CRenderTarget::Render_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer ,_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    if (m_isDraw == false) {
        return false;
    }

    m_WorldMatrix._11 = fSizeX;
    m_WorldMatrix._22 = fSizeY;
    m_WorldMatrix._41 = fX - m_ptWindowSize.x * 0.5f;
    m_WorldMatrix._42 = -fY + m_ptWindowSize.y * 0.5f;

    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix))){
        return false;
    }

    if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV))) {
        return false;
    }

    if (FAILED(pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::DEBUG)))) {
        return false;
    }

    if (FAILED(pVIBuffer->Bind_Resources())) {
        return false;
    }

    if (FAILED(pVIBuffer->Render())) {
        return false;
    }

    return true;
}

#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor)))
    {
        MSG_BOX("Failed to Created : CRenderTarget");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    __super::Free();


    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pContext);

    SAFE_RELEASE(m_pSRV);
    SAFE_RELEASE(m_pRTV);
    SAFE_RELEASE(m_pTexture2D);

}
