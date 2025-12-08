#include "pch.h"
#include "Font_Manager.h"
#include "Font.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CFont* pFont = CFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float vScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render(pText, vPosition, vColor, vScale);
}

HRESULT CFont_Manager::Render_Rotate(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float Rotation, _float vScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render_Rotate(pText, vPosition, vColor, Rotation, vScale);
}

HRESULT CFont_Manager::Perspective_Render(_matrix View, _matrix Proj, const _wstring& strFontTag, const _tchar* pText, const _fvector& vPosition, _fvector vColor, _float vScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Perspective_Render(View, Proj, pText, vPosition, vColor, vScale);
}

_float CFont_Manager::FontSizeX(const _wstring& strFontTag, const _tchar* pText)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return 0.f;

    return pFont->FontSizeX(pText);
}

CFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);
    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CFont_Manager(pDevice, pContext);
}

void CFont_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);
    m_Fonts.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
