#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"
#include "Enemy_Detection.h"

NS_BEGIN(Client)

class CGoblin_Assassin final : public CMonster
{
	enum class GOBLIN_ASSASSIN_SKILL
	{
		DASH,
		TP,
		END
	};
private:
	CGoblin_Assassin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_Assassin(const CGoblin_Assassin& Prototype);
	virtual ~CGoblin_Assassin() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_OutLine();
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	HRESULT Render_MotionTrail(ID3D11ShaderResourceView* pSRV);
	virtual _vector Get_LockOnPos() override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;

	void Set_Detection(_bool bDetection);
private:
	CCallBack_Monster_Behavior* m_pCallBack_Behavior = { nullptr };
	CCallBack_Monster_HitReport* m_pCallBack_HitReport = { nullptr };
	CEnemy_Detection* m_pDetection = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	_float2 m_vStunTimer = { 0.f, 4.f };
	_uint iIndex;
private:
	class CEffectPool* m_pEffectPool = nullptr;
	DAMAGE_INFO m_DamageInfo;

	_bool m_bDetection = { false };


	class CEffectParts* m_pSmoke = { nullptr };
	class CEffectParts* m_pGoblin_Particle = { nullptr };
	class CEffectParts* m_pGoblin_Particle2 = { nullptr };
	class CGoblin_Assassin_Spector* m_pGoblinSpector = { nullptr };

	ID3D11RasterizerState* m_pRS_Wireframe = { nullptr };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	void HitState_Behavior(_float fTimeDelta);

public:
	static CGoblin_Assassin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

private:
	virtual void Add_FSM();

	_float m_fSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::END)] = { 10.f,15.f };

	_bool	m_bStep = { true };
	_float	m_fTpTime = {};
	_float	m_fAirTime = {};
	_float	m_fLength = {};
	_float	m_fMoveTime = {};
	_bool	m_bFirstMove = {};
	_bool	m_bPos = {};
	_float	m_fAccel = { 3.f};
	_float	m_fTargetSpeed = {60.f};
	_float	m_fSpeed = {};
	_bool	m_bAir = {};
	_float	m_fGravityAmount = {};
	_float	m_fHitTimer = {};
	_float	m_fTumbleTimer = {};

	_float	m_fMoveDecisionTime = 0.f;
	_float	m_fMoveDecisionLimit = 1.2f;

	_float4 m_vDashDir = {};
	_float2 m_vCaptureTimer = { 0.f, 0.1f };

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

	void	Behavior_SlashEnter();
	HRESULT Behavior_SlashExitCheck(_float fTimeDelta);
	void	Behavior_SlashExit();

	void	Behavior_DashEnter();
	HRESULT Behavior_DashExitCheck(_float fTimeDelta);
	void	Behavior_DashExit();

	void	Behavior_BlinkEnter();
	HRESULT Behavior_BlinkExitCheck(_float fTimeDelta);
	void	Behavior_BlinkExit();

	void	Behavior_ShuffleEnter();
	HRESULT Behavior_ShuffleExitCheck(_float fTimeDelta);
	void	Behavior_ShuffleExit();

	void	Behavior_FearEnter();
	HRESULT Behavior_FearExitCheck(_float fTimeDelta);
	void	Behavior_FearExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck(_float fTimeDelta);
	void	Behavior_HitExit();
	
	void	Behavior_DeadEnter();
	HRESULT Behavior_DeadExitCheck(_float fTimeDelta);
	void	Behavior_DeadExit();
};

NS_END
