#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"

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
	virtual void Set_Anim();

	_float m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::END)] = {};
	_float m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::END)] = { 20.f,15.f, 7.f, 5.f ,10.f};


	_float m_fRushTime = {};
	_float m_fAttackDelay = {};


	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck();
	void	Behavior_IdleExit();

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

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck();
	void	Behavior_HitExit();

};

NS_END
