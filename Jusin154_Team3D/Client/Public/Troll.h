#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Troll_HitReport.h"

NS_BEGIN(Client)

class CTroll final : public CMonster
{
	enum class TROLL_SKILL
	{
		RUSH,
		THROWROCK,
		SLAM,
		SWING,
		BACKHAND_SWING,
		END
	};

private:
	CTroll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTroll(const CTroll& Prototype);
	virtual ~CTroll() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_OutLine() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual _vector Get_LockOnPos() override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;
private:
	void    Troll_Trail_Visible(_bool isTrailVisible);

private:
	CCallBack_Monster_Behavior* m_pCallBack_Behavior = { nullptr };
	CCallBack_Troll_HitReport* m_pCallBack_HitReport = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	PSX::PxSweepBufferN<12> m_SweepBufferHammer = {};
	PSX::PxSweepBufferN<12> m_SweepBufferGrip = {};
	const _float4* m_pHammerPos = { nullptr };
	const _float4* m_pHammerGripPos = { nullptr };
	_bool m_bCollisionPlayer = { false };
	_float4 m_vStartHammerPos = { };
	_float4 m_vStartGripPos = { };
	_float2 m_vStunTimer = { 0.f, 4.f };
	_uint iIndex;
	DAMAGE_INFO m_DamageInfo;

	class CEffectParts* m_pRight_Smoke = { nullptr };
	class CEffectParts* m_pLeft_Smoke = { nullptr };
	class CEffectParts* m_pTroll_Particle = { nullptr };
	class CEffectParts* m_pTroll_Particle2 = { nullptr };

	class CTrailObject* m_pLeftTrail = { nullptr };
	class CTrailObject* m_pRightTrail = { nullptr };
	class CTrailObject* m_pWeaponTrail = { nullptr };

	const _float4x4* m_pLeftHand_BoneMat = { nullptr };
	const _float4x4* m_pRightHand_BoneMat = { nullptr };
	const _float4x4* m_pWeapon_BoneMat = { nullptr };
private:
	class CEffectPool* m_pEffectPool = nullptr;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CTroll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

private:
	virtual void Add_FSM();
	void SlamHit(Engine::_bool& bHit, Engine::_float4* m_pStartPos, bool& retFlag);
	void CheckHammerHits(_uint& iHitCount, vector<PSX::PxSweepHit>& pxHits);

	_float m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::END)] = { 20.f,20.f, 20.f, 20.f ,20.f };

	_float m_fRushTime = {};
	_float m_fAttackDelay = {};


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

	void	Behavior_RushEnter();
	HRESULT Behavior_RushExitCheck(_float fTimeDelta);
	void	Behavior_RushExit();

	void	Behavior_ThrowEnter();
	HRESULT Behavior_ThrowExitCheck(_float fTimeDelta);
	void	Behavior_ThrowExit();

	void	Behavior_SwingEnter();
	HRESULT Behavior_SwingExitCheck(_float fTimeDelta);
	void	Behavior_SwingExit();
	void	SwingHit(_bool& bPlayerHit);

	void	Behavior_SlamEnter();
	HRESULT Behavior_SlamExitCheck(_float fTimeDelta);
	void	Behavior_SlamExit();
	void	SlamHit(_bool& bPlayerHit);

	void	Behavior_BackHandSwingEnter();
	HRESULT Behavior_BackHandSwingExitCheck(_float fTimeDelta);
	void	Behavior_BackHandSwingExit();

	void	Behavior_StunEnter();
	HRESULT Behavior_StunExitCheck(_float fTimeDelta);
	void	Behavior_StunExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck();
	void	Behavior_HitExit();

	void	Behavior_DeadEnter();
	HRESULT Behavior_DeadExitCheck(_float fTimeDelta);
	void	Behavior_DeadExit();

	virtual _bool IsHitSpellDisabled() override;
};

NS_END
