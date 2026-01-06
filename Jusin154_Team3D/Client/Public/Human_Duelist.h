#pragma once

#include "Client_Define.h"
#include "Unit.h"
#include "Player_ENUM.h"

NS_BEGIN(Client)

class CHuman_Duelist final : public CUnit
{
public:
	typedef struct tagPlayerInitDesc {
		_float4 vPos;
		_float4 vRotQ;
	}PLAYERDESC;
private:
	CHuman_Duelist(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHuman_Duelist(const CHuman_Duelist& Prototype);
	virtual ~CHuman_Duelist() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;
	void Set_SpellHit(_bool bHit) { m_bSpellHit = bHit; }
	void Set_Shield(_bool bShield) { m_bShield = bShield; }
	_matrix Get_WandPos();
private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	class	CCallBack_Playable_Behavior* m_pCallBack_Behavior = { nullptr };
	class	CCallBack_Playable_HitReport* m_pCallBack_HitReport = { nullptr };
	CLight* m_pLightCom = { nullptr };
	LIGHT_DESC LightDesc = {};

	CStat* m_pStat = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_ShaderParameters(_uint iMeshOrder);
	void SetGravity();
	void Add_SpellEvent(_uint AnimIndex, _float fRatio);


public:
	static CHuman_Duelist* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

#pragma region STATE

private:

	virtual void Add_FSM();

	_bool			m_bLookAt = { false };
	_bool			m_bSpellHit = {};
	_bool			m_bShield = { false };
	_float			m_fCross = 0.f;
	_float			m_fabsDir = 0.f;

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck(_float fTimeDelta);
	void	Behavior_IdleExit();

	void	Behavior_CombatEnter();
	HRESULT Behavior_CombatExitCheck();
	void	Behavior_CombatExit();

	void	Behavior_LightAttackEnter();
	HRESULT Behavior_LightAttackExitCheck(_float fTimeDelta);
	void	Behavior_LightAttackExit();

	void	Behavior_SpellEnter();
	HRESULT Behavior_SpellExitCheck();
	void	Behavior_SpellExit();

	void	Behavior_ShieldEnter();
	HRESULT Behavior_ShieldExitCheck();
	void	Behavior_ShieldExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck();
	void	Behavior_HitExit();

#pragma endregion

private:
	class CEffectPool* m_pEffectPool = nullptr;
};

NS_END
