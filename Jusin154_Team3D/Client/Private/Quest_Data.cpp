#include "pch.h"
#include "Quest_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"
#include "InfoInstance.h"

CQuest_Data::CQuest_Data()
		: m_pInfoInstance(CInfoInstance::GetInstance()) {
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

		tinyxml2::XMLElement* pObjectives = pQuest->FirstChildElement("Objectives");
		if (pObjectives)
		{
			tinyxml2::XMLElement* pObjective = pObjectives->FirstChildElement("Objective");
			while (pObjective)
			{
				OBJECTIVEINFO  obj{};

				pObjective->QueryIntAttribute("Objective", &obj.iObjectType);
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

_int CQuest_Data::Get_Count()
{
	return m_iQuest_Count;
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
