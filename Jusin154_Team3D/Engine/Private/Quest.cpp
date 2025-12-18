#include "pch.h"
#include "Quest.h"

CQuest::CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CQuest::CQuest(const CQuest& rhs)
	: CGameObject(rhs)
{
}

HRESULT CQuest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest::Initialize(void* pArg)
{
	QUESTINFO* Info = static_cast<QUESTINFO*>(pArg);
	m_iQuestID = Info->iQuestID;
	m_ObjectiveInfo = Info->ObjectiveInfo;

	return S_OK;
}

_bool CQuest::Update_Objective(_int MonsterID)
{
	for (_int i = 0; i < m_ObjectiveInfo.size(); ++i)
	{
		auto& it = m_ObjectiveInfo[i];

		if (it.bClear == true)
			continue;

		if (it.iTargetID != MonsterID)
			continue;

		++it.iCurrentCount;

		if (it.iCurrentCount >= it.iRequiredCount)
			it.bClear = true;

		break;
	}

	for (_int i = 0; i < m_ObjectiveInfo.size(); ++i)
	{
		if (m_ObjectiveInfo[i].bClear == false)
			return false;
	}

	return true;
}

_int CQuest::Get_QuestID()
{
	return m_iQuestID;
}

void CQuest::Free()
{
	__super::Free();
}
