#include "pch.h"
#include "CamPosition_AI.h"
#include "CamPosition_Target.h"
#include "GameInstance.h"
#include "Camera_Gaze.h"
#include "Player.h"

CCamPosition_AI::CCamPosition_AI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamPosition(pDevice, pContext)
{
}
CCamPosition_AI::CCamPosition_AI(const CCamPosition_AI& rhs)
	: CCamPosition(rhs)
{
}
HRESULT CCamPosition_AI::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCamPosition_AI::Initialize(void* pArg)
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

	CAMERA_AI_DESC* pDesc = static_cast<CAMERA_AI_DESC*>(pArg);
	{
		m_pLookTransform = m_pTarget_LookPart->Get_Component<CTransform>();
		m_pFollowTransform = m_pTarget_FollowPart->Get_Component<CTransform>();
		m_pParentPos = m_pParentTransformCom->Get_StatePtr(STATE::POSITION);
		m_vDampingStartPosition = *m_pParentPos;
		m_vDampingDestPosition = *m_pParentPos;
		m_vDampingLerpTimer.x = m_vDampingLerpTimer.y;
		m_vFocalRatio.x = m_vFocalRatio.y;
		m_fFocalRatioTargetValue = m_vFocalRatio.y;

		_vector vLookTargetPos = Calc_LookTargetPos();
		m_pLookTransform->Set_State(STATE::POSITION, vLookTargetPos);
		m_pFollowTransform->Set_State(STATE::POSITION, Calc_FollowTargetPos(vLookTargetPos) + XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}
	m_bDampingParentPos = true;

	return S_OK;
}

void CCamPosition_AI::Priority_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_AI))) {
		return;
	}
	{
		if (false == m_bDampingParentPos) {
			m_vDampingLerpTimer.x = 0.f;
			m_vDampingStartPosition = *m_pParentPos;
			m_vDampingDestPosition = *m_pParentPos;
		}
		else {
			m_vDampingLerpTimer.x += fTimeDelta;
			if (m_vDampingLerpTimer.x >= m_vDampingLerpTimer.y) {
				XMStoreFloat4(&m_vDampingStartPosition, Calc_DampingParentPos());
				m_vDampingLerpTimer.x = max(m_vDampingLerpTimer.x - m_vDampingLerpTimer.y, 0.f);
				m_vDampingDestPosition = *m_pParentPos;
			}
		}
	}
	if (true == m_bMovable) {
		m_vAccRotDegrees.y += m_pGameInstance->Get_MouseMove().x * m_fMouseSensor;
		m_vAccRotDegrees.x += m_pGameInstance->Get_MouseMove().y * m_fMouseSensor;
		CMyTools::AdjustAccumulateDegreePitchYawDegree(m_vAccRotDegrees);
	}
	{
		_vector vLookTargetPos = Calc_LookTargetPos();
		m_pLookTransform->Set_State(STATE::POSITION, vLookTargetPos);
		m_pFollowTransform->Set_State(STATE::POSITION, Calc_FollowTargetPos(vLookTargetPos));

		_float focalRatioLerpAlpha = fTimeDelta * m_fFocalRatioLerpSpeed;
		if (focalRatioLerpAlpha > 1.f)
		{
			focalRatioLerpAlpha = 1.f;
		}

		m_vFocalRatio.x = CMyTools::Lerp_f1D(m_vFocalRatio.x, m_fFocalRatioTargetValue, focalRatioLerpAlpha);
	}
}

void CCamPosition_AI::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Up(DIK_HOME)) {

		m_pBinded_Camera->Toggle_AIPriority();
	}
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_AI))) {
		return;
	}
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CCamPosition_AI::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_AI))) {
		return;
	}
	if (true == m_bShoulderLerp)
	{
		m_vShoulderLerpTimer.x += fTimeDelta;

		_float fLerpRatio = m_vShoulderLerpTimer.x/m_vShoulderLerpTimer.y;
		if (fLerpRatio >= 1.f) {
			fLerpRatio = 1.f;
			m_bShoulderLerp = false;
		}
		if (false == m_bShoulderLerp) {
			m_vShoulderLerpTimer.x = 0.f;
			m_fFollowTargetIncludedAngleDegree = m_vShoulderLerpDegree.y;
		} else {
			m_fFollowTargetIncludedAngleDegree = CMyTools::Lerp_f1D(m_vShoulderLerpDegree.x, m_vShoulderLerpDegree.y, fLerpRatio);
		}
	}
	if (true == m_bStartGame) {
		m_vStartLerpTimer.x += fTimeDelta;
		if (m_vStartLerpTimer.x >= m_vStartLerpTimer.y) {
			m_vStartLerpTimer.x = 0.f;
			m_bDampingParentPos = true;
			m_bStartGame = false;
		}
		m_pGameInstance->Add_RenderGroup(RENDER::UI_OVERLAY, this);
	}
}

