#pragma once

#include "Editor_Define.h"
#include "Unit.h"

NS_BEGIN(Editor)

class CPlayer final : public CUnit
{
public:
	struct InputCondition
	{
		FSMSTATE::ESTATE state;
		function<_bool()> checker;
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool Set_Sprint(_bool bSprint) { m_bSprintToggle = bSprint; }

private:
	unordered_map<size_t, CState*> m_States = { };
	size_t m_iStateMask = { 0 };
	_float m_fDirectionRadian = 0.f;

	vector<InputCondition> m_InputConditions;
	_bool m_bSprintToggle = { false };
	_bool m_bWalkToggle = { false };

	class CCamPosition_Player* m_pCamPosition_TopDown_LookPart = { nullptr };
	class CCamPosition_Arm* m_pCamPosition_TopDown_FollowPart = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

	HRESULT InputSystem();
	HRESULT InputSkill();
	HRESULT InputMove();

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
	

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;

#pragma region STATE
private:
	virtual void Add_FSM();
	virtual void Set_Anim();
	_bool IsSprint();
	_bool IsWalk();
public:
	virtual void Reset_Sprint() { m_bSprintToggle = false; }
	virtual void Reset_Walk() { m_bWalkToggle = false; }


#pragma endregion




};

NS_END
