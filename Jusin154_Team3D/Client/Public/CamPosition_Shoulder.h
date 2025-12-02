#pragma once
#include "Client_Define.h"
#include "CamPosition.h"
#include "CamPosition_Target.h"

NS_BEGIN(Client)

class CCamPosition_Shoulder final : public CCamPosition
{
public:
	typedef struct tagCameraShoulder_Desc : public CCamPosition::CAMERAPOSITION_DESC
	{
		_float		fMouseSensor = { 0.5f };
		_float		fShoulderDistance = { 2.f };
		_float		fBackFrontRatio = { 0.9f };
		_float		fCameraFocalLength = { 10.f };
		_float3		vInitialLook = { 1.f, 2.f, -1.f };
	}CAMERA_SHOULDER_DESC;
private:
	CCamPosition_Shoulder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition_Shoulder(const CCamPosition_Shoulder& rhs);
	virtual ~CCamPosition_Shoulder() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_SubParts();
	void Start_LerpShoulderPos();

	_bool	m_bMovable = { true };
	_float	m_fMouseSensor = { 0.5f };
	_float	m_fShoulderDistance = { 2.f };
	_float	m_fCameraFocalLength = { 10.f };
	_float	m_fBackFrontRatio = { 0.9f };
	_float2	m_vAccDegreeXY = {};
	_float3 m_vShoulderPosRatio = { 1.f, 2.f, -1.f };

	_float3 m_vShoulderStartRatio = { 1.f, 2.f, -1.f };
	_float3 m_vShoulderOtherRatio = { 1.f, 2.f, -1.f };

	_bool m_bShoulderLerp = { false };
	_float2 m_vShoulderLerpTimer = { 0.f, 1.f };

	_float2 m_vPosLerpTimer = { 0.f, 0.16f };
	_float4 m_StartPos = { };
	_float4 m_DestPos = { };

	_bool m_bZoomIn = { false };
	_bool m_bLerp = {};


	CCamPosition_Target* m_pTarget_LookPart = { nullptr };
	CCamPosition_Target* m_pTarget_FollowPart = { nullptr };
	class CCamera_Gaze* m_pBinded_Camera = { nullptr };

public:
	static CCamPosition_Shoulder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamPosition_Shoulder* Clone(void* pArg, class CGameObject* pOWner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
