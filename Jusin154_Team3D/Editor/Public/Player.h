#pragma once

#include "Editor_Define.h"
#include "Unit.h"

NS_BEGIN(Editor)

class CPlayer final : public CUnit
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool Set_Sprint(_bool bSprint) { m_bSprintToggle = bSprint; }

private:
	unordered_map<size_t, CState*> m_States = { };
	size_t m_iStateMask = { 0 };
	_float m_fDirectionRadian = 0.f;

	_bool m_bSprintToggle = { false };
	_bool m_bWalkToggle = { false };


	class CCamPosition_Socket* m_pCamPosition_TopDown_LookPart = { nullptr };
	class CCamPosition_Arm* m_pCamPosition_TopDown_FollowPart = { nullptr };
	class CCamPosition_Shoulder* m_pCamPosition_ShoulderPart = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	class	CCallBack_Playable_Behavior* m_pCallBack_Behavior = { nullptr };
	class	CCallBack_Playable_HitReport* m_pCallBack_HitReport = { nullptr };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;

#pragma region STATE

public:
	virtual void Reset_Sprint() { m_bSprintToggle = false; }
	virtual void Reset_Walk() { m_bWalkToggle = false; }

private:
	void TestKeyInput(_float fTimeDelta);
	virtual void Add_FSM();
	virtual void Set_Anim();

	HRESULT InputAction();
	HRESULT InputMove();
	HRESULT InputSpell();

	STATEANIM::ESTATE m_eSpell = { STATEANIM::END };

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck();
	void	Behavior_IdleExit();

	void	Behavior_MoveEnter();
	HRESULT Behavior_MoveExitCheck();
	void	Behavior_MoveExit();

	void	Behavior_JumpEnter();
	HRESULT Behavior_JumpExitCheck();
	void	Behavior_JumpExit();

	void	Behavior_LandEnter();
	HRESULT Behavior_LandExitCheck();
	void	Behavior_LandExit();

	void	Behavior_DodgeEnter();
	HRESULT Behavior_DodgeExitCheck();
	void	Behavior_DodgeExit();

	void	Behavior_CombatEnter();
	HRESULT Behavior_CombatExitCheck();
	void	Behavior_CombatExit();

#pragma endregion
};

NS_END
