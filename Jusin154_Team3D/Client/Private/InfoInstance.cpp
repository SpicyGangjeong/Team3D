#include "pch.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "MapInfo.h"
#include "MonsterInfo.h"
#include "PlayerInfo.h"
#include "InteractiveInfo.h"
#include "Skill_Data.h"
#include "Quest_Data.h"
#include "Damage_Font.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CInfoInstance)

CInfoInstance::CInfoInstance()
{
}


void CInfoInstance::Update(_float fTimeDelta)
{
	m_pPlayerInfo->Update(fTimeDelta);
	m_pMonsterInfo->Update(fTimeDelta);
	m_pMapInfo->Update(fTimeDelta);
	m_pSkillInfo->Update(fTimeDelta);
	m_pInteractiveInfo->Update(fTimeDelta);
}	

void CInfoInstance::Change_Level()
{
	m_pPlayerInfo->Change_Level();
	m_pMonsterInfo->Change_Level();
	m_pMapInfo->Change_Level();
	m_pSkillInfo->Change_Level();
	m_pInteractiveInfo->Change_Level();
}

CStat* CInfoInstance::Get_PlayerStatPtr()
{
	return m_pPlayerInfo->Get_PlayerStatPtr();
}

void CInfoInstance::Update_CameraCoordinateSystem(_float3& vLook, _float3& vRight)
{
	m_pPlayerInfo->Update_CameraCoordinateSystem(vLook, vRight);
}

pair<_float3, _float3> CInfoInstance::Get_CameraCoordinateSystem()
{
	return m_pPlayerInfo->Get_CameraCoordinateSystem();
}

_float CInfoInstance::Player_Damage()
{
	return m_fDamage;
}

void CInfoInstance::Set_Damage(_float fDamage)
{
	m_fDamage = fDamage;
	if (m_pSkillInfo != nullptr)
		m_pSkillInfo->Update_Damage(fDamage);
}

#pragma region MONSTER_INFO
HRESULT CInfoInstance::Regist_PlayerAlly(CUnit* pUnit)
{
	return m_pMonsterInfo->Regist_PlayerAlly(pUnit);
}

HRESULT CInfoInstance::Deregist_PlayerAlly(CUnit* pUnit)
{
	if (nullptr == s_pInstance || nullptr == m_pMonsterInfo) {
		return S_OK; // 게임 종료 된 상태
	}
	return m_pMonsterInfo->Deregist_PlayerAlly(pUnit);
}

HRESULT CInfoInstance::Regist_ActiveMonster(CMonster* pUnit)
{
	return m_pMonsterInfo->Regist_ActiveMonster(pUnit);
}

HRESULT CInfoInstance::Deregist_ActiveMonster(CMonster* pUnit)
{
	if (nullptr == s_pInstance || nullptr == m_pMonsterInfo) {
		return S_OK; // 게임 종료 된 상태
	}
	return m_pMonsterInfo->Deregist_ActiveMonster(pUnit);
}

void CInfoInstance::Get_LockOnInfo(LOCKON_INFO& Info)
{
	Info.pUnit = m_pMonsterInfo->Get_LockOnUnit();
	Info.pInteractive = m_pInteractiveInfo->Get_LockOnUnit();
}

pair<CUnit*, CTransform*> CInfoInstance::Get_NearestPlayerAlly(_fvector vPos)
{
	return m_pMonsterInfo->Get_NearestPlayerAlly(vPos);
}

CMonster* CInfoInstance::Get_TargetMonster()
{
	return m_pMonsterInfo->Get_TargetMonster();
}

#pragma endregion

#pragma region MAP_INFO
HRESULT CInfoInstance::Load_MapObjects(const _char* pFilePath)
{
	return m_pMapInfo->Load_MapObjects(pFilePath);
}
HRESULT CInfoInstance::Load_LightElements(const _char* pFilePath)
{
	return m_pMapInfo->Load_LightElements(pFilePath);
}
HRESULT CInfoInstance::Load_InteractableElements(const _char* pFileName)
{
	return m_pMapInfo->Load_InteractableElements(pFileName);
}
HRESULT CInfoInstance::Load_WaterElemet(const _char* pFileName)
{
	return m_pMapInfo->Load_WaterElemet(pFileName);
}
HRESULT CInfoInstance::Load_DoorElemet(const _char* pFileName)
{
	return m_pMapInfo->Load_DoorElemet(pFileName);
}
HRESULT CInfoInstance::Load_ChestElemet(const _char* pFileName)
{
	return m_pMapInfo->Load_ChestElemet(pFileName);
}
#pragma endregion

