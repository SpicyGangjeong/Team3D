#include "pch.h"
#include "RenderTarget_Manager.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

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
HRESULT CRenderTarget_Manager::Begin_MRT_NO_DepthStencil(const _wstring& strMRTTag)
{
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    _uint iNumRenderTargets = { 0 };
    ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Clear();
        pRenderTargetViews[iNumRenderTargets++] = pRenderTarget->Get_RTV();
    }
    if (0 == iNumRenderTargets) {
        return E_FAIL;
    }

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargetViews, nullptr);

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

HRESULT CRenderTarget_Manager::Paste_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget) {
        return E_FAIL;
    }

    pRenderTarget->Paste_Resource(pTexture2D);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Accumulate_RenderTarget(CVIBuffer_Rect* pVIBuffer, CShader* pShader, 
    const _wstring& wstrRenderTarget_SrcA, const _wstring& wstrRenderTarget_SrcB, 
    const _wstring& wstrRenderTarget_Target, SHADER_PASS_DEFERRED ePass)
{
    if (nullptr == pVIBuffer || nullptr == pShader) {
        return E_FAIL;
    }
    CRenderTarget* pSrcA = Find_RenderTarget(wstrRenderTarget_SrcA);
    CRenderTarget* pSrcB = Find_RenderTarget(wstrRenderTarget_SrcB);
    CRenderTarget* pOutput = Find_RenderTarget(wstrRenderTarget_Target);
    if (nullptr == pSrcA || nullptr == pSrcB || nullptr == pOutput) { return E_FAIL; }

    HRESULT hResult = S_OK;

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT OriginalViewport = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &OriginalViewport);

    _uint iNumViewPort = 1;
    D3D11_VIEWPORT vp = {};
    m_pContext->RSGetViewports(&iNumViewPort, &vp);

    D3D11_TEXTURE2D_DESC OutputDesc = {};
    pOutput->Get_TextureDesc(OutputDesc);

    D3D11_VIEWPORT NewViewport = vp;
    NewViewport.TopLeftX = 0.f;
    NewViewport.TopLeftY = 0.f;
    NewViewport.Width = static_cast<_float>(OutputDesc.Width);
    NewViewport.Height = static_cast<_float>(OutputDesc.Height);
    NewViewport.MinDepth = 0.f;
    NewViewport.MaxDepth = 1.f;

    pOutput->Clear();

    { // Bind RTVs
        _uint iNumRenderTargets = { 1 };
        ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { pOutput->Get_RTV(), };
        m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
        m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargetViews, nullptr);
    }

    m_pContext->RSSetViewports(iNumViewPort, &NewViewport);

    { // Bind const
        _float4x4 matIdentity = {};
        XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());

        if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pSrcA->Bind_ShaderResource(pShader, "g_DiffuseTexture"))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pSrcB->Bind_ShaderResource(pShader, "g_BlurTexture"))) {
            hResult = E_FAIL;
        }
    }

    if (SUCCEEDED(hResult)) {
        if (FAILED(pShader->Begin(ENUM_CLASS(ePass)))) {
            hResult = E_FAIL;
        }
        else {
            pVIBuffer->Bind_Resources();
            pVIBuffer->Render();
        }
    }

    ID3D11ShaderResourceView* const pNullSRV[1] = { nullptr };
    m_pContext->PSSetShaderResources(0, 1, pNullSRV);

    m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pOriginalDSV);
    m_pContext->RSSetViewports(iNumViewPorts, &OriginalViewport);

    SAFE_RELEASE(m_pBackBufferRTV);
    SAFE_RELEASE(m_pOriginalDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Refit_RenderTarget(CVIBuffer_Rect* pVIBuffer, CShader* pShader,
    const _wstring& wstrRenderTargetInput, const _wstring& wstrRenderTargetOutput, SHADER_PASS_DEFERRED ePass)
{
    if (nullptr == pVIBuffer || nullptr == pShader) {
        return E_FAIL;
    }
    CRenderTarget* pInput = Find_RenderTarget(wstrRenderTargetInput);
    CRenderTarget* pOutput = Find_RenderTarget(wstrRenderTargetOutput);
    if (nullptr == pInput || nullptr == pOutput) { return E_FAIL; }

    HRESULT hResult = S_OK;

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pOriginalDSV);

    _uint iNumViewPorts = 1;
    D3D11_VIEWPORT OriginalViewport = {};
    m_pContext->RSGetViewports(&iNumViewPorts, &OriginalViewport);

    _uint iNumViewPort = 1;
    D3D11_VIEWPORT vp = {};
    m_pContext->RSGetViewports(&iNumViewPort, &vp);
    
    D3D11_TEXTURE2D_DESC OutputDesc = {};
    pOutput->Get_TextureDesc(OutputDesc);

    D3D11_VIEWPORT NewViewport = vp;
    NewViewport.TopLeftX = 0.f;
    NewViewport.TopLeftY = 0.f;
    NewViewport.Width = static_cast<_float>(OutputDesc.Width);
    NewViewport.Height = static_cast<_float>(OutputDesc.Height);
    NewViewport.MinDepth = 0.f;
    NewViewport.MaxDepth = 1.f;

    _float2 vResolution = { NewViewport.Width, NewViewport.Height };
    if (FAILED(pShader->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2)))) {
        assert(false);
    }

    pOutput->Clear();

    { // Bind RTVs
        _uint iNumRenderTargets = { 1 };
        ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { pOutput->Get_RTV(), };
        m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
        m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargetViews, nullptr);
    }

    m_pContext->RSSetViewports(iNumViewPort, &NewViewport);

    const _char* pConstantName = { nullptr };
    switch (ePass)
    {
    case Engine::SHADER_PASS_DEFERRED::BLOOM_BLURX:
        pConstantName = "g_BlurTexture";
        break;
    case Engine::SHADER_PASS_DEFERRED::REFIT:
        pConstantName = "g_DiffuseTexture";
        break;
    case Engine::SHADER_PASS_DEFERRED::EMBOSSING:
        pConstantName = "g_DiffuseTexture";
        break;
    case Engine::SHADER_PASS_DEFERRED::BLOOM_BLURY:
        pConstantName = "g_BlurTexture";
        break;
    default:
        hResult = E_FAIL;
        break;
    }

    { // Bind const
        _float4x4 matIdentity = {};
        XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());

        if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pInput->Bind_ShaderResource(pShader, pConstantName))) {
            hResult = E_FAIL;
        }
    }

    if (SUCCEEDED(hResult)) {
        if (FAILED(pShader->Begin(ENUM_CLASS(ePass)))) {
            hResult = E_FAIL;
        }
        else {
            pVIBuffer->Bind_Resources();
            pVIBuffer->Render();
        }
    }

    ID3D11ShaderResourceView* const pNullSRV[1] = { nullptr };
    m_pContext->PSSetShaderResources(0, 1, pNullSRV);

    m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pOriginalDSV);
    m_pContext->RSSetViewports(iNumViewPorts, &OriginalViewport);

    vResolution = { OriginalViewport.Width, OriginalViewport.Height };
    if (FAILED(pShader->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2)))) {
        assert(false);
    }
    SAFE_RELEASE(m_pBackBufferRTV);
    SAFE_RELEASE(m_pOriginalDSV);

    return S_OK;
}

