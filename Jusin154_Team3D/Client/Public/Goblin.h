#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"

NS_BEGIN(Client)

class CGoblin final : public CMonster
{
	enum class GOBLIN_SKILL
	{
		SWING,
		THROW,
		TP,
		END
	};
private:
	CGoblin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin(const CGoblin& Prototype);
	virtual ~CGoblin() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
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
private:
	class CEffectPool* m_pEffectPool = nullptr;

	class CEffectParts* m_pSmoke = { nullptr };
	class CEffectParts* m_pGoblin_Particle = { nullptr };
	class CEffectParts* m_pGoblin_Particle2 = { nullptr };
	class CGoblin_Spector* m_pGoblinSpector = { nullptr };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CGoblin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

private:
	virtual void Add_FSM();
	virtual void Set_Anim();

	_float m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::END)] = { 5.f,2.f,15.f };

	_bool	m_bStep = { false };
	_float	m_fTpTime = {};
	_float	m_fAirTime = {};
	_vector m_vOriginPos = {};
	_float	m_fLength = {};
	_float	m_fMoveTime = {};
	_bool	m_bFirstMove = {};

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


	void	Behavior_SwingEnter();
	HRESULT Behavior_SwingExitCheck(_float fTimeDelta);
	void	Behavior_SwingExit();

	void	Behavior_ThrowEnter();
	HRESULT Behavior_ThrowExitCheck(_float fTimeDelta);
	void	Behavior_ThrowExit();

	void	Behavior_BlinkEnter();
	HRESULT Behavior_BlinkExitCheck(_float fTimeDelta);
	void	Behavior_BlinkExit();

	void	Behavior_ShuffleEnter();
	HRESULT Behavior_ShuffleExitCheck(_float fTimeDelta);
	void	Behavior_ShuffleExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck(_float fTimeDelta);
	void	Behavior_HitExit();
	
	void	Behavior_DeadEnter();
	HRESULT Behavior_DeadExitCheck(_float fTimeDelta);
	void	Behavior_DeadExit();
};

NS_END
