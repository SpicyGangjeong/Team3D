#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CUnit;
class CState;
class CStat;
class CNPCStat;
class CUIObject;
NS_END

NS_BEGIN(Client)

class CInfoInstance final : public CBase
{
DECLARE_SINGLETON(CInfoInstance)
private:
	CInfoInstance();
	virtual ~CInfoInstance() = default;
public:
	HRESULT Initialize_Information(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	HRESULT Late_Initialize(); // GamePlay Initialize 할 때
	void Release_Information();
	LEVEL Get_RestartLevel();
public:
	void Update(_float fTimeDelta);
	void Change_Level();

#pragma region PLAYER_INFO
	CStat* Get_PlayerStatPtr();
	void Update_CameraCoordinateSystem(_float3& vLook, _float3& vRight);
	pair<_float3, _float3> Get_CameraCoordinateSystem();
	_float Player_Damage();
	void Set_Damage(_float fDamage);
	void Set_PlayerPos(_vector Position);
	_vector Get_PalyerPos();
#pragma endregion
#pragma region MONSTER_INFO
	HRESULT Regist_PlayerAlly(CUnit* pUnit);
	HRESULT Deregist_PlayerAlly(CUnit* pUnit);
	HRESULT Regist_ActiveMonster(class CMonster* pUnit);
	HRESULT Deregist_ActiveMonster(class CMonster* pUnit);

	void Get_LockOnInfo(LOCKON_INFO& Info);
	pair<CUnit*, CTransform*> Get_NearestPlayerAlly(_fvector vPos);
	class CMonster* Get_TargetMonster();
#pragma endregion
#pragma region MAP_INFO
	HRESULT Load_MapObjects(const _char* pFilePath, const _wchar* pLayerTag);
	HRESULT Load_LightElements(const _char* pFilePath, const _wchar* pLayerTag);
	HRESULT Load_InteractableElements(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_WaterElemet(const _char* pFileName);
	HRESULT Load_DoorElemet(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_ChestElemet(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_WorldDecal(const _char* pFileName, const _wchar* pLayerTag);
	HRESULT Load_PointLights(const _char* pFileName, const _wchar* pLayerTag);
#pragma endregion
#pragma region Spell_INFO
	HRESULT Load_SpellInfo(const _char* pFilePath);
	SPELL_INFO Get_Spell_Info(_int Spell_Info);
	_int Update_Spell(_int SpellIndex);
	_float Get_CoolTime(_int SpellID);
	void Change_Canvas();
	void Key_Input(_uint Input); // 플레이어 키 입력
	void Mouse_Input(_uint Input); // 플레이어 키 입력
	void Set_UISTATE(UI_STATE eState);
	UI_STATE Get_UISTATE();
	_float Get_Spell_Damage(_int Index);
	void Add_Event(_wstring EventName, function<void(void*)> Event);
	void Event_CallBack(_wstring EventName, void* pArg = nullptr);
#pragma endregion
#pragma region QuestINFO
	QUESTINFO Get_Quest(_int QuestType, _int QuestID);
	_int Get_Quest_Count(_int Index);
	const vector<QUESTINFO>& Get_AllQuest() const;
	const vector<QUESTINFO>& Get_ClearQuest() const;
	const vector<QUESTINFO>& Get_AcceptQuest() const;
	HRESULT Set_AcceptQuest(_int Index);
#pragma endregion
#pragma region SpellLearn_Data
	const SPELLLEARNINFO& Get_SpellLearn(_int Index) const;
	_int Get_SpellLearnIndex();
#pragma endregion
#pragma region Dialogue_Font
	void Set_Font(void* pArg);
#pragma endregion
	HRESULT Regist_ActiveInteractive(class CMapElement_Interactable* pInteractive);
	HRESULT Deregist_ActiveInteractive(class CMapElement_Interactable* pInteractive);

	HRESULT ActiveAt_Interactive(_fvector vPosition);
#pragma endregion
private:
	CGameInstance*				m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CPlayerInfo*			m_pPlayerInfo = { nullptr };
	class CMonsterInfo*			m_pMonsterInfo = { nullptr };
	class CMapInfo*				m_pMapInfo = { nullptr };
	class CSkill_Data*			m_pSkillInfo = { nullptr };
	class CQuest_Data*			m_pQuestInfo= { nullptr };
	class CInteractiveInfo*		m_pInteractiveInfo =  { nullptr };
	class CDamage_Font*			m_pDamage_Font =  { nullptr };
	class CSpellLearn_Data*		m_pSpellLearn_Data =  { nullptr };
	class CDialogue_Font*		m_pDialogue_Font = { nullptr };

	_vector						m_pPlayerPos{};

	_uint						m_eInput = ENUM_CLASS(KEYINPUT::END);
	_int						m_eSpell = ENUM_CLASS(SKILL_TYPE::END);

	_float						m_fDamage{};
	UI_STATE					m_eUI_State = { UI_STATE::END };

	// 임시로 이벤트 1개 만들어 둠
	multimap<_wstring, function<void(void*)>> UI_Event;
#ifdef _DEBUG
	_string m_strLog = {};
#endif // _DEBUG

private:
	HRESULT Stat_FileLoad(const _char* pDirectoryPath);
	HRESULT NPC_FileLoad(const _char* pDirectoryPath);

public:
	virtual void Free() override;
};

NS_END
