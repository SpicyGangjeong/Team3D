#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CUnit;
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
	void Release_Information();
	LEVEL Get_RestartLevel();
public:
	void Update(_float fTimeDelta);
	void Change_Level();

#pragma region PLAYER_INFO
	void Update_CameraCoordinateSystem(_float3& vLook, _float3& vRight);
	pair<_float3, _float3> Get_CameraCoordinateSystem();
#pragma endregion
#pragma region MONSTER_INFO
	HRESULT Regist_PlayerAlly(CUnit* pUnit);
	HRESULT Deregist_PlayerAlly(CUnit* pUnit);
	HRESULT Regist_ActiveMonster(class CMonster* pUnit);
	HRESULT Deregist_ActiveMonster(class CMonster* pUnit);

	class CUnit* Get_LockOnUnit();
	pair<CUnit*, CTransform*> Get_NearestPlayerAlly(_fvector vPos);
#pragma endregion
#pragma region MAP_INFO
	HRESULT Load_MapObjects(const _char* pFilePath);
	HRESULT Load_LightElements(const _char* pFilePath);
#pragma endregion
#pragma region Spell_INFO
	HRESULT Load_SpellInfo(const _char* pFilePath);
	SPELL_INFO Get_Spell_Info(_int Spell_Info);
	_int Update_Spell(_int SpellIndex);
	_float Get_CoolTime(_int SpellID);
	void Change_Canvas();
	void Key_Input(_uint Input); // 플레이어 키 입력

	void Add_Event(_wstring EventName, function<void(void*)> Event);
	void Event_CallBack(_wstring EventName, void* pArg = nullptr);
#pragma endregion
private:
	CGameInstance*				m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CPlayerInfo*			m_pPlayerInfo = { nullptr };
	class CMonsterInfo*			m_pMonsterInfo = { nullptr };
	class CMapInfo*				m_pMapInfo = { nullptr };
	class CSkill_Data*			m_pSkillInfo= { nullptr };
	_uint						m_eInput = ENUM_CLASS(KEYINPUT::END);
	_int						m_eSpell = ENUM_CLASS(SKILL_TYPE::END);

	// 임시로 이벤트 1개 만들어 둠
	multimap<_wstring, function<void(void*)>> UI_Event;
#ifdef _DEBUG
	_string m_strLog = {};
#endif // _DEBUG

public:
	virtual void Free() override;
};

NS_END
