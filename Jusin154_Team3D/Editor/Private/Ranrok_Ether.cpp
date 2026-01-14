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
	}
}

void CRanrok::Ether_Update(_float fTimeDelata)
{
	for (size_t i = 0; i < ENUM_CLASS(RANROK_MESH_ORDER::WINGS); i++)
	{
		m_pEtherInfo[i]->Update(fTimeDelata);
	}
}

void CRanrok::Ether_GUI()
{
	GUI::Begin("Ether");

	const char* pRanrokParts[] = { "ETHEREAL_HOT_SPINE" , "ETHEREAL_HOT_WINGS" , "ETHEREAL_HOT_ARMS" , "ETHEREAL_HOT_SHELL" , "ETHEREAL_AURA" , "ETHEREAL_EYES" , "ETHEREAL_WINGS"};
	_int iCurrentItem = m_iCurrentItem;


	if (GUI::Combo("RanrokParts", &iCurrentItem, pRanrokParts, ENUM_CLASS(RANROK_MESH_ORDER::WINGS)))
	{
		m_iCurrentItem = iCurrentItem;
	}

	if (GUI::TreeNode(pRanrokParts[m_iCurrentItem]))
	{
		m_pEtherInfo[m_iCurrentItem]->Describe_Entity();

		GUI::TreePop();
	}


	GUI::InputTextMultiline("FILE PATH", m_szBuffer, sizeof(m_szBuffer), ImVec2(250, 25));

	m_strSavePath = m_szBuffer;

	if (GUI::Button("SAVE"))
	{
		m_pEtherInfo[m_iCurrentItem]->Save_Info(m_strSavePath.c_str());
	}

	if (GUI::Button("LOAD"))
	{
		m_pEtherInfo[m_iCurrentItem]->Load_Info(m_strSavePath.c_str() , LEVEL::EFFECT);
	}

	GUI::End();

}
