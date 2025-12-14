#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CBroomRacerAI final : public CUnit
{
private:
	CBroomRacerAI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroomRacerAI(const CBroomRacerAI& Prototype);
	virtual ~CBroomRacerAI() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;
private:
	CLight* m_pLightCom = { nullptr };
	LIGHT_DESC LightDesc = {};

	class CModel* m_pBroomModel = { nullptr };
	class CTransform* m_pBroomTransform = { nullptr };
	class CBroom* m_pBroom = { nullptr };
	const list<CGameObject*>* m_pRaceRingList = { nullptr };
	class CRaceRing* m_pRaceRing = { nullptr };
	list<CGameObject*>::const_iterator m_itCurRing;
	_int m_iIndex = {};
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();

	_float ComputeTurnToRing();
	_float ComputeHeightAdjust();
#ifdef _DEBUG
	unique_ptr<BasicEffect> m_BasicEffect;
	unique_ptr<PrimitiveBatch<VertexPositionColor>> m_Batch;
#endif // _DEBUG


public:
	static CBroomRacerAI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

#pragma region STATE

private:
	virtual void Add_FSM();
	virtual void Set_Anim();
	void Check_RingPassed();
	void OnRingPassed();
	void Set_Input();

	function<void()> m_InputAction = nullptr;
	_int			m_eUIState = { };
	_uint			m_iLightCombo = { 0 };

	_float3			m_OffsetPos = {};
	_float			m_fAmount = { 1.f };
	_bool			m_bRatio = { false };
	_float			m_fAnimSpeed = {};
	_bool			m_bOnce = {  };
	_float3			m_BroomScale = { 0.f, 0.f, 0.f };
	_float3			m_TargetScale = { 1.f, 1.f, 1.f };
	_float			m_fScaleSmoothSpeed = 2.5f;
	_float			m_fAnimTime = {};
	_bool			m_bTurbo = {};
	_vector			m_vPrevPos = {};

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


	void Attach_Broom();
#pragma endregion
};

NS_END