HRESULT CCamPosition_AI::Render()
{
	return S_OK;
}

_vector CCamPosition_AI::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

_vector CCamPosition_AI::Calc_LookTargetPos()
{
	_vector vDestPos = Calc_DampingParentPos();

	_vector vFoward = { 0.f, 0.f, 1.f, 0.f };
	_vector vRotCameraq = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_vAccRotDegrees.x + m_vAccRealDegrees.x), XMConvertToRadians(m_vAccRotDegrees.y + m_vAccRealDegrees.y), 0.f);
	vFoward = XMVector3Normalize(XMVector3Rotate(vFoward, vRotCameraq));

	_vector vHeadPos = vDestPos + XMVectorSet(0.f, m_fHeadHeight, 0.f, 0.f);
	_vector vFowardPos = vHeadPos + vFoward * m_fCameraFowardDistance;

	return XMVectorLerp(vHeadPos, vFowardPos, m_vFocalRatio.x);
}
_vector CCamPosition_AI::Calc_FollowTargetPos(_vector vLookTargetWorldPos)
{
	_float fBestFollowTargetDistance = m_fCameraBarrelLength * (1.f - m_fDefaultCameraBackToFrontRatio);

	_vector fRotQ = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-m_fFollowTargetIncludedAngleDegree));
	_vector vDestPos = Calc_DampingParentPos();
	_vector vHeadPos = vDestPos + XMVectorSet(0.f, m_fHeadHeight, 0.f, 0.f);
	_vector vDir = vHeadPos - vLookTargetWorldPos;
	_float fLengthSQ = XMVectorGetX(XMVector3LengthSq(vDir));

	if (fLengthSQ < FLT_EPSILON)
	{
		vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		vDir = XMVector3Rotate(vDir, fRotQ);
	}
	else
	{
		vDir = XMVector3Normalize(vDir);
		vDir = XMVector3Rotate(vDir, fRotQ);
	}

	m_BufferHit = {};
	_bool bHit = m_pGameInstance->SphereCast( 0.25f, vLookTargetWorldPos, vDir, fBestFollowTargetDistance,
					PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eSTATIC/*|PSX::PxQueryFlag::eDYNAMIC*/, m_BufferHit);

	if (false == bHit) {
		return vLookTargetWorldPos + vDir * fBestFollowTargetDistance;
	}

	vector<PSX::PxSweepHit> sweepHits;
	_uint iTouchCount = m_BufferHit.getNbTouches();

	if (true == m_BufferHit.hasBlock) {
		sweepHits.reserve(iTouchCount + 1);
		sweepHits.emplace_back(m_BufferHit.block);
	}
	else {
		sweepHits.reserve(iTouchCount);
	}

	auto* touches = m_BufferHit.getTouches();
	for (_uint index = 0; index < iTouchCount; ++index) {
		sweepHits.emplace_back(touches[index]);
	}
	CMyTools::SortHitsByDistance(sweepHits);

	// 벽면에서 팔로우 타겟을 살짝 띄우는 오프셋
	_float fCameraSurfaceOffset = 0.05f;
	_float fCameraMinDistance = 0.20f;
	_float fDistance = { };
	_float fFinalTargetDistance = fBestFollowTargetDistance;
	for (auto& hit : sweepHits)
	{
		fDistance = hit.distance - fCameraSurfaceOffset;
		if (fDistance < fFinalTargetDistance) {
			if (fDistance < fCameraMinDistance) {
				fDistance = fCameraMinDistance;
			}
			fFinalTargetDistance = fDistance;
			break;
		}
	}

	_float fSafeDistance = fBestFollowTargetDistance;
	if (fSafeDistance < FLT_EPSILON3)
	{
		fSafeDistance = FLT_EPSILON3;
	}

	_float fBestFocalRatio = m_vFocalRatio.y * fFinalTargetDistance / fSafeDistance;

	if (fBestFocalRatio < m_fFocalRatioMin) {
		fBestFocalRatio = m_fFocalRatioMin;
	} else if (fBestFocalRatio > m_vFocalRatio.y) {
		fBestFocalRatio = m_vFocalRatio.y;
	}

	m_fFocalRatioTargetValue = fBestFocalRatio;

	return vLookTargetWorldPos + vDir * fFinalTargetDistance;
}
_vector CCamPosition_AI::Calc_DampingParentPos()
{
	_float duration = max(m_vDampingLerpTimer.y, FLT_EPSILON);
	_float fTime = m_vDampingLerpTimer.x / duration;
	fTime = CMyTools::Saturate(fTime); // 0~1

	return XMVectorLerp(XMLoadFloat4(&m_vDampingStartPosition), XMLoadFloat4(&m_vDampingDestPosition), fTime);
}
void CCamPosition_AI::Set_CameraShake(_float fXShock, _float fYShock)
{
	m_vAccRealDegrees.x = fXShock;
	m_vAccRealDegrees.y = fYShock;
}
_vector CCamPosition_AI::Get_ShoulderGlobalPos()
{
	// Right/Left 숄더를 로컬 X 부호로 결정
	_float shoulderSideSign = (true == m_bRightShoulderActive) ? 1.f : -1.f;

	_float3 shoulderLocalPosition = m_vShoulderLocalPos;
	shoulderLocalPosition.x *= shoulderSideSign;

	_vector shoulderLocalOffset = XMLoadFloat3(&shoulderLocalPosition);

	// 카메라 회전(현재 마우스로 누적된 pitch/yaw)
	_vector cameraRotationQuaternion = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(m_vAccRotDegrees.x + m_vAccRealDegrees.x),
		XMConvertToRadians(m_vAccRotDegrees.y + m_vAccRealDegrees.y), 0.f
	);

	// 카메라 로컬 오프셋을 카메라 회전에 맞춰 월드로 회전
	_vector shoulderWorldOffset = XMVector3Rotate(shoulderLocalOffset, cameraRotationQuaternion);

	return shoulderWorldOffset;
}


