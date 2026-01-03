#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CBroom final : public CUnit
{
public:
	struct BroomInput
	{
		_float X;
		_float Y;
		_float Z;
		_bool  bHoverToggle;
		_bool  bTurbo;
	};


private:
	CBroom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom(const CBroom& Prototype);
	virtual ~CBroom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Ride(_bool bRide) { m_bRide = bRide; }
	void Set_Hover(_bool bHover) { m_bHoverToggle = bHover; }
	_bool Get_Hover() { return m_bHoverToggle; }
	_bool Get_Turbo() { return m_bTurbo; }

	void Set_Input(BroomInput broomInput) { m_Input = broomInput; }
	void Set_AISpeed(_float speedMul, _float accelMul);
	void Set_Move(_bool bMove) { m_bMove = bMove; }	
	_bool Get_Move() { return m_bMove; }
	HRESULT         Ready_Child();
private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	class CUnit* m_pParentUnit = {nullptr};

	_bool m_bHoverToggle = { true };
	_bool m_bTurbo = { false };

	_bool m_bRide = {false};

	_float m_fSpeed = 0.f; 
	_float m_fTurnSpeed = 0.f;
	_float m_fTargetSpeed = 0.f;
	_float m_fTurnMaxSpeed = 12.f;
	_float m_fFlyTurnMaxSpeed = 9.f;
	_float m_fTurboTurnMaxSpeed = 12.f;
	_float m_fHoverMaxSpeed = 5.f;
	_float m_fFlyMaxSpeed = 15.f;    
	_float m_fTurboMaxSpeed = 20.f;
	_float m_fAccel = 1.f;             
	_float m_fDecel = 1.f;
	_float m_fTurnDecel = 0.3f;
	_float m_fVerticalSpeed = 0.f;

	_float m_fFlyAccel = 1.f;
	_float m_fFlyDecel = 1.f;

	_float m_fTurboAccel = 1.3f;
	_float m_fTurboDecel = 0.6f;

	_bool m_bMove = { true };
	
#pragma region AI
	_float m_fAISpeedMul = 1.f;
	_float m_fAIAccelMul = 1.f;

	_float m_fAICondition = 1.f;   
	_float m_fAISeed = 0.f;      
	_float m_fAITime = 0.f;

#pragma endregion


	_float3 m_vCameraLookDir = { 0.f, 0.f, 1.f, };
	_float3 m_vCameraRightDir = { 1.f, 0.f, 0.f };

	BroomInput			m_Input;
	class CEffectParts* m_pWindEffect = { nullptr };
	_float m_fCameraOffset = { 10.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Update_CameraCoordinateSystem();
	void PlayerInput();

public:
	static CBroom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

#pragma region STATE
	void Camera_InterpTurn(_float fTimeDelta);
	virtual void Add_FSM();

	_float		m_fAmount = { 1.f };
	_float		m_fInputTime = {};
	_bool		m_bRatio = { false };

	
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

	void	Behavior_Broom_HoverEnter();
	HRESULT Behavior_Broom_HoverExitCheck(_float fTimeDelta);
	void	Behavior_Broom_HoverExit();

	void	Behavior_Broom_FlyEnter();
	HRESULT Behavior_Broom_FlyExitCheck(_float fTimeDelta);
	void	Behavior_Broom_FlyExit();

	void	Behavior_Broom_TurboFlyEnter();
	HRESULT Behavior_Broom_TurboFlyExitCheck(_float fTimeDelta);
	void	Behavior_Broom_TurboFlyExit();
#pragma endregion


};

NS_END
