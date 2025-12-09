#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUnit;
class CTransform;
NS_END

NS_BEGIN(Client)

class CMonsterInfo final : public CBase
{
private:
	CMonsterInfo();
	~CMonsterInfo() = default;
public:
	void Update(_float fTimeDelta);
	void Change_Level();

	HRESULT Regist_PlayerAlly(CUnit* pUnit);
	HRESULT Deregist_PlayerAlly(CUnit* pUnit);
	HRESULT Regist_ActiveMonster(class CMonster* pUnit);
	HRESULT Deregist_ActiveMonster(class CMonster* pUnit);

	CUnit* Get_LockOnUnit();
	pair<CUnit*, CTransform*> Get_NearestPlayerAlly(_fvector vPos);

	CMonster* Get_TargetMonster();
private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CMonster*			m_pLockOnMonster = { nullptr };
	list<class CMonster*>	m_ActiveMonsters = {};
	list<CUnit*>			m_PlayerAllies = {};

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	HRESULT	Refresh_LockOnMonsters();
	HRESULT	Refresh_PlayerAllies();
	HRESULT Refresh_ActiveMonsters();


public:
	static CMonsterInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
