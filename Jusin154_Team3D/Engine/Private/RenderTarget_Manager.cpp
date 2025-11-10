#include "pch.h"
#include "RenderTarget_Manager.h"
#include "RenderTarget.h"

CRenderTarget_Manager::CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBase(),
    m_pDevice{ pDevice },
    m_pContext{ pContext }
{
    SAFE_ADDREF(m_pDevice);
    SAFE_ADDREF(m_pContext);
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(const _wstring& strRenderTargetKey, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strRenderTargetKey)) {
        return E_FAIL;
    }
    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget) {
        return E_FAIL;
    }

    m_RenderTargets.emplace(strRenderTargetKey, pRenderTarget);


    return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const _wstring& strMultiRenderTargetKey, const _wstring& strRenderTargetKey)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetKey);
    if (nullptr == pRenderTarget) {
        return E_FAIL;
    }

    list<CRenderTarget*>* pMRTList = Find_MRT(strMultiRenderTargetKey);
    if (nullptr == pMRTList)
    {
        list<CRenderTarget*>       MRTList;
        MRTList.push_back(pRenderTarget);
        m_MRTs.emplace(strMultiRenderTargetKey, MRTList);
    }
    else {
        pMRTList->push_back(pRenderTarget);
    }

    SAFE_ADDREF(pRenderTarget);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const _wstring& strMultiRenderTargetKey, ID3D11DepthStencilView* pDSV)
{
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    list<CRenderTarget*>* pMRTList = Find_MRT(strMultiRenderTargetKey);

    if (nullptr == pMRTList)
        return E_FAIL;

    _uint iNumRenderTargets = { 0 };
    ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);

    if (nullptr != pDSV) {
        m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRenderTargetViews[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }
    if (0 == iNumRenderTargets) {
        return E_FAIL;
    }

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargetViews, (nullptr == pDSV) ? m_pOriginalDSV : pDSV);

    return S_OK;
}
HRESULT CRenderTarget_Manager::Begin_MRT_Include_BackBuffer(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    _uint iNumRenderTargets = { 0 };
    ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);

    if (nullptr != pDSV) {
        m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }

    pRenderTargetViews[iNumRenderTargets++] = m_pBackBufferRTV;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRenderTargetViews[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }
    if (0 == iNumRenderTargets) {
        return E_FAIL;
    }

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargetViews, (nullptr == pDSV) ? m_pOriginalDSV : pDSV);

    return S_OK;
}
HRESULT CRenderTarget_Manager::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget) {
        return E_FAIL;
    }

    pRenderTarget->Copy_Resource(pTexture2D);

    return S_OK;
}


HRESULT CRenderTarget_Manager::End_MRT()
{
    ID3D11RenderTargetView* pRenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { m_pBackBufferRTV };

    m_pContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, pRenderTargets, m_pOriginalDSV);

    SAFE_RELEASE(m_pBackBufferRTV);
    SAFE_RELEASE(m_pOriginalDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_RenderTarget(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget) {
        return E_FAIL;
    }

    return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}
#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_RenderTarget_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget) {
        return E_FAIL;
    }
    return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTarget_Manager::Render_RenderTarget_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList) {
        return E_FAIL;
    }

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Render_Debug(pShader, pVIBuffer);
    }

    return S_OK;
}

#endif // _DEBUG

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const _wstring& strRenderTargetKey)
{
    auto    iter = m_RenderTargets.find(strRenderTargetKey);

    if (iter == m_RenderTargets.end()) {
        return nullptr;
    }

    return iter->second;
}
list<class CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const _wstring& strMultiRenderTargetKey)
{
    auto    iter = m_MRTs.find(strMultiRenderTargetKey);

    if (iter == m_MRTs.end()) {
        return nullptr;
    }

    return &iter->second;
}
CRenderTarget_Manager* CRenderTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CRenderTarget_Manager(pDevice, pContext);
}
void CRenderTarget_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second) {
            SAFE_RELEASE(pRenderTarget);
        } Pair.second.clear();

    }
    m_MRTs.clear();

    for (auto& Pair : m_RenderTargets) {
        SAFE_RELEASE(Pair.second);
    }

    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pContext);
}