#pragma region SPELL_INFO
HRESULT CInfoInstance::Load_SpellInfo(const _char* pFilePath)
{
	return m_pSkillInfo->Load_SpellInfo(pFilePath);
}
SPELL_INFO CInfoInstance::Get_Spell_Info(_int Spell_Info)
{
	return m_pSkillInfo->Get_Info(Spell_Info);
}

_int CInfoInstance::Update_Spell(_int SpellIndex)
{
	return m_pSkillInfo->Update_Spell(SpellIndex);
}

_float CInfoInstance::Get_CoolTime(_int SpellID)
{
	return m_pSkillInfo->Get_CoolTime(SpellID);
}
void CInfoInstance::Change_Canvas()
{

}

void CInfoInstance::Key_Input(_uint Input)
{
	m_eInput = Input;

	switch (m_eInput)
	{
	case ENUM_CLASS(KEYINPUT::INPUT_1):
		Event_CallBack(TEXT("Spell"), &m_eInput);
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_2):
		Event_CallBack(TEXT("Spell"), &m_eInput);
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_3):
		Event_CallBack(TEXT("Spell"), &m_eInput);
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_4):
		Event_CallBack(TEXT("Spell"), &m_eInput);
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_T):
		if (m_eUI_State == UI_STATE::GAMEPLAYER)
		{
			m_eUI_State = UI_STATE::SPELL;
			Event_CallBack(TEXT("Canvas_Change"), &m_eUI_State);
		}
		else if (m_eUI_State == UI_STATE::SPELL)
		{
			m_eUI_State = UI_STATE::GAMEPLAYER;
			Event_CallBack(TEXT("Canvas_Change"), &m_eUI_State);
		}
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_X):
		Event_CallBack(TEXT("Ancient_Magic_Throw"));
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_G):
		Event_CallBack(TEXT("Use_Potion"));
		break;
	case ENUM_CLASS(KEYINPUT::INPUT_TAB):
		if (m_eUI_State == UI_STATE::GAMEPLAYER)
		{
			m_eUI_State = UI_STATE::QUEST_CANVES;
			Event_CallBack(TEXT("Canvas_Change"), &m_eUI_State);
		}
		else if (m_eUI_State == UI_STATE::QUEST_CANVES)
		{
			m_eUI_State = UI_STATE::GAMEPLAYER;
			Event_CallBack(TEXT("Canvas_Change"), &m_eUI_State);
		}
		break;

	default:
		break;
	}
}

void CInfoInstance::Mouse_Input(_uint Input)
{
	m_eInput = Input;
	_bool bHover = false;
	switch (m_eInput)
	{
	case ENUM_CLASS(KEYINPUT::DIM_RBUTTON_UP):
		bHover = false;
		Event_CallBack(TEXT("CameraLockOn"), &bHover);
		break;
	case ENUM_CLASS(KEYINPUT::DIM_RBUTTON_DOWN):
		bHover = true;
		Event_CallBack(TEXT("CameraLockOn"), &bHover);
		break;
	default:
		break;
	}


}

void CInfoInstance::Set_UISTATE(UI_STATE eState)
{
	m_eUI_State = eState;
}

UI_STATE CInfoInstance::Get_UISTATE()
{
	return m_eUI_State;
}

_float CInfoInstance::Get_Spell_Damage(_int Index)
{
	return m_pSkillInfo->Get_Spell_Damage(Index);
}

void CInfoInstance::Add_Event(_wstring EventName, function<void(void*)> Event)
{
	UI_Event.emplace(EventName, Event);
}

void CInfoInstance::Event_CallBack(_wstring EventName, void* pArg)
{
	auto range = UI_Event.equal_range(EventName);
	for (auto it = range.first; it != range.second; ++it)
	{
		it->second(pArg);
	}
}

QUESTINFO CInfoInstance::Get_Quest(_int QuestType, _int QuestID)
{
	return m_pQuestInfo->Get_Quest(QuestType, QuestID);
}

