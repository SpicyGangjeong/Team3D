#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CBroomRacerAI final : public CUnit
{
	enum MESH_ORDER {
		HEAD_EYE_OCC,
		HEAD_FACE,
		HEAD_TEETH,
		HEAD_EYELASH,
		HEAD_EYES,
		BODY_ARMS,
		HAIR_MAIN,
		HAIR_MAIN_CLOTH,
		LOWER,
		SHOES,
		UPPER,
		HAIR_SUB,
		HAIR_SUB_CLOTH,
		ROBE_SKIRT,
		ROBE_FUR,
		ROBE_CLOTH,
		END
	};
private:
	CBroomRacerAI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroomRacerAI(const CBroomRacerAI& Prototype);
	virtual ~CBroomRacerAI() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;
	void Set_RaceRing(class CRaceRing* pRaceRing) {m_pRaceRing = pRaceRing;}
	class CBroom* Get_Broom() { return m_pBroom; }
	void Set_Move(_bool bMove) { m_bMove = bMove; }
private:
	CLight* m_pLightCom = { nullptr };
	LIGHT_DESC LightDesc = {};

	class CModel* m_pBroomModel = { nullptr };
	class CTransform* m_pBroomTransform = { nullptr };
	class CBroom* m_pBroom = { nullptr };
	class CRaceRing* m_pRaceRing = { nullptr };
	class CBroomRaceManager* m_pBroomRaceManager = { nullptr };
	class CCamPosition_AI* m_pCamPosition_AIPart = { nullptr };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_ShaderParameters(_uint iMeshOrder);

	_vector Get_RingForwardTarget();

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
	void Set_Input(_float fTimeDelta);

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
	_bool			m_bMove = {};
	_float			m_LaneOffsetX = 0.f; 
	_float			m_LaneOffsetY = 0.f; 
	_float			m_TurnGain = 1.f;
	_float			m_HeightGain =1.f;

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
