#include "pch.h"
#include "CamPosition_Shoulder.h"
#include "CamPosition_Target.h"
#include "GameInstance.h"
#include "Camera_Gaze.h"

CCamPosition_Shoulder::CCamPosition_Shoulder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamPosition(pDevice, pContext)
{
}
CCamPosition_Shoulder::CCamPosition_Shoulder(const CCamPosition_Shoulder& rhs)
	: CCamPosition(rhs)
{
}
HRESULT CCamPosition_Shoulder::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCamPosition_Shoulder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_SubParts())) {
		return E_FAIL;
	}

	CAMERA_SHOULDER_DESC* pDesc = static_cast<CAMERA_SHOULDER_DESC*>(pArg);
	{
		m_fMouseSensor = pDesc->fMouseSensor;
		m_vShoulderPosRatio = pDesc->vInitialLook;
		m_fShoulderDistance = pDesc->fShoulderDistance;
		m_vAccDegreeXY = { 0.f, 0.f };
		m_fCameraFocalLength = pDesc->fCameraFocalLength;
		m_fBackFrontRatio = pDesc->fBackFrontRatio;

		m_vShoulderOtherRatio = m_vShoulderPosRatio = m_vShoulderStartRatio = pDesc->vInitialLook;
		m_vShoulderStartRatio.x *= -1.f;
		m_pTransformCom->Set_State(STATE::POSITION, 
			m_pParentTransformCom->Get_State(STATE::POSITION) 
			+ XMVector3Normalize(XMLoadFloat3(&m_vShoulderPosRatio)) * m_fShoulderDistance);

		_vector vShoulderPos = Get_WorldPostion();
		_vector vCameraLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		_vector vRotCameraq = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_vAccDegreeXY.x), XMConvertToRadians(m_vAccDegreeXY.y), 0.f);
		vCameraLook = XMVector3Normalize(XMVector3Rotate(vCameraLook, vRotCameraq));

		CTransform* pLookTransform = m_pTarget_LookPart->Get_Component<CTransform>();
		CTransform* pFollowTransform = m_pTarget_FollowPart->Get_Component<CTransform>();

		pLookTransform->Set_State(STATE::POSITION, vShoulderPos + vCameraLook * (m_fCameraFocalLength * m_fBackFrontRatio));
		pFollowTransform->Set_State(STATE::POSITION, vShoulderPos - vCameraLook * (m_fCameraFocalLength * (1 - m_fBackFrontRatio)));
	}

	return S_OK;
}

void CCamPosition_Shoulder::Priority_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_SHOULDER))) {
		return;
	}
	if (true == m_bMovable) {
		m_vAccDegreeXY.y += m_pGameInstance->Get_MouseMove().x * m_fMouseSensor;
		m_vAccDegreeXY.x += m_pGameInstance->Get_MouseMove().y * m_fMouseSensor;
		CMyTools::AdjustAccumulateDegreePitchYawDegree(m_vAccDegreeXY);
	}
	
	_vector vLook = XMVector3Normalize(XMLoadFloat3(&m_vShoulderPosRatio));
	_vector vRotq = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(m_vAccDegreeXY.y), 0.f);
	vLook = XMVector3Normalize(XMVector3Rotate(vLook, vRotq));

	m_pTransformCom->Set_State(STATE::POSITION, m_pParentTransformCom->Get_State(STATE::POSITION) + XMVector3Normalize(vLook) * m_fShoulderDistance);
	_vector vShoulderPos = Get_WorldPostion();
	_vector vCameraLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	_vector vRotCameraq = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_vAccDegreeXY.x), XMConvertToRadians(m_vAccDegreeXY.y), 0.f);
	vCameraLook = XMVector3Normalize(XMVector3Rotate(vCameraLook, vRotCameraq));

	CTransform* pLookTransform = m_pTarget_LookPart->Get_Component<CTransform>();
	CTransform* pFollowTransform = m_pTarget_FollowPart->Get_Component<CTransform>();

	pLookTransform->Set_State(STATE::POSITION, vShoulderPos + vCameraLook * (m_fCameraFocalLength * m_fBackFrontRatio));
	pFollowTransform->Set_State(STATE::POSITION, vShoulderPos - vCameraLook * (m_fCameraFocalLength * (1 - m_fBackFrontRatio)));
}

