#include "pch.h"
#include "Stat.h"
#include "StatEvent.h"

CStat::CStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
    ZeroMemory(m_StatArr, sizeof(m_StatArr));
}

CStat::CStat(const CStat& rhs)
    : CComponent(rhs)
{
    ZeroMemory(m_StatArr, sizeof(m_StatArr));
    
    memcpy_s(m_StatArr, sizeof(m_StatArr), rhs.m_StatArr, sizeof(m_StatArr));
    memcpy_s(m_ConditionArr, sizeof(m_ConditionArr), rhs.m_ConditionArr, sizeof(m_ConditionArr));
}

HRESULT CStat::Initialize_Prototype(tinyxml2::XMLNode* pChild)
{
    auto pElement = pChild->ToElement();
    pElement->QueryFloatAttribute("HP",             &m_StatArr[ENUM_CLASS(STAT::HP)]);
    pElement->QueryFloatAttribute("MAXHP",          &m_StatArr[ENUM_CLASS(STAT::MAXHP)]);
    pElement->QueryFloatAttribute("MELEE",          &m_StatArr[ENUM_CLASS(STAT::MELEE)]);
    pElement->QueryFloatAttribute("MAGIC",          &m_StatArr[ENUM_CLASS(STAT::MAGIC)]);
    pElement->QueryFloatAttribute("DEFENSE",        &m_StatArr[ENUM_CLASS(STAT::DEFENSE)]);
    pElement->QueryFloatAttribute("SPEED",          &m_StatArr[ENUM_CLASS(STAT::SPEED)]);
    pElement->QueryFloatAttribute("AGILITY",        &m_StatArr[ENUM_CLASS(STAT::AGILITY)]);
    pElement->QueryFloatAttribute("LEVEL",          &m_StatArr[ENUM_CLASS(STAT::LEVEL)]);
    pElement->QueryFloatAttribute("EXPERIENCE",     &m_StatArr[ENUM_CLASS(STAT::EXPERIENCE)]);
    pElement->QueryFloatAttribute("MAX_EXPERIENCE", &m_StatArr[ENUM_CLASS(STAT::MAX_EXPERIENCE)]);
    pElement->QueryFloatAttribute("GOLD",           &m_StatArr[ENUM_CLASS(STAT::GOLD)]);
    memset(m_ConditionArr, 0, sizeof(m_ConditionArr));

    return S_OK;
}

HRESULT CStat::Initialize(void* pArg)
{
    return S_OK;
}

CStat* CStat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, tinyxml2::XMLNode* pChild)
{
    CStat* pInstance = new CStat(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pChild)))
    {
        MSG_BOX("Failed to Created : CStat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CStat::Clone(void* pArg, class CGameObject* pOwner)
{
    CStat* pInstance = new CStat(*this);
    pInstance->m_pOwner = pOwner;
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CStat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStat::Free()
{
    __super::Free();
}
#ifdef _DEBUG

void CStat::Describe_Entity()
{

}

#endif // _DEBUG

_float CStat::Get_Stat(_int _iStatIndex)
{
    return m_StatArr[_iStatIndex];
}

void CStat::Set_Stat(_int _iStatIndex, _float _iAmount)
{
    m_StatArr[_iStatIndex] = _iAmount;
}

void CStat::Add_Stat(_int _iStatIndex, _float _iAmount)
{
    m_StatArr[_iStatIndex] += _iAmount;
}

_bool CStat::Get_Condition(_uint _iConditionIndex) const
{
    return m_ConditionArr[_iConditionIndex];
}

void CStat::Set_Condition(_uint _iConditionIndex, _bool _Condition)
{
    m_ConditionArr[_iConditionIndex] = _Condition;
}

void CStat::Add_Hp(_float iAmount)
{
    _float fMaxHp = Get_Stat(ENUM_CLASS(STAT::MAXHP));
    _float fCurHp = Get_Stat(ENUM_CLASS(STAT::HP));
    if (fCurHp < fMaxHp) {
        _float iNewHP = fCurHp + iAmount;
        if (iNewHP > fMaxHp) {
            iAmount = fMaxHp - fCurHp;
        }
        Add_Stat(ENUM_CLASS(STAT::HP), iAmount);
    }
}

_bool CStat::Get_Damage(_float fDamage)
{
    _bool bResultDead = { false };
    _float fCurHp = Get_Stat(ENUM_CLASS(STAT::HP)) - fDamage;
    if (fCurHp < 0) { fCurHp = 0; bResultDead = true; }
    Set_Stat(ENUM_CLASS(STAT::HP), fCurHp);
    return bResultDead;
}
