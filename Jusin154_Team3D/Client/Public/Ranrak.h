#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"

NS_BEGIN(Client)

class CRanrak final : public CMonster
{
	enum class RANRAK_SKILL
	{
		FIREBREATH,
		FIREBALL,
		PULSE,
		TUCKED,
		END
	};

private:
	CRanrak(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRanrak(const CRanrak& Prototype);
	virtual ~CRanrak() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual _vector Get_LockOnPos() override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;

private:
	CCallBack_Monster_Behavior* m_pCallBack_Behavior = { nullptr };
	CCallBack_Monster_HitReport* m_pCallBack_HitReport = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	_float2 m_vStunTimer = { 0.f, 4.f };
	_uint iIndex;
	DAMAGE_INFO m_DamageInfo;

private:

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CRanrak* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

private:
	virtual void Add_FSM();
	virtual void Set_Anim();

	_float m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::END)] = { 20.f ,20.f,20.f,20.f};

	_float m_fDegree = {};
	_float m_fCross = {};

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck();
	void	Behavior_IdleExit();

	void	Behavior_IdleBreakEnter();
	HRESULT Behavior_IdleBreakExitCheck();
	void	Behavior_IdleBreakExit();

	void	Behavior_MoveEnter();
	HRESULT Behavior_MoveExitCheck(_float fTimeDelta);
	void	Behavior_MoveExit();

	void	Behavior_CombatEnter();
	HRESULT Behavior_CombatExitCheck(_float fTimeDelta);
	void	Behavior_CombatExit();

	void	Behavior_HoverEnter();
	HRESULT Behavior_HoverExitCheck(_float fTimeDelta);
	void	Behavior_HoverExit();

	void	Behavior_FlyEnter();
	HRESULT Behavior_FlyExitCheck(_float fTimeDelta);
	void	Behavior_FlyExit();

	void	Behavior_FireBreathEnter();
	HRESULT Behavior_FireBreathExitCheck(_float fTimeDelta);
	void	Behavior_FireBreathExit();

	void	Behavior_FireBallEnter();
	HRESULT Behavior_FireBallExitCheck(_float fTimeDelta);
	void	Behavior_FireBallExit();

	void	Behavior_PulseEnter();
	HRESULT Behavior_PulseExitCheck(_float fTimeDelta);
	void	Behavior_PulseExit();

	void	Behavior_TuckedEnter();
	HRESULT Behavior_TuckedExitCheck(_float fTimeDelta);
	void	Behavior_TuckedExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck(_float fTimeDelta);
	void	Behavior_HitExit();

	void	Behavior_DeadEnter();
	HRESULT Behavior_DeadExitCheck(_float fTimeDelta);
	void	Behavior_DeadExit();
};

NS_END
