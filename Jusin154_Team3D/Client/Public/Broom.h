#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CBroom final : public CUnit
{
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

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	_bool m_bHoverToggle = { true };
	_bool m_bWalkToggle = { false };

	_bool m_bRide = {false};

	_float m_fSpeed = 0.f; 
	_float m_fTurnSpeed = 0.f;
	_float m_fTargetSpeed = 0.f;
	_float m_fTurnMaxSpeed = 7.f;
	_float m_fFlyMaxSpeed = 15.f;    
	_float m_fHoverMaxSpeed = 7.f;
	_float m_fAccel = 1.f;             
	_float m_fDecel = 1.f;
	_float m_fTurnDecel = 0.3f;


	_float m_fVerticalSpeed = 0.f;



	_float3 m_vCameraLookDir = { 0.f, 0.f, 1.f, };
	_float3 m_vCameraRightDir = { 1.f, 0.f, 0.f };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Update_CameraCoordinateSystem();

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
	virtual void Set_Anim();

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

#pragma endregion


};

NS_END
