#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Ranrok_HitReport.h"

NS_BEGIN(Client)

class CRanrok final : public CMonster
{
	enum class RANROK_SKILL
	{
		FIREBREATH,
		FIRESWEEP,
		FIREBALL,
		SWIPE,
		PULSE,
		RUSH,
		END
	};
	enum class RANROK_MESH_ORDER {
		ETHEREAL_HOT_SPINE,
		ETHEREAL_HOT_WINGS,
		ETHEREAL_HOT_ARMS,
		ETHEREAL_HOT_SHELL,
		ETHEREAL_AURA,
		ETHEREAL_EYES,
		ETHEREAL_WINGS,
		WINGS,
		BODY,
		END
	};

	enum class RANROK_PHASE
	{
		PHASE_AIR,
		PHASE_GROUND,
		END
	};
private:
	CRanrok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRanrok(const CRanrok& Prototype);
	virtual ~CRanrok() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_OutLine() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	HRESULT Render_MotionTrail(ID3D11ShaderResourceView* pSRV);
	virtual _vector Get_LockOnPos() override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;

private:
	CCallBack_Monster_Behavior* m_pCallBack_Behavior = { nullptr };
	CCallBack_Ranrok_HitReport* m_pCallBack_HitReport = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	class CEffectPool* m_pEffectPool = nullptr;
	_float2 m_vStunTimer = { 0.f, 4.f };
	_float2 m_vAuraTimer = { 0.f, 0.16f };
	_float2 m_vEtherealTimer = { 0.f, 0.16f };
	_uint iIndex;
	DAMAGE_INFO m_DamageInfo;
	_bool m_bCollisionPlayer = { false };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	HRESULT Render_Nonblend();
	HRESULT Render_Blend();
	void MoveTo(_float fTimeDelta);
	HRESULT Load_RanrokPos(const _char* pFilePath);
public:
	static CRanrok* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

private:
	virtual void Add_FSM();

	_int m_ePhase = ENUM_CLASS(RANROK_PHASE::PHASE_AIR);

	_float m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::END)] = { 40.f,40.f ,8.f,40.f,40.f,40.f};

	_float m_fTuckedTime = {};
	_bool m_bFireBurst = { false };
	_bool m_bTucked = {false};
	_bool m_bHoverDash = { false };

	vector<vector<_float4>> m_Points;
	_int m_iCurrentPoint = 0;
	_int m_iCurrentFlow = 0;

	_float m_fAroundAngle = 0.f;
	_float m_fAroundSpeed = 1.2f;
	_float m_fAroundRadius = 40.f;
	_float m_fAroundTime = {};
	_float m_fRushTime = {};
	_float m_fHeadAimWeight = {};
	_float m_fPrevHpRatio = {};
	_int   m_iBreathRand = {};

	_float2 m_vCaptureTimer = { 0.f, 0.3f };

	class CEffect_Container* m_pRanrok_Point = { nullptr };
	_float					 m_fTuckedSpeed = { 90.f };


	class CEffectParts* m_pLeftSmoke = { nullptr };
	class CEffectParts* m_pRightSmoke = { nullptr };
	class CEffectParts* m_pBottomSmoke = { nullptr };

	class CEffectParts* m_pRightPt = { nullptr };
	class CEffectParts* m_pLeftPt = { nullptr };

	class CTrailObject* m_pRightEye_Trail = { nullptr };
	class CTrailObject* m_pLeftEye_Trail = { nullptr };


	const _float4x4* m_pLeftEye_BoneMat = { nullptr };
	const _float4x4* m_pRightEye_BoneMat = { nullptr };

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck();
	void	Behavior_IdleExit();

	void	Behavior_IdleBreakEnter();
	HRESULT Behavior_IdleBreakExitCheck(_float fTimeDelta);
	void	Behavior_IdleBreakExit();

	void	Behavior_MoveEnter();
	HRESULT Behavior_MoveExitCheck(_float fTimeDelta);
	void	Behavior_MoveExit();

	void	Behavior_CombatEnter();
	HRESULT Behavior_CombatExitCheck(_float fTimeDelta);
	void	Behavior_CombatExit();

	void	Behavior_GroundEnter();
	HRESULT Behavior_GroundExitCheck(_float fTimeDelta);
	void	Behavior_GroundExit();

	void	Behavior_HoverEnter();
	HRESULT Behavior_HoverExitCheck(_float fTimeDelta);
	void	Behavior_HoverExit();

	void	Behavior_FlyEnter();
	HRESULT Behavior_FlyExitCheck(_float fTimeDelta);
	void	Behavior_FlyExit();

	void	Behavior_FireBreathEnter();
	HRESULT Behavior_FireBreathExitCheck(_float fTimeDelta);
	void	Behavior_FireBreathExit();

	void	Behavior_FireSweepEnter();
	HRESULT Behavior_FireSweepExitCheck(_float fTimeDelta);
	void	Behavior_FireSweepExit();

	void	Behavior_FireBallEnter();
	HRESULT Behavior_FireBallExitCheck(_float fTimeDelta);
	void	Behavior_FireBallExit();

	void	Behavior_SwipeEnter();
	HRESULT Behavior_SwipeExitCheck(_float fTimeDelta);
	void	Behavior_SwipeExit();

	void	Behavior_SkillEnter();
	HRESULT Behavior_SkillExitCheck(_float fTimeDelta);
	void	Behavior_SkillExit();

	void	Behavior_RushEnter();
	HRESULT Behavior_RushExitCheck(_float fTimeDelta);
	void	Behavior_RushExit();

	void	Behavior_PulseEnter();
	HRESULT Behavior_PulseExitCheck(_float fTimeDelta);
	void	Behavior_PulseExit();

	void	Behavior_TuckedEnter();
	HRESULT Behavior_TuckedExitCheck(_float fTimeDelta);
	void	Behavior_TuckedExit();

	void	Behavior_LandEnter();
	HRESULT Behavior_LandExitCheck(_float fTimeDelta);
	void	Behavior_LandExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck(_float fTimeDelta);
	void	Behavior_HitExit();

	void	Behavior_DeadEnter();
	HRESULT Behavior_DeadExitCheck(_float fTimeDelta);
	void	Behavior_DeadExit();


	virtual _bool IsHitStateDisabled() override;
	virtual _bool IsHitSpellDisabled() override;
};

NS_END
