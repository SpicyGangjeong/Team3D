#include "pch.h"
#include "Font.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont::Initialize(const _tchar* pFontFilePath)
{
    m_pBatch = new SpriteBatch(m_pContext);
    m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

    return S_OK;
}

HRESULT CFont::Render(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float vScale)
{
    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, 0.f, _float2(0.f, 0.f), vScale);

    m_pBatch->End();

    return S_OK;
}

HRESULT CFont::Render_Rotate(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float Rotation, _float vScale)
{
    _float2 origin = _float2(XMVectorGetX(m_pFont->MeasureString(pText)) * 0.5f, XMVectorGetY(m_pFont->MeasureString(pText)) * 0.5f);

    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, XMConvertToRadians(Rotation), origin, vScale);

    m_pBatch->End();

    return S_OK;
}

HRESULT CFont::Perspective_Render(_matrix View, _matrix Proj, const _tchar* pText, const _fvector& vPosition, _fvector vColor, _float vScale)
{
    // 월드 좌표 -> 화면 좌표 변환
    XMVECTOR projected = XMVector3Project(vPosition, 0.0f, 0.0f, 1280.f, 720.f, 0.0f, 1.0f, Proj, View, XMMatrixIdentity());

    // 화면 좌표 XY만 사용
    XMFLOAT2 screenPos;
    XMStoreFloat2(&screenPos, projected);

    // 화면 밖이면 그리지 않음
    float z = XMVectorGetZ(projected);
    if (z < 0.f || z > 1.f)
        return S_OK;

    // 텍스트 크기 계산 + 중앙 origin 설정
    XMVECTOR textSizeV = m_pFont->MeasureString(pText);
    XMVECTOR origin = XMVectorSet(
        XMVectorGetX(textSizeV) * 0.5f,
        XMVectorGetY(textSizeV) * 0.5f,
        0.f, 0.f
    );

    // 원근에 따른 스케일 보정
    float scale = vScale / (z + 0.0001f);

    m_pBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, nullptr);

    m_pFont->DrawString(m_pBatch, pText, XMVectorSet(screenPos.x, screenPos.y, 0.f, 0.f), vColor, 0.f, origin, scale);

    m_pBatch->End();

    return S_OK;
}

_float CFont::FontSizeX(const _tchar* pText)
{
    return XMVectorGetX(m_pFont->MeasureString(pText));
}

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontFilePath)
{
    CFont* pInstance = new CFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pFontFilePath)))
    {
        MSG_BOX("Failed to Created : CFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFont::Free()
{
    __super::Free();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    Safe_Delete(m_pBatch);
    Safe_Delete(m_pFont);
}
