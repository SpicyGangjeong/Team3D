#include "pch.h"
#include "Quest_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"

CQuest_Data::CQuest_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CQuest_Data::CQuest_Data(const CQuest_Data& rhs)
	:CGameObject(rhs)
{
}

HRESULT CQuest_Data::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Data::Initialize(void* pArg)
{
	auto* pInfo = static_cast<CUIObject::SPELLINFO*>(pArg);

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError Error = doc.LoadFile("../Bin/Resources/Data/Quest/QuestData.xml");
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
		quest.pQuestName = pQuest->Attribute("QuestName");
		quest.pQuestInfo = pQuest->Attribute("QuestInfo");

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

		QuestInfos.push_back(quest);

		pQuest = pQuest->NextSiblingElement("Quest");
	}

	return S_OK;
}

void CQuest_Data::Priority_Update(_float fTimeDelta)
{
}

void CQuest_Data::Update(_float fTimeDelta)
{
}

void CQuest_Data::Late_Update(_float fTimeDelta)
{
}

HRESULT CQuest_Data::Render()
{
	return S_OK;
}

_vector CQuest_Data::Get_WorldPostion()
{
	return _vector();
}

HRESULT CQuest_Data::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CQuest_Data::Ready_Components(void* pArg)
{
	return S_OK;
}

CQuest_Data* CQuest_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Data* pInstance = new CQuest_Data(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Data::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Data* pInstance = new CQuest_Data(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Data");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Data::Free()
{
	__super::Free();

}

void CQuest_Data::Describe_Entity()
{
}
