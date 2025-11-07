#include "pch.h"
#include "Level.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelID)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
    , m_iID{ iLevelID }
{
    SAFE_ADDREF(m_pGameInstance);
    SAFE_ADDREF(m_pDevice);
    SAFE_ADDREF(m_pContext);
}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}

void CLevel::Free()
{
    __super::Free();

    SAFE_RELEASE(m_pGameInstance);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pContext);
}