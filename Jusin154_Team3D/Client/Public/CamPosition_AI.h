#pragma once
#include "Client_Define.h"
#include "CamPosition.h"
#include "CamPosition_Target.h"

NS_BEGIN(Client)

class CCamPosition_AI final : public CCamPosition
{
public:
	typedef struct tagCameraAI_Desc : public CCamPosition::CAMERAPOSITION_DESC
	{
	}CAMERA_AI_DESC;
private:
	CCamPosition_AI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition_AI(const CCamPosition_AI& rhs);
	virtual ~CCamPosition_AI() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _vector Get_WorldPostion() override;
	_vector			Get_ShoulderGlobalPos();
	void			Set_CameraShake(_float fXShock, _float fYShock);
	
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_SubParts();
	_vector Calc_LookTargetPos();
	_vector Calc_FollowTargetPos(_vector vLookTargetWorldPos);
	_vector Calc_DampingParentPos();

#pragma region Base
	_float		m_fMouseSensor = { 0.1f };
	_float		m_fCameraFowardDistance = { 3.45f };
	_float		m_fCameraBarrelLength = { 10.f };
	_float2		m_vFocalRatio = { 0.f, 0.467f };
	_float		m_fHeadHeight = { 1.61f };
	_float		m_fFollowTargetIncludedAngleDegree = { 20.f };
	_float		m_fDefaultCameraBackToFrontRatio = { 0.61f };
	_float2		m_vAccRotDegrees = { 0.f, 0.f };
	_float2		m_vAccRealDegrees = { 0.f, 0.f };
	_float3		m_vShoulderLocalPos = { 1.f, 2.f, 2.f };
#pragma endregion
#pragma region Lerp
	_bool m_bStartGame = { true };
	_bool m_bDampingParentPos = { true };
	_bool m_bShoulderLerp = { false };
	_bool m_bRightShoulderActive = { true };
	_float2 m_vShoulderLerpTimer = { 0.f, 0.30f };
	_float2 m_vDampingLerpTimer = { 0.f, 0.10f };
	_float2 m_vStartLerpTimer = { 0.f, 0.30f };

	_float2 m_vShoulderLerpDegree = { };
	_float2 m_vShoulderLerpIncludedAngleDegree = { -20.f, 20.f };
	_float4 m_vDampingStartPosition = { };
	_float4 m_vDampingDestPosition = { };

	_float m_fFocalRatioTargetValue = 0.f;
	_float m_fFocalRatioLerpSpeed = 6.f;
	_float m_fFocalRatioMin = 0.05f;

	_float2 m_vMoveLerpPositions = { };
	_float2 m_vMoveLerpTimer = { 0.f, TIMER_SHORT_LERP };
#pragma endregion
	_float2 m_vCameraShakeTimer = { 0.f, 0.3f };

	PSX::PxSweepBuffer m_BufferHit = {};

	_bool m_bZoomIn = { false };
	_bool m_bMovable = { true };

	const _float4*			m_pParentPos = { nullptr };
	CTransform*				m_pLookTransform = { nullptr };
	CTransform*				m_pFollowTransform = { nullptr };
	CCamPosition_Target*	m_pTarget_LookPart = { nullptr };
	CCamPosition_Target*	m_pTarget_FollowPart = { nullptr };
	class CCamera_Gaze*		m_pBinded_Camera = { nullptr };

public:
	static CCamPosition_AI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamPosition_AI* Clone(void* pArg, class CGameObject* pOWner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
