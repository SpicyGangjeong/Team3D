#include "pch.h"
#include "Stat.h"
#include "StatEvent.h"

CStat::CStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
	, m_UnitInfo{}
{
}

CStat::CStat(const CStat& rhs)
	: CComponent(rhs)
	, m_UnitInfo(rhs.m_UnitInfo)
{
}

HRESULT CStat::Initialize_Prototype(tinyxml2::XMLNode* pChild)
{
	auto pElement = pChild->ToElement();
	m_UnitInfo.pUnit_Name = CMyTools::ToWstring(pElement->Attribute("Name"));
	pElement->QueryFloatAttribute("HP", &m_UnitInfo.fCurrentHp);
	pElement->QueryFloatAttribute("MAXHP", &m_UnitInfo.fMaxHp);
	pElement->QueryFloatAttribute("TARGETHP", &m_UnitInfo.fTargetHp);
	pElement->QueryFloatAttribute("MELEE", &m_UnitInfo.fMelee);
	pElement->QueryFloatAttribute("MAGIC", &m_UnitInfo.fMagic);
	pElement->QueryFloatAttribute("DEFENSE", &m_UnitInfo.fDefense);
	pElement->QueryFloatAttribute("SPEED", &m_UnitInfo.fSpeed);
	pElement->QueryFloatAttribute("AGILITY", &m_UnitInfo.fAgility);
	pElement->QueryIntAttribute("LEVEL", &m_UnitInfo.iLevel);
	pElement->QueryFloatAttribute("EXPERIENCE", &m_UnitInfo.fExprince);
	pElement->QueryFloatAttribute("MAX_EXPERIENCE", &m_UnitInfo.fMaxExprience);
	pElement->QueryIntAttribute("GOLD", &m_UnitInfo.iGold);

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

CStat::UNITINFO CStat::Get_Stat()
{
	return m_UnitInfo;
}

void CStat::Set_Stat(_int _iStatIndex, _float _iAmount)
{

	switch (_iStatIndex)
	{
	case 1:
		m_UnitInfo.fCurrentHp = _iAmount;
		break;
	case 2:
		m_UnitInfo.fMaxHp = _iAmount;
		break;
	case 3:
		m_UnitInfo.fTargetHp = _iAmount;
		break;
	case 4:
		m_UnitInfo.fMelee = _iAmount;
		break;
	case 5:
		m_UnitInfo.fMagic = _iAmount;
		break;
	case 6:
		m_UnitInfo.fDefense = _iAmount;
		break;
	case 7:
		m_UnitInfo.fSpeed = _iAmount;
		break;
	case 8:
		m_UnitInfo.fAgility = _iAmount;
		break;
	case 9:
		m_UnitInfo.iLevel = static_cast<_int>(_iAmount);
		break;
	case 10:
		m_UnitInfo.fExprince = _iAmount;
		break;
	case 11:
		m_UnitInfo.fMaxExprience = _iAmount;
		break;
	case 12:
		m_UnitInfo.iGold = static_cast<_int>(_iAmount);
		break;
	default:
		return;
		break;
	}
}

void CStat::Add_Stat(_int _iStatIndex, _float _iAmount)
{
	switch (_iStatIndex)
	{
	case 1:
		m_UnitInfo.fCurrentHp = _iAmount;
		break;
	case 2:
		m_UnitInfo.fMaxHp = _iAmount;
		break;
	case 3:
		m_UnitInfo.fTargetHp = _iAmount;
		break;
	case 4:
		m_UnitInfo.fMelee = _iAmount;
		break;
	case 5:
		m_UnitInfo.fMagic = _iAmount;
		break;
	case 6:
		m_UnitInfo.fDefense = _iAmount;
		break;
	case 7:
		m_UnitInfo.fSpeed = _iAmount;
		break;
	case 8:
		m_UnitInfo.fAgility = _iAmount;
		break;
	case 9:
		m_UnitInfo.iLevel = static_cast<_int>(_iAmount);
		break;
	case 10:
		m_UnitInfo.fExprince = _iAmount;
		break;
	case 11:
		m_UnitInfo.fMaxExprience = _iAmount;
		break;
	case 12:
		m_UnitInfo.iGold = static_cast<_int>(_iAmount);
		break;
	default:
		return;
		break;
	}
}

void CStat::Add_Hp(_float iAmount)
{
	_float fMaxHp = m_UnitInfo.fMaxHp;
	_float fCurHp = m_UnitInfo.fTargetHp;
	if (fCurHp < fMaxHp) {
		_float iNewHP = fCurHp + iAmount;
		if (iNewHP > fMaxHp) {
			iAmount = fMaxHp - fCurHp;
		}
		Add_Stat(ENUM_CLASS(STAT::CURRENTHP), iAmount);
	}
}

_bool CStat::Get_Damage(_float fDamage)
{
	_bool bResultDead = { false };
	_float fCurHp = Get_Stat().fCurrentHp - fDamage;
	Set_Stat(ENUM_CLASS(STAT::TARGETHP), fDamage);
	if (fCurHp <= 0) { fCurHp = 0; bResultDead = true; }
	Set_Stat(ENUM_CLASS(STAT::CURRENTHP), fCurHp);
	return bResultDead;
	return false;
}
