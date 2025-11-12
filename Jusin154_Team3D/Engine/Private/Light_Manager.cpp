#include "pch.h"
#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::On_Light(_uint iLevel, const _wstring& wstrLightKey, const LIGHT_DESC& LightDesc, class CLight** ppOut)
{
    list<CLight*>* pLights = Find_Lights(iLevel, wstrLightKey);
    if (nullptr != pLights) {
        if (*(*pLights).front() == LightDesc) { // 같은 종류, 같은 위치의 광원은 추가하지 않음
            return E_FAIL;
        }
    }
    else if (nullptr == pLights) {
        pLights = new list<CLight*>();
        m_pLights[iLevel].emplace(wstrLightKey, pLights);
    }
    CLight* pLight = CLight::Create(LightDesc);
    if (nullptr == pLight) {
        return E_FAIL;
    }
    if (nullptr != ppOut) {
        *ppOut = pLight;
    }
    pLights->push_back(pLight);
    return S_OK;
}

HRESULT CLight_Manager::Off_Light(_uint iLevel, const _wstring& wstrLightKey)
{
    map<_wstring, list<CLight*>*>::iterator iter = m_pLights[iLevel].find(wstrLightKey);
    if (iter == m_pLights[iLevel].end()) {
        return E_FAIL;
    }
    for (auto& pLight : *(*iter).second) {
        SAFE_RELEASE(pLight);
    }
    Safe_Delete((*iter).second);
    (m_pLights[iLevel]).erase(iter);
    return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer* pVIBuffer)
{
    for (_uint i = 0; i < m_iNumLevel; ++i) {
        for (auto& pairLight : m_pLights[i])
        {
            for (auto& pLight : *pairLight.second) {
                if (FAILED((pLight)->Render(pShader, pVIBuffer))) {
                    return E_FAIL;
                }
            }
        }
    }

    return S_OK;
}

HRESULT CLight_Manager::Change_Level(_uint iLevel)
{
    for (auto& pairLight : m_pLights[iLevel]) {
        for (auto& pLight : *pairLight.second) {
            SAFE_RELEASE(pLight);
        }
        Safe_Delete(pairLight.second);
    } m_pLights[iLevel].clear();
    return S_OK;
}

list<CLight*>* CLight_Manager::Find_Lights(_uint iLevel, const _wstring& wstrLightKey)
{
    map<_wstring, list<CLight*>*>::iterator iter = m_pLights[iLevel].find(wstrLightKey);
    if (iter == m_pLights[iLevel].end()) {
        return nullptr;
    }
    return iter->second;
}

HRESULT CLight_Manager::Initialize(_uint iLevelNumber)
{
    m_iNumLevel = iLevelNumber;
    m_pLights = new map<_wstring, list<CLight*>*>[iLevelNumber];
    return S_OK;
}

CLight_Manager* CLight_Manager::Create(_uint iLevelNumber)
{
    CLight_Manager* pInstance = new CLight_Manager();
    if (FAILED(pInstance->Initialize(iLevelNumber))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CLight_Manager::Free()
{
    __super::Free();

    for (_uint i = 0; i < m_iNumLevel; ++i) {
        for (auto& pairLight : m_pLights[i]) {
            for (auto& pLight : *pairLight.second) {
                SAFE_RELEASE(pLight);
            }
            Safe_Delete(pairLight.second);
        } m_pLights[i].clear();
    } Safe_Delete_Array(m_pLights);
}
