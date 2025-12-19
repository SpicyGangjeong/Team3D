#include "pch.h"
#include "Quest_Data.h"
#include "GameObject.h"
#include "UIObject.h"
#include "tinyxml2.h"
#include "QuestInstance.h"

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
		quest.pQuestName = CMyTools::ToWstring(pQuest->Attribute("QuestName"));
		quest.pQuestInfo = CMyTools::ToWstring(pQuest->Attribute("QuestInfo"));

		tinyxml2::XMLElement* pObjectives = pQuest->FirstChildElement("Objectives");
		if (pObjectives)
		{
			tinyxml2::XMLElement* pObjective = pObjectives->FirstChildElement("Objective");
			while (pObjective)
			{
				OBJECTIVEINFO  obj{};
				_int Clear = 0;
				pObjective->QueryIntAttribute("Objective", &Clear);
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

	Set_AcceptQuest(0);

	return S_OK;
}

void CQuest_Data::Priority_Update(_float fTimeDelta)
{
}

void CQuest_Data::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_0))
		Update_AcceptQuest(0);
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

CQuest_Data::QUESTINFO CQuest_Data::Get_Quest(_int QuestID)
{
	return QuestInfos[QuestID];
}

const vector<CQuest_Data::QUESTINFO>& CQuest_Data::Get_AllQuest() const
{
	return CQuest_Data::QuestInfos;
}

const vector<CQuest_Data::QUESTINFO>& CQuest_Data::Get_ClearQuest() const
{
	return	CQuest_Data::m_QuestEndInfos;
}

const vector<CQuest_Data::QUESTINFO>& CQuest_Data::Get_AcceptQuest() const
{
	return CQuest_Data::m_QuestAcceptedInfos;
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuestInstance>
		(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, &QuestInfos[Index], this, reinterpret_cast <CQuestInstance**>(&m_pQuestInstance))))
	{
		return E_FAIL;
	}

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

	SAFE_RELEASE(m_pVIBufferCom);
}

void CQuest_Data::Describe_Entity()
{
}