HRESULT CRenderTarget_Manager::Finish_RenderTarget(CVIBuffer_Rect* pVIBuffer, CShader* pShader, const _wstring& wstrRenderTargetOriginal, const _wstring& wstrRenderTargetBloomed, SHADER_PASS_DEFERRED ePass)
{
    if (nullptr == pVIBuffer || nullptr == pShader) {
        return E_FAIL;
    }
    CRenderTarget* pInput = Find_RenderTarget(wstrRenderTargetOriginal);
    CRenderTarget* pBloomed = Find_RenderTarget(wstrRenderTargetBloomed);
    if (nullptr == pInput || nullptr == pBloomed) { 
        return E_FAIL; 
    }

    HRESULT hResult = S_OK;
    
    { // Bind const
        _float4x4 matIdentity = {};
        XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());

        if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", &matIdentity))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pInput->Bind_ShaderResource(pShader, "g_DiffuseTexture"))) {
            hResult = E_FAIL;
        }
        else if (FAILED(pBloomed->Bind_ShaderResource(pShader, "g_BlurTexture"))) {
            hResult = E_FAIL;
        }
    }

    if (SUCCEEDED(hResult)) {
        if (FAILED(pShader->Begin(ENUM_CLASS(ePass)))) {
            hResult = E_FAIL;
        }
        else {
            pVIBuffer->Bind_Resources();
            pVIBuffer->Render();
        }
    }

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

HRESULT CRenderTarget_Manager::Render_RenderTarget_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{

    _float fX = m_iSizeX  * 0.5f;
    _float fY = m_iSizeY * 0.5f;

    for (auto& pRenderTarget : m_RenderTargets)
    {
        if (pRenderTarget.second->Render_Debug(pShader, pVIBuffer, fX, fY, (_float)m_iSizeX, (_float)m_iSizeY) == false){
            continue;
        }

        fX += m_iSizeX;

        if (fX > 1920.f - m_iSizeX * 0.5f)
        {
            fX = m_iSizeX * 0.5f;
            fY += m_iSizeY;
        }
    }

    return S_OK;
}

void CRenderTarget_Manager::RenderTarget_Debuger()
{
    GUI::Begin("RenderTarget Debuger");

    GUI::Spacing();

    GUI::Text("Active Btn -> F10");

    GUI::Spacing();

    GUI::PushItemWidth(80);
    GUI::DragInt("Target Size X" , &m_iSizeX);
    GUI::DragInt("Target Size Y", &m_iSizeY);
    GUI::PopItemWidth();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
    GUI::BeginChild("RTDebugger", { 120.f, 200.f }, ImGuiChildFlags_None, window_flags);
    for (auto& [key, pRenderTarget] : m_RenderTargets)
    {
        _string strKey = CMyTools::ToString(key);
        _string strTargetName = "NOT_STARTED_WITH_Target_";

        _string TargetPrefix = "Target_";
        if (size_t pos = strKey.find(TargetPrefix); pos != string::npos)
        {
            strTargetName = strKey.substr(pos + TargetPrefix.length());
        }

        pRenderTarget->Describe_Entity(strTargetName.c_str());
    }

    GUI::EndChild();

    GUI::End();
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