HRESULT CCamPosition_AI::Ready_Components(void* pArg)
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
HRESULT CCamPosition_AI::Ready_SubParts()
{
	{
		CCamPosition_Target::CAMERAPOSITION_TARGET_DESC Desc{};
		Desc.pParentTransform = m_pTransformCom;
		m_pTarget_LookPart = m_pGameInstance->Clone_Prototype< CCamPosition_Target>(g_iStaticLevel, &Desc);
		m_pTarget_LookPart->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(-34.f, 5, -10.4f, 1.f));
		m_pTarget_FollowPart = m_pGameInstance->Clone_Prototype< CCamPosition_Target>(g_iStaticLevel, &Desc);
		m_pTarget_FollowPart->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(-34.f, 5, -11.4f, 1.f));
	}
	
	CCamera_Gaze::CAMERA_GAZE_DESC CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);
	CameraDesc.pFollowTarget = m_pTarget_FollowPart;
	CameraDesc.pLookTarget = m_pTarget_LookPart;
	CameraDesc.iPriority = 102;
	CameraDesc.pCameraKey = CAMERA_AI;
	CameraDesc.bEnableTransitionLerp = true;
	CameraDesc.bEnableFollowLerp = false;
	CameraDesc.bEnableLookLerp = false;
	CameraDesc.vTransitionTime = { 0.f, 1.f };
	CameraDesc.vFollowLerpTime = { 0.f, TIMER_SHORT_LERP };
	CameraDesc.vLookLerpTime = { 0.f, TIMER_SHORT_LERP };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Gaze>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &CameraDesc, nullptr, &m_pBinded_Camera)))
	{
		return E_FAIL;
	}
	
	m_pGameInstance->Add_Camera(NEXT_LEVEL, m_pBinded_Camera, CAMERA_AI);
	if (FAILED(m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_AI, true))) {
		return E_FAIL;
	}

	return S_OK;
}

CCamPosition_AI* CCamPosition_AI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_AI* pInstance = new CCamPosition_AI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_AI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CCamPosition_AI* CCamPosition_AI::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_AI* pInstance = new CCamPosition_AI(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_AI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamPosition_AI::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTarget_LookPart);
	SAFE_RELEASE(m_pTarget_FollowPart);
}
#ifdef _DEBUG

void CCamPosition_AI::Describe_Entity()
{
	GUI::Begin("CAMERA", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Cam_Shoulder")) {
		m_pTransformCom->Describe_Entity();

		GUI::Text("fMouseSensor : %.1f", m_fMouseSensor);
		GUI::SliderFloat("m_fFollowTargetIncludedAngleDegree", &m_fFollowTargetIncludedAngleDegree, -360.f, 360.f, "%.1f");
		GUI::SliderFloat("m_fDefaultCameraBackToFrontRatio", &m_fDefaultCameraBackToFrontRatio, -1.f, 1.f);
		GUI::SliderFloat("m_vFocalRatio", &m_vFocalRatio.x, 0.f, 1.f);
		GUI::SliderFloat("m_fCameraFowardDistance", &m_fCameraFowardDistance, 0.f, 4.f);
	}
	GUI::End();
}

#endif // _DEBUG
