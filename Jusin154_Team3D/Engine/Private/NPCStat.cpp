#include "pch.h"
#include "NPCStat.h"

CNPCStat::CNPCStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
	, m_NpcInfo{}
{
}

CNPCStat::CNPCStat(const CNPCStat& rhs)
	: CComponent(rhs)
	, m_NpcInfo{ rhs.m_NpcInfo }
{
}

HRESULT CNPCStat::Initialize_Prototype(tinyxml2::XMLNode* pChild)
{
	auto pElement = pChild->ToElement();
	pElement->QueryIntAttribute("ID", &m_NpcInfo.iID);
	m_NpcInfo.pNpc_Name = CMyTools::ToWstring(pElement->Attribute("Name"));
	pElement->QueryIntAttribute("TAG", &m_NpcInfo.iNpcType);
	pElement->QueryFloatAttribute("SizeX", &m_NpcInfo.fFontPos);
	return S_OK;
}

HRESULT CNPCStat::Initialize(void* pArg)
{
	return S_OK;
}

CNPCStat* CNPCStat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, tinyxml2::XMLNode* pChild)
{
	CNPCStat* pInstance = new CNPCStat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pChild)))
	{
		MSG_BOX("Failed to Created : CNPCStat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNPCStat::Clone(void* pArg, class CGameObject* pOwner)
{
	CNPCStat* pInstance = new CNPCStat(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPCStat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPCStat::Free()
{
	__super::Free();
}

#ifdef _DEBUG
void CNPCStat::Describe_Entity()
{
}
#endif // _DEBUG
CNPCStat::NPCINFO CNPCStat::Get_Stat()
{
	return m_NpcInfo;
}

