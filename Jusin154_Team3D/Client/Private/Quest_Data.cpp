#include "pch.h"
#include "Quest_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"
#include "InfoInstance.h"
#include "QuestInstance.h"
#include "GameInstance.h"

CQuest_Data::CQuest_Data()
	: m_pInfoInstance(CInfoInstance::GetInstance()),
	m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CQuest_Data::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	Load_QuestInfo("../Bin/Resources/Data/Quest/QuestData.xml");
	return S_OK;
}

void CQuest_Data::Update(_float fTimeDelta)
{

}

HRESULT CQuest_Data::Load_QuestInfo(const _char* pFilePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError Error = doc.LoadFile(pFilePath);
	if (Error != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* pQuestInfo = doc.FirstChildElement("QuestInfo");
	if (!pQuestInfo)
		return E_FAIL;

	tinyxml2::XMLElement* pQuestData = pQuestInfo->FirstChildElement("QuestData");
	if (!pQuestData)
		return E_FAIL;

	tinyxml2::XMLElement* pQuest = pQuestData->FirstChildElement("Quest");
	while (pQuest)
	{
		QUESTINFO quest{};

		pQuest->QueryIntAttribute("QuestID", &quest.iQuestID);
		pQuest->QueryIntAttribute("QuestType", &quest.iType);
		quest.pQuestName = CMyTools::ToWstring(pQuest->Attribute("QuestName"));
		quest.pQuestInfo = CMyTools::ToWstring(pQuest->Attribute("QuestInfo"));
		pQuest->QueryIntAttribute("AcceptState", &quest.iAcceptState);

		tinyxml2::XMLElement* pObjectives = pQuest->FirstChildElement("Objectives");
		if (pObjectives)
		{
			tinyxml2::XMLElement* pObjective = pObjectives->FirstChildElement("Objective");
			while (pObjective)
			{
				OBJECTIVEINFO  obj{};
				_int Clear = 0;
				pObjective->QueryIntAttribute("Clear", &Clear);
				obj.bClear = Clear != 0;
				pObjective->QueryIntAttribute("TargetID", &obj.iTargetID);
				obj.pQuestInfo = CMyTools::ToWstring(pObjective->Attribute("QuestInfo"));
				pObjective->QueryIntAttribute("CurrentCount", &obj.iCurrentCount);
				pObjective->QueryIntAttribute("RequiredCount", &obj.iRequiredCount);

				quest.ObjectiveInfo.push_back(obj);

				pObjective = pObjective->NextSiblingElement("Objective");
			}
		}

		tinyxml2::XMLElement* pRewards = pQuest->FirstChildElement("Rewards");
		if (pRewards)
		{
			tinyxml2::XMLElement* pReward = pRewards->FirstChildElement("Reward");
			while (pReward)
			{
				REWARDSINFO reward{};

				pReward->QueryIntAttribute("Type", &reward.iRewardType);
				pReward->QueryIntAttribute("Value", &reward.iRewardID);

				quest.RewardsInfo.push_back(reward);

				pReward = pReward->NextSiblingElement("Reward");
			}
		}
		m_iQuest_Count++;
		QuestInfos.push_back(quest);

		pQuest = pQuest->NextSiblingElement("Quest");
	}

	return S_OK;
}

QUESTINFO CQuest_Data::Get_Quest(_int QuestType, _int QuestID)
{
	switch (QuestType)
	{
	case 0:
		return QuestInfos[QuestID];
	case 1:
		return m_QuestAcceptedInfos[QuestID].QeustInfo;
	case 2:
		return m_QuestEndInfos[QuestID];
	default:
		return QuestInfos[QuestID];
	}

}

const vector<QUESTINFO>& CQuest_Data::Get_AllQuest() const
{
	return QuestInfos;
}

const vector<QUESTINFO>& CQuest_Data::Get_ClearQuest() const
{
	return	m_QuestEndInfos;
}

vector<QUESTINFO>& CQuest_Data::Get_AcceptQuest()
{
	m_QuestAcceptedInfo.reserve(m_QuestAcceptedInfos.size());

	for (const auto& q : m_QuestAcceptedInfos)
		m_QuestAcceptedInfo.push_back(q.QeustInfo);

	return m_QuestAcceptedInfo;
}

_int CQuest_Data::Get_Count(_int Index)
{
	switch (Index)
	{
	case 0:
		return m_iQuest_Count;

	case 1:
		return m_iAcceptQeustCount;

	case 2:
		return m_iClearQeustCount;

	default:
		return -1;
	}
}

_int CQuest_Data::Set_AcceptQuest(_int Index)
{
	if (Index == -1)
		return -1;
	if (QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::ACCEPTED))
	{
		return 1;
	}
	if (QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::CLEARED))
	{
		return 2;
	}

	HRESULT hr = m_pGameInstance->Add_GameObject_ToLayer<CQuestInstance>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, &QuestInfos[Index], nullptr, reinterpret_cast<CQuestInstance**>(&m_pQuestInstance));
	if (FAILED(hr))
	{
		return -1;
	}

	QuestInfos[Index].iAcceptState = ENUM_CLASS(QUESTSTATE::ACCEPTED);

	m_pInfoInstance->Add_Event(TEXT("MonsterDead"), [this](void* p) {this->Update_AcceptQuest(*reinterpret_cast<_int*>(p)); });
	m_AcceptedInfo.QeustData = m_pQuestInstance;
	m_AcceptedInfo.QeustInfo = QuestInfos[Index];
	m_QuestAcceptedInfos.push_back(m_AcceptedInfo);
	m_iAcceptQeustCount++;
	return 0;
}

void CQuest_Data::Set_ClearQuest(_int Index)
{
	if (QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::NONE) || QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::CLEARED))
		return;

	QuestInfos[Index].iAcceptState = ENUM_CLASS(QUESTSTATE::CLEARED);
	m_QuestEndInfos.push_back(m_QuestAcceptedInfos[Index].QeustInfo);
	m_iClearQeustCount++;
	if (m_iAcceptQeustCount >= 0)
		m_iAcceptQeustCount--;
}

void CQuest_Data::Update_AcceptQuest(_int MonsterID)
{
	for (auto it = m_QuestAcceptedInfos.begin(); it != m_QuestAcceptedInfos.end(); )
	{
		auto* quest = static_cast<CQuestInstance*>(it->QeustData);
		auto& info = it->QeustInfo;

		for (auto& obj : info.ObjectiveInfo)
		{
			if (obj.iTargetID != MonsterID)
				continue;

			if (obj.iCurrentCount < obj.iRequiredCount)
			{
				obj.iCurrentCount++;
			}
		}

		_bool QuestClear = quest->Update_Objective(MonsterID);

		if (QuestClear)
		{
			Set_ClearQuest(quest->Get_QuestID());
			it = m_QuestAcceptedInfos.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CQuest_Data* CQuest_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Data* pInstance = new CQuest_Data();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
	{
		MSG_BOX("Failed to Created : CQuest_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Data::Free()
{
	__super::Free();
}
