#include "pch.h"
#include "Ranrok.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "Ranrok_EtherInfo.h"

void CRanrok::Ether_Create()
{
	for (size_t i = 0; i < ENUM_CLASS(RANROK_MESH_ORDER::WINGS); i++)
	{
		m_pEtherInfo[i] = CRanrok_EtherInfo::Create(m_pDevice, m_pContext);
		m_pEtherInfo[i]->Set_Visible(false);
	}

	m_pEtherInfo[ENUM_CLASS(RANROK_MESH_ORDER::ETHEREAL_EYES)]->Load_Info("../Bin/Resources/Data/Effect/Ranrok/Ranrok_Ether/Eye" , static_cast<LEVEL>(g_iStaticLevel));
	m_pEtherInfo[ENUM_CLASS(RANROK_MESH_ORDER::ETHEREAL_AURA)]->Load_Info("../Bin/Resources/Data/Effect/Ranrok/Ranrok_Ether/Aura", static_cast<LEVEL>(g_iStaticLevel));
	m_pEtherInfo[ENUM_CLASS(RANROK_MESH_ORDER::ETHEREAL_WINGS)]->Load_Info("../Bin/Resources/Data/Effect/Ranrok/Ranrok_Ether/Wing", static_cast<LEVEL>(g_iStaticLevel));
}

void CRanrok::Ether_Update(_float fTimeDelata)
{
	for (size_t i = 0; i < ENUM_CLASS(RANROK_MESH_ORDER::WINGS); i++)
	{
		m_pEtherInfo[i]->Update(fTimeDelata);
	}
}
