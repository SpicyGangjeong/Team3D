#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

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
#ifdef _DEBUG
	void Render_CameraCoordinateSystem();
#endif // _DEBUG
	_bool   Set_Sprint(_bool bSprint) { m_bSprintToggle = bSprint; }
	_matrix Get_WandPos();
private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CUnit* m_pLockOnMonster = { nullptr };
	_float m_fDirectionRadian = 0.f;

	_bool m_bSprintToggle = { false };
	_bool m_bWalkToggle = { false };
	_bool m_bHoverToggle = { true };

	_float3 m_vCameraLookDir = { 0.f, 0.f, 1.f, };
	_float3 m_vCameraRightDir = { 1.f, 0.f, 0.f };

	class CCamPosition_Socket* m_pCamPosition_TopDown_LookPart = { nullptr };
	class CCamPosition_Arm* m_pCamPosition_TopDown_FollowPart = { nullptr };
	class CCamPosition_Shoulder* m_pCamPosition_ShoulderPart = { nullptr };

	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	class	CCallBack_Playable_Behavior* m_pCallBack_Behavior = { nullptr };
	class	CCallBack_Playable_HitReport* m_pCallBack_HitReport = { nullptr };
	CLight* m_pLightCom = { nullptr };
	LIGHT_DESC LightDesc = {};

	class CModel* m_pBroomModel = { nullptr };
	class CTransform* m_pBroomTransform = { nullptr };
	class CBroom* m_pBroom = { nullptr };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	void ReLockOnTarget();
	void SetGravity();

	void Update_CameraCoordinateSystem();
#ifdef _DEBUG
	unique_ptr<BasicEffect> m_BasicEffect;
	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_Batch;
#endif // _DEBUG


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

#pragma region STATE

public:
	virtual void Reset_Sprint() { m_bSprintToggle = false; }
	virtual void Reset_Walk() { m_bWalkToggle = false; }



private:
	void TestKeyInput(_float fTimeDelta);
	virtual void Add_FSM();
	virtual void Set_Anim();

	function<void()> m_InputAction = nullptr;


	_float3 m_OffsetPos = {};
	_float m_fAmount = { 1.f };
	_float m_fInputTime = {};
	_bool m_bRatio = { false };

	HRESULT InputAction();
	HRESULT InputMove();
	HRESULT InputKeyUpMove();
	HRESULT InputSpell();
	HRESULT InputAim();

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck(_float fTimeDelta);
	void	Behavior_IdleExit();

	void	Behavior_MoveEnter();
	HRESULT Behavior_MoveExitCheck(_float fTimeDelta);
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

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck();
	void	Behavior_HitExit();

	void	Behavior_Broom_RideEnter();
	HRESULT Behavior_Broom_RideExitCheck();
	void	Behavior_Broom_RideExit();


	void Player_InterpTurn(_float fTimeDelta);

#pragma endregion

private:
	class CEffectPool* m_pEffectPool = nullptr;
};

NS_END
