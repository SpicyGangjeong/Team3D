#include "pch.h"
#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}




HRESULT CLight_Manager::Initialize(_uint iNumLevels)
{
    m_iLevelNumber = iNumLevels;

    m_Lights = new list<class CLight*>[m_iLevelNumber];
    m_FrameLights = new list<class CLight*>[m_iLevelNumber];

    return S_OK;
}

void CLight_Manager::Add_Light(_uint _iCurrentLevel, CLight* _pLight)
{
    if (nullptr == _pLight)
        return;

    m_Lights[_iCurrentLevel].push_back(_pLight);

    SAFE_ADDREF(_pLight);

}

void CLight_Manager::Add_Light_Group(_uint _iCurrentLevel, CLight* _pLight)
{
    if (nullptr == _pLight)
        return;

    m_FrameLights[_iCurrentLevel].push_back(_pLight);

    SAFE_ADDREF(_pLight);
}

void CLight_Manager::Delete_Light(_uint _iCurrentLevel, CLight* _pLight)
{
    for (auto iter = m_Lights[_iCurrentLevel].begin(); iter != m_Lights[_iCurrentLevel].end();)
    {
        if (*iter == _pLight)
        {
            SAFE_RELEASE(_pLight);

            iter = m_Lights[_iCurrentLevel].erase(iter);
            return;
        }
        else
        {
            iter++;
        }
    }
}

void CLight_Manager::Light_Clear(_uint _iCurrentLevel)
{
    for (auto& pLight : m_Lights[_iCurrentLevel])
    {
        SAFE_RELEASE(pLight);
    }

    m_Lights[_iCurrentLevel].clear();
}

const LIGHT_DESC* CLight_Manager::Get_Light_Info(_uint _iCurrentLevel, _uint _iLightIndex)
{

    if (_iLightIndex >= m_Lights[_iCurrentLevel].size())
        return nullptr;

    auto iter = m_Lights[_iCurrentLevel].begin();

    for (size_t i = 0; i < _iLightIndex; i++)
    {
        iter++;
    }

    return (*iter)->Get_LightDesc();
}

_uint CLight_Manager::Get_NumLight(_uint _iCurrentLevel)
{
    if(true == m_Lights[_iCurrentLevel].empty())
		return 0;

    return static_cast<_uint>(m_Lights[_iCurrentLevel].size());
}


HRESULT CLight_Manager::Render_Lights(_uint iCurrentLevel, CShader* pShader, CVIBuffer* pVIBuffer)
{
    for (auto& pLight : m_Lights[iCurrentLevel])
    {
        pLight->Render(pShader, pVIBuffer);
    }

    for (auto& pLight : m_FrameLights[iCurrentLevel])
    {
        pLight->Render(pShader, pVIBuffer);
        
        SAFE_RELEASE(pLight);

    }

    m_FrameLights[iCurrentLevel].clear();


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

    for (size_t i = 0; i < m_iLevelNumber; i++)
    {
        for (auto& Light : m_Lights[i])
        {
            SAFE_RELEASE(Light);
        } 
        
        m_Lights[i].clear();

    }

    Safe_Delete_Array(m_Lights);

    for (size_t i = 0; i < m_iLevelNumber; i++)
    {
        for (auto& Light : m_FrameLights[i])
        {
            SAFE_RELEASE(Light);
        }

        m_FrameLights[i].clear();

    }

    Safe_Delete_Array(m_FrameLights);

}