_int CInfoInstance::Get_Quest_Count(_int Index)
{
	return m_pQuestInfo->Get_Count(Index);
}

const vector<QUESTINFO>& CInfoInstance::Get_AllQuest() const
{
	return m_pQuestInfo->Get_AllQuest();
}

const vector<QUESTINFO>& CInfoInstance::Get_ClearQuest() const
{
	return m_pQuestInfo->Get_ClearQuest();
}

const vector<QUESTINFO>& CInfoInstance::Get_AcceptQuest() const
{
	return m_pQuestInfo->Get_AcceptQuest();
}

HRESULT CInfoInstance::Set_AcceptQuest(_int Index)
{
	return m_pQuestInfo->Set_AcceptQuest(Index);
}

HRESULT CInfoInstance::Regist_ActiveInteractive(CMapElement_Interactable* pInteractive)
{
	if (nullptr == s_pInstance || nullptr == m_pInteractiveInfo) {
		return S_OK; // 게임 종료 된 상태
	}
	return m_pInteractiveInfo->Regist_ActiveInteractive(pInteractive);
}

HRESULT CInfoInstance::Deregist_ActiveInteractive(CMapElement_Interactable* pInteractive)
{
	if (nullptr == s_pInstance || nullptr == m_pInteractiveInfo) {
		return S_OK; // 게임 종료 된 상태
	}
	return m_pInteractiveInfo->Deregist_ActiveInteractive(pInteractive);
}

#pragma endregion
LEVEL CInfoInstance::Get_RestartLevel()
{
	return LEVEL::GAMEPLAY;
}

HRESULT CInfoInstance::Initialize_Information(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContext;
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);

	m_pMapInfo = CMapInfo::Create(pDevice, pContext);
	if (nullptr == m_pMapInfo) {
		return E_FAIL;
	}

	{ // 순서 고정
#pragma region STAT PROTOTYPE
		if (FAILED(Stat_FileLoad("../Bin/Resources/Data/Stat/Stat.xml"))) {
			return E_FAIL;
		}
#pragma endregion
		m_pPlayerInfo = CPlayerInfo::Create(pDevice, pContext);
		if (nullptr == m_pPlayerInfo) {
			return E_FAIL;
		}
	}
	m_pMonsterInfo = CMonsterInfo::Create(pDevice, pContext);
	if (nullptr == m_pMonsterInfo) {
		return E_FAIL;
	}
	m_pSkillInfo = CSkill_Data::Create(pDevice, pContext);
	if (nullptr == m_pSkillInfo) {
		return E_FAIL;
	}
	m_pQuestInfo = CQuest_Data::Create(pDevice, pContext);
	if (nullptr == m_pQuestInfo) {
		return E_FAIL;
	}
	m_pInteractiveInfo = CInteractiveInfo::Create(pDevice, pContext);
	if (nullptr == m_pInteractiveInfo) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInfoInstance::Stat_FileLoad(const _char* pDirectoryPath)
{
	filesystem::path pathStatFile = pDirectoryPath;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError Error = doc.LoadFile(pathStatFile.string().c_str());
	if (Error != tinyxml2::XML_SUCCESS) {
		return E_FAIL;
	}

	tinyxml2::XMLElement* pStatInfo = doc.FirstChildElement("StatInfo");
	if (!pStatInfo) {
		return E_FAIL;
	}

	_uint iNumChild = pStatInfo->ChildElementCount();
	tinyxml2::XMLNode* pChild = pStatInfo->FirstChildElement();
	for (_uint i = 0; i < iNumChild; ++i) {
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, CMyTools::ToWstring(pChild->Value()),
			CStat::Create(m_pDevice, m_pContext, pChild)))) {
			return E_FAIL;
		}
		pChild = pChild->NextSiblingElement();
	}

	return S_OK;
}
void CInfoInstance::Release_Information()
{
	UI_Event.clear();

	DestroyInstance();


	SAFE_RELEASE(m_pMapInfo);
	SAFE_RELEASE(m_pPlayerInfo);
	SAFE_RELEASE(m_pMonsterInfo);
	SAFE_RELEASE(m_pSkillInfo);
	SAFE_RELEASE(m_pQuestInfo);
	SAFE_RELEASE(m_pInteractiveInfo);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}

void CInfoInstance::Free()
{
	__super::Free();
	UI_Event.clear();

}
