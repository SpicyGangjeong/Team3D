#pragma once

#include "Editor_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CUnit;
NS_END

NS_BEGIN(Editor)

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

	void Get_LockOnInfo(LOCKON_INFO& Info);
	class CUnit* Get_LockOnUnit();
	pair<CUnit*, CTransform*> Get_NearestPlayerAlly(_fvector vPos);
#pragma endregion
#pragma region MAP_INFO
	HRESULT Load_MapObjects(const _char* pFilePath);
	HRESULT Load_PointLights(const _char* pFilePath);
#pragma endregion
#pragma region Spell_INFO
	HRESULT Load_SpellInfo(const _char* pFilePath);
	SPELL_INFO Get_Spell_Info(_int Spell_Info);
	_int Update_Spell(_int SpellIndex);
	_float Get_CoolTime(_int SpellID);
	void Change_Canvas();

#pragma endregion
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CPlayerInfo* m_pPlayerInfo = { nullptr };
	class CMonsterInfo* m_pMonsterInfo = { nullptr };
	class CMapInfo* m_pMapInfo = { nullptr };
	class CSkill_Data* m_pSkillInfo = { nullptr };
	_int						m_eSpell = ENUM_CLASS(SKILL_TYPE::END);

#ifdef _DEBUG
	_string m_strLog = {};
#endif // _DEBUG

public:
	virtual void Free() override;
};

NS_END
