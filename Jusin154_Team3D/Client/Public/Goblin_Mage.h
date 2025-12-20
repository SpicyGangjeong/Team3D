#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"
#include "Enemy_Detection.h"

NS_BEGIN(Client)

class CGoblin_Mage final : public CMonster
{
	enum class GOBLIN_SKILL
	{
		SPELL,
		LIGHTATTACK,
		SUSTAIN,
		TP,
		END
	};
private:
	CGoblin_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_Mage(const CGoblin_Mage& Prototype);
	virtual ~CGoblin_Mage() = default;

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
	//CEnemy_Detection* m_pDetection = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };

	_float2 m_vStunTimer = { 0.f, 4.f };
	_uint iIndex;

	DAMAGE_INFO m_DamageInfo;


	class CEffectParts* m_pGoblin_Orb = { nullptr };
	class CGoblin_Spector* m_pGoblinSpector = { nullptr };

	_float m_fHoverTime = 0.1f;
	_float m_fHoverHeight = 0.1f; 
	_float m_fHoverSpeed = 5.f;
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CGoblin_Mage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

private:
	virtual void Add_FSM();
	virtual void Set_Anim();
private:
	class CEffectPool* m_pEffectPool = { nullptr };

	_bool m_bDetection = { false };

	_float m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::END)] = { 20.f,5.f,30.f,15.f };

	_bool	m_bStep = { false };
	_float	m_fTpTime = {};
	_float	m_fAirTime = {};
	_vector m_vOriginPos = {};
	_float	m_fLength = {};
	_bool	m_bShield = { false };
	_bool	m_bPos = {};
	_float	m_fAccel = { 3.f };
	_float	m_fTargetSpeed = { 60.f };
	_float	m_fSpeed = {};
	_bool	m_bAir = {};
	_float	m_fGravityAmount = {};

#ifdef _DEBUG
	_bool   m_isDebugMode = {};
#endif

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck();
	void	Behavior_IdleExit();

	void	Behavior_IdleBreakEnter();
	HRESULT Behavior_IdleBreakExitCheck();
	void	Behavior_IdleBreakExit();

	void	Behavior_MoveEnter();
	HRESULT Behavior_MoveExitCheck();
	void	Behavior_MoveExit();

	void	Behavior_CombatEnter();
	HRESULT Behavior_CombatExitCheck(_float fTimeDelta);
	void	Behavior_CombatExit();

	void	Behavior_SpellEnter();
	HRESULT Behavior_SpellExitCheck(_float fTimeDelta);
	void	Behavior_SpellExit();

	void	Behavior_LightAttackEnter();
	HRESULT Behavior_LightAttackExitCheck(_float fTimeDelta);
	void	Behavior_LightAttackExit();

	void	Behavior_SustainEnter();
	HRESULT Behavior_SustainExitCheck(_float fTimeDelta);
	void	Behavior_SustainExit();

	void	Behavior_BlinkEnter();
	HRESULT Behavior_BlinkExitCheck(_float fTimeDelta);
	void	Behavior_BlinkExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck(_float fTimeDelta);
	void	Behavior_HitExit();

	void	Behavior_DeadEnter();
	HRESULT Behavior_DeadExitCheck(_float fTimeDelta);
	void	Behavior_DeadExit();

};

NS_END
