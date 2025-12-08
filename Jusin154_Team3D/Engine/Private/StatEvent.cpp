#include "pch.h"
#include "StatEvent.h"

CStatEvent::CStatEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CStatEvent::CStatEvent(const CStatEvent& Prototype)
    : CComponent(Prototype)
{
}

void CStatEvent::AddEvent(const function<void()> _Equipfunc, const function<void()> _UnEquipfunc)
{
    if (m_EquipEventVector.size() > 6)
        return;

    m_EquipEventVector.push_back(_Equipfunc);
    m_UnEquipEventVector.push_back(_UnEquipfunc);

}

// 팝된 속성이 어떤 속성인지 리턴함
_uint CStatEvent::PopEvent(_uint iIndex)
{
    vector<function<void()>> EquipEventVector = {};
    vector<function<void()>> UnEquipEventVector = {};
    vector<_uint>			 ActiveAbillityIDVec = {};

    _uint iReturnStat(0);

    EquipEventVector = move(m_EquipEventVector);
    UnEquipEventVector = move(m_UnEquipEventVector);
    ActiveAbillityIDVec = move(m_ActiveAbillityIDVec);

    for (size_t i = 0; i < EquipEventVector.size(); i++)
    {
        if (i == iIndex)
        {
            iReturnStat = ActiveAbillityIDVec[i];
            continue;
        }


        m_EquipEventVector.push_back(EquipEventVector[i]);
        m_UnEquipEventVector.push_back(UnEquipEventVector[i]);
        m_ActiveAbillityIDVec.push_back(ActiveAbillityIDVec[i]);
    }

    return iReturnStat;
}

HRESULT CStatEvent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStatEvent::Initialize(void* pArg)
{

    return S_OK;
}

CStatEvent* CStatEvent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStatEvent* pInstance = new CStatEvent(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CStatEvent");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStatEvent::Equip()
{
    for (auto& func : m_EquipEventVector)
    {
        func();
    }
}

void CStatEvent::UnEquip()
{
    for (auto& func : m_UnEquipEventVector)
    {
        func();
    }
}

void CStatEvent::Clear()
{
    m_EquipEventVector.clear();
    m_UnEquipEventVector.clear();
    m_ActiveAbillityIDVec.clear();
}

void CStatEvent::Push_Order(_uint _iId)
{
    m_ActiveAbillityIDVec.push_back(_iId);
}

CComponent* CStatEvent::Clone(void* pArg, class CGameObject* pOwner)
{
    CStatEvent* pInstance = new CStatEvent(*this);
    pInstance->m_pOwner = pOwner;
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CStatEvent");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStatEvent::Free()
{
    __super::Free();
}
#ifdef _DEBUG

void CStatEvent::Describe_Entity()
{

}

#endif // _DEBUG