void CCamPosition_Shoulder::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_INSERT)) {
		m_pBinded_Camera->Toggle_Priority();
	}
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_SHOULDER))) {
		return;
	}
	Describe_Entity();
	if (m_pGameInstance->Key_Up(DIK_GRAVE)) {
		m_bMovable = !m_bMovable;
	}
	if (m_pGameInstance->Mouse_Down(DIM_MBUTTON)) {
		m_pGameInstance->Toggle_MouseCenter();
	}

	if (true == m_bMovable) {
		m_vAccRotDegrees.y += m_pGameInstance->Get_MouseMove().x * m_fMouseSensor;
		m_vAccRotDegrees.x += m_pGameInstance->Get_MouseMove().y * m_fMouseSensor;
		CMyTools::AdjustAccumulateDegreePitchYawDegree(m_vAccRotDegrees);
	}

	if (m_pGameInstance->Key_Up(DIK_P)) {
		m_vShoulderOtherRatio = m_vShoulderPosRatio = m_vShoulderStartRatio;
		m_vShoulderStartRatio.x *= -1.f;
		m_bLerp = true;
		m_vLerpTimer.x = 0.f;
	}
}

void CCamPosition_Shoulder::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_SHOULDER))) {
		return;
	}
	if (true == m_bLerp) {
		m_vLerpTimer.x += fTimeDelta;
		XMStoreFloat3(&m_vShoulderPosRatio, XMVectorLerp(XMLoadFloat3(&m_vShoulderStartRatio), XMLoadFloat3(&m_vShoulderOtherRatio), m_vLerpTimer.x / m_vLerpTimer.y));
		if (m_vLerpTimer.x > m_vLerpTimer.y) {
			m_bLerp = false;
			m_vLerpTimer.x = 0.f;

			m_vShoulderPosRatio = m_vShoulderOtherRatio;
		}
	}
}

HRESULT CCamPosition_Shoulder::Render()
{
	return S_OK;
}

_vector CCamPosition_Shoulder::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CCamPosition_Shoulder::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC DescTransfrom = {};
	DescTransfrom.fRadius = 30.f;
	DescTransfrom.fRotationPerSec = 0.f;
	DescTransfrom.fSpeedPerSec = 0.f;
	if (FAILED(__super::Ready_Components(&DescTransfrom))) {
		return E_FAIL;
	}
	return S_OK;
}

void CCamPosition_Shoulder::Set_CameraShake(_float fXShock, _float fYShock)
{
	m_vAccRealDegrees.x = fXShock;
	m_vAccRealDegrees.y = fYShock;
}

HRESULT CCamPosition_Shoulder::Ready_SubParts()
{
	{
		CCamPosition_Target::CAMERAPOSITION_TARGET_DESC Desc{};
		Desc.pParentTransform = m_pTransformCom;
		m_pTarget_LookPart = m_pGameInstance->Clone_Prototype< CCamPosition_Target>(g_iStaticLevel, &Desc);
		m_pTarget_FollowPart = m_pGameInstance->Clone_Prototype< CCamPosition_Target>(g_iStaticLevel, &Desc);
	}

	CCamera_Gaze::CAMERA_GAZE_DESC CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);
	CameraDesc.pFollowTarget = m_pTarget_FollowPart;
	CameraDesc.pLookTarget = m_pTarget_LookPart;
	CameraDesc.iPriority = 51;
	CameraDesc.pCameraKey = CAMERA_SHOULDER;
	CameraDesc.bEnableTransitionLerp = true;
	CameraDesc.bEnableFollowLerp = true;
	CameraDesc.bEnableLookLerp = true;
	CameraDesc.vTransitionTime = { 0.f, 1.f };
	CameraDesc.vFollowLerpTime = { 0.f, TIMER_SHORT_LERP };
	CameraDesc.vLookLerpTime = { 0.f, TIMER_SHORT_LERP };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Gaze>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &CameraDesc, nullptr, &m_pBinded_Camera)))
	{
		return E_FAIL;
	}
	m_pGameInstance->Add_Camera(g_iStaticLevel, m_pBinded_Camera, CAMERA_SHOULDER);


	return S_OK;
}
CCamPosition_Shoulder* CCamPosition_Shoulder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_Shoulder* pInstance = new CCamPosition_Shoulder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_Shoulder");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CCamPosition_Shoulder* CCamPosition_Shoulder::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_Shoulder* pInstance = new CCamPosition_Shoulder(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_Shoulder");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamPosition_Shoulder::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTarget_LookPart);
	SAFE_RELEASE(m_pTarget_FollowPart);
}
void CCamPosition_Shoulder::Describe_Entity()
{
	GUI::Begin("Cam_Shoulder");
	m_pTransformCom->Describe_Entity();
	GUI::Text("fMouseSensor : %.1f", &m_fMouseSensor);
	GUI::SliderFloat("ShoulderDistance", &m_fShoulderDistance, 0.f, 10.f, "%.1f");
	GUI::SliderFloat("BackFrontRatio", &m_fBackFrontRatio, 0.01f, 0.99f, "%.2f");
	GUI::Text("Pitch Yaw : %f %f", &m_vAccDegreeXY.x, &m_vAccDegreeXY.y);
	GUI::SliderFloat3("ShoulderPosRatio", (_float*)&m_vShoulderPosRatio, -2.f, 2.f, "%.1f");
	GUI::End();
}
