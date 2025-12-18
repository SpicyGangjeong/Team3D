#include "pch.h"
#include "Quest_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"
#include "InfoInstance.h"
#include "QuestInstance.h"
#include "GameInstance.h"

CQuest_Data::CQuest_Data()
		: m_pInfoInstance(CInfoInstance::GetInstance()) ,
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
				pObjective->QueryIntAttribute("RequiredCount", &obj.iRequiredCount);
				pObjective->QueryIntAttribute("CurrentCount", &obj.iCurrentCount);

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

				pReward->QueryIntAttribute("RequiredCount", &reward.iRewardType);
				pReward->QueryIntAttribute("CurrentCount", &reward.iRewardID);

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

QUESTINFO CQuest_Data::Get_Quest(_int QuestID)
{
	return QuestInfos[QuestID];
}

const vector<QUESTINFO>& CQuest_Data::Get_AllQuest() const
{
	return QuestInfos;
}

const vector<QUESTINFO>& CQuest_Data::Get_ClearQuest() const
{
	return	m_QuestEndInfos;
}

const vector<QUESTINFO>& CQuest_Data::Get_AcceptQuest() const
{
	return m_QuestAcceptedInfos;
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

HRESULT CQuest_Data::Set_AcceptQuest(_int Index)
{
	if (QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::ACCEPTED) || QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::CLEARED))
		return S_OK;
	QuestInfos[Index].iAcceptState = ENUM_CLASS(QUESTSTATE::ACCEPTED);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuestInstance>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, &QuestInfos[Index], nullptr, reinterpret_cast<CQuestInstance**>(&m_pQuestInstance))))
	{
		return E_FAIL;
	}
	m_pInfoInstance->Add_Event(TEXT("MonsterDead"), [this](void* p) {this->Update_AcceptQuest(*reinterpret_cast<_int*>(p)); });
	m_QuestAcceptedInfoObject.push_back(m_pQuestInstance);
	m_QuestAcceptedInfos.push_back(QuestInfos[Index]);
	m_iAcceptQeustCount++;
	return S_OK;
}

void CQuest_Data::Set_ClearQuest(_int Index)
{
	if (QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::NONE) || QuestInfos[Index].iAcceptState == ENUM_CLASS(QUESTSTATE::CLEARED))
		return;

	QuestInfos[Index].iAcceptState = ENUM_CLASS(QUESTSTATE::CLEARED);
	m_QuestEndInfos.push_back(QuestInfos[Index]);
	m_iClearQeustCount++;
	if (m_iAcceptQeustCount >= 0)
		m_iAcceptQeustCount--;
}

void CQuest_Data::Update_AcceptQuest(_int MonsterID)
{
	m_QuestAcceptedInfoObject.erase(remove_if(m_QuestAcceptedInfoObject.begin(), m_QuestAcceptedInfoObject.end(),
		[MonsterID, this](auto& it) {
			CQuestInstance* instance = static_cast<CQuestInstance*>(it);
			if (static_cast<CQuestInstance*>(it)->Update_Objective(MonsterID) == true)
			{
				Set_ClearQuest(static_cast<CQuestInstance*>(it)->Get_QuestID());
				return true;
			}
			return false;
		}),
		m_QuestAcceptedInfoObject.end()
	);
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
