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
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;
	_bool Get_Aim() { return m_bAim; }
	void Set_SpellHit(_bool bHit) { m_bSpellHit = bHit; }
	void Set_Shield(_bool bShield) { m_bShield = bShield; }
	void Start_CameraShake(_float fTime, _float fIntense);
	_int Get_UIState() { return m_eUIState; }
#ifdef _DEBUG
	void Render_CameraCoordinateSystem();
#endif // _DEBUG
	_bool   Set_Sprint(_bool bSprint) { m_bSprintToggle = bSprint; }
	_matrix Get_WandPos();
	void UpdateGrapInteractive(_float fTimeDelta);
	void Update_CameraShake(_float fTimeDelta);
private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	LOCKON_INFO m_LockOnInfo = {};
	class CMapElement_Interactable* m_pGrapInteractive = { nullptr };
	_float m_fDirectionRadian = 0.f;

	_bool m_bSprintToggle = { false };
	_bool m_bWalkToggle = { false };
	_bool m_bCameraShake = { false };

	_float3 m_vCameraLookDir = { 0.f, 0.f, 1.f, };
	_float3 m_vCameraRightDir = { 1.f, 0.f, 0.f };
	_float2 m_vGrapInteratableLerp = { 0.f, TIMER_SHORT_LERP };
	_float2 m_vCameraShakeTimer = { 0.f, TIMER_SHORT_LERP };
	_float m_fCameraShakeTime = TIMER_SHORT_LERP;
	_float m_fCameraShakeIntense = 5.f;
	_float m_fOriginGravityAmount = {};
	_float m_fGravityAmount = {};

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
	CStat* m_pStat = { nullptr };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	void ReLockOnTarget();
	void SetGravity();

	void Update_CameraCoordinateSystem(_float fTimeDelta);
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
	// UI 연동 추가
	void Get_Spell(_int SkillIndex);
	void Get_UIState(_int UIState);

	virtual void Add_FSM();
	virtual void Set_Anim();

	void	Reset_LightCombo() { m_iLightCombo = 0; }
	_uint	Next_LightCombo() { return ++m_iLightCombo; }
	void	Set_LightCombo(_uint LightCombo) { m_iLightCombo = LightCombo; }

	function<void()> m_InputAction = nullptr;
	_int			m_eUIState = { };
	_uint			m_iLightCombo = { 0 };

	_float3			m_OffsetPos = {};
	_float			m_fAmount = { 1.f };
	_float			m_fInputTime = {};
	_bool			m_bRatio = { false };
	_bool			m_bAim = { false };
	_float			m_fAnimSpeed = {};
	_bool			m_bOnce = {  };
	_bool			m_bLookAt = {false};
	_bool			m_bSpellHit = {};
	_float3			m_BroomScale = { 0.f, 0.f, 0.f };
	_float3			m_vScale = { 0.f, 0.f, 0.f };
	_float3			m_TargetScale = { 1.f, 1.f, 1.f };
	_float			m_fScaleSmoothSpeed = 2.5f;
	_bool			m_bLumos = {};
	_bool			m_bShield = {false};
	_float			m_fAnimTime = {};
	_bool			m_bTurbo = {};
	_float			m_fBlinkTime = {  };
	_float			m_fAccel = { 1.f };
	_float			m_fSlideSpeed = {};
	_float			m_fTargetSpeed = { 7.f };
	/* 무적 불 변수*/
#ifdef _DEBUG
	_bool			m_isDebugMode = { false };
#endif

	HRESULT InputAction();
	HRESULT InputMove();
	HRESULT InputKeyUpMove();
	HRESULT InputSpell();
	HRESULT InputAim();
	HRESULT InputBroom();

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
	HRESULT Behavior_LandExitCheck(_float fTimeDelta);
	void	Behavior_LandExit();

	void	Behavior_DodgeEnter();
	HRESULT Behavior_DodgeExitCheck(_float fTimeDelta);
	void	Behavior_DodgeExit();

	void	Behavior_BlinkEnter();
	HRESULT Behavior_BlinkExitCheck(_float fTimeDelta);
	void	Behavior_BlinkExit();

	void	Behavior_SlideEnter();
	HRESULT Behavior_SlideExitCheck(_float fTimeDelta);
	void	Behavior_SlideExit();

	void	Behavior_CombatEnter();
	HRESULT Behavior_CombatExitCheck();
	void	Behavior_CombatExit();

	void	Behavior_LightAttackEnter();
	HRESULT Behavior_LightAttackExitCheck(_float fTimeDelta);
	void	Behavior_LightAttackExit();

	void	Behavior_SpellEnter();
	HRESULT Behavior_SpellExitCheck();
	void	Behavior_SpellExit();
	 
	void	Behavior_AncientSpellEnter();
	HRESULT Behavior_AncientSpellExitCheck();
	void	Behavior_AncientSpellExit();

	void	Behavior_ShieldEnter();
	HRESULT Behavior_ShieldExitCheck();
	void	Behavior_ShieldExit();

	void	Behavior_HitEnter();
	HRESULT Behavior_HitExitCheck();
	void	Behavior_HitExit();

	void	Behavior_Broom_RideEnter();
	HRESULT Behavior_Broom_RideExitCheck(_float fTimeDelta);
	void	Behavior_Broom_RideExit();

	void	Behavior_Broom_Ride_MoveEnter();
	HRESULT Behavior_Broom_Ride_MoveExitCheck(_float fTimeDelta);
	void	Behavior_Broom_Ride_MoveExit();

	void	Behavior_Broom_HoverEnter();
	HRESULT Behavior_Broom_HoverExitCheck(_float fTimeDelta);
	void	Behavior_Broom_HoverExit();
						   
	void	Behavior_Broom_FlyEnter();
	HRESULT Behavior_Broom_FlyExitCheck(_float fTimeDelta);
	void	Behavior_Broom_FlyExit();

	void	Behavior_Broom_TurboFlyEnter();
	HRESULT Behavior_Broom_TurboFlyExitCheck(_float fTimeDelta);
	void	Behavior_Broom_TurboFlyExit();

	void	Behavior_Broom_DismountEnter();
	HRESULT Behavior_Broom_DismountExitCheck(_float fTimeDelta);
	void	Behavior_Broom_DismountExit();

	void Player_InterpTurn(_float fTimeDelta);
	void Throwing_Interactive();
	void Attach_Broom();
#pragma endregion

private:
	class CEffectPool* m_pEffectPool = nullptr;
};

NS_END
