#include "pch.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "GameInstance.h"

CCamera_Manager::CCamera_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBase(),
    m_pDevice{ pDevice },
    m_pContext{ pContext }
{
    SAFE_ADDREF(m_pDevice);
    SAFE_ADDREF(m_pContext);
}

HRESULT CCamera_Manager::Add_Camera(_uint iLevel, CCamera* pCamera, const _wstring& wstrCameraKey)
{
    if (nullptr != Find_Camera(iLevel, wstrCameraKey)) {
        return E_FAIL;
    }
    if (nullptr == pCamera) {
        return E_FAIL;
    }

    m_Cameras[iLevel].emplace(wstrCameraKey, pCamera);
    SAFE_ADDREF(pCamera);

    return S_OK;
}

HRESULT CCamera_Manager::Bind_Camera(_uint iLevel, const _wstring& wstrCameraKey, _bool bIgnorePriority)
{
    CCamera* pNextCamera = Find_Camera(iLevel, wstrCameraKey);
    if (nullptr == pNextCamera) {
        return E_FAIL;
    }
    pair<_float4, _float3> pairTransitionInfo = { };
    if (nullptr != m_pCurrentMainCamera) {
        if (false == bIgnorePriority && false == pNextCamera->IsImportantThan(m_pCurrentMainCamera)) {
            return E_ACCESSDENIED;
        }
        m_pCurrentMainCamera->DeActive_Camera(pairTransitionInfo);
        SAFE_RELEASE(m_pCurrentMainCamera);
    }
    else {
        XMStoreFloat4(&pairTransitionInfo.first, XMQuaternionIdentity());
    }

    {
        m_pCurrentMainCamera = pNextCamera;
        m_wstrCurrentCameraKey = wstrCameraKey;
        m_fFar = *m_pCurrentMainCamera->Get_CurrentFar();
    }
    {
        m_pCurrentMainCamera->Active_Camera(pairTransitionInfo);
        SAFE_ADDREF(m_pCurrentMainCamera);
    }

    return S_OK;
}

CCamera* CCamera_Manager::Get_Camera(_uint iLevel, const _wstring& strCameraKey)
{
    CCamera* pOut = Find_Camera(iLevel, strCameraKey); 
    return pOut;
}

HRESULT CCamera_Manager::IsBinded_Camera(const _wstring& wstrCameraKey)
{
    if (m_wstrCurrentCameraKey == wstrCameraKey) {
        return S_OK;
    }
    return E_FAIL;
}

_vector CCamera_Manager::Get_CameraLook()
{
    return m_pCurrentMainCamera->Get_Component<CTransform>()->Get_State(STATE::LOOK);
}

_float CCamera_Manager::Get_CameraFov()
{
    return m_pCurrentMainCamera->Get_Fov();
}

void CCamera_Manager::Force_CamPosition(_fvector vPos)
{
    if (nullptr == m_pCurrentMainCamera)
        return;

    m_pCurrentMainCamera->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSetW(vPos, 1.f));
}

const _float* CCamera_Manager::Get_CurrentCameraFar()
{
    return &m_fFar;
}

HRESULT CCamera_Manager::Initialize(_uint iLevelNumber)
{
    m_iLevelNumber = iLevelNumber;
    m_Cameras = new unordered_map<_wstring, CCamera*>[iLevelNumber];
    return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(_uint iLevel, const _wstring& wstrCameraKey)
{
    auto    iter = m_Cameras[iLevel].find(wstrCameraKey);

    if (iter == m_Cameras[iLevel].end()) {
        return nullptr;
    }

    return iter->second;
}

HRESULT CCamera_Manager::Clear_Cameras(_uint iLevel)
{
    if (nullptr != m_pCurrentMainCamera) {
        for (pair<const _wstring, CCamera*>& pairCamera : m_Cameras[iLevel]) {
            if (m_pCurrentMainCamera == pairCamera.second) {
                pair<_float4, _float3> pairTransitionInfo = { };
                m_pCurrentMainCamera->DeActive_Camera(pairTransitionInfo);
                SAFE_RELEASE(m_pCurrentMainCamera);
            }
        }
    }
    for (auto& iter : m_Cameras[iLevel]) {
        SAFE_RELEASE(iter.second);
    }
    m_Cameras[iLevel].clear();
    return S_OK;
}

HRESULT CCamera_Manager::Release_Camera(_uint iLevel, const _wstring& wstrCameraKey)
{
    auto    iter = m_Cameras[iLevel].find(wstrCameraKey);

    if (iter == m_Cameras[iLevel].end()) {
        return E_FAIL;
    }
    if (nullptr != m_pCurrentMainCamera) {
        if (m_pCurrentMainCamera == (*iter).second) {
            pair<_float4, _float3> pairTransitionInfo = { };
            m_pCurrentMainCamera->DeActive_Camera(pairTransitionInfo);
            SAFE_RELEASE(m_pCurrentMainCamera);
        }
    }

    SAFE_RELEASE((*iter).second);
    m_Cameras[iLevel].erase(iter);

    return S_OK;
}

CCamera_Manager* CCamera_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelNumber)
{
    CCamera_Manager* pInstance = new CCamera_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize(iLevelNumber))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CCamera_Manager::Free()
{
    __super::Free();

    for (_uint i = 0; i < m_iLevelNumber; ++i) {
        Clear_Cameras(i);
    }

    SAFE_RELEASE(m_pCurrentMainCamera);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pContext);

    Safe_Delete_Array(m_Cameras);
}
