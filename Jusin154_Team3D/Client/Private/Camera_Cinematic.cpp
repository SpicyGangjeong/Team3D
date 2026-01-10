#include "pch.h"
#include "Camera_Cinematic.h"
#include "InfoInstance.h"
#include "TimeSocket.h"
#include "CamPosition_Target.h"
#include "Layer.h"
#include "Unit.h"


CCamera_Cinematic::CCamera_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CCamera(pDevice, pContext) {}
CCamera_Cinematic::CCamera_Cinematic(const CCamera_Cinematic& rhs) : CCamera(rhs), m_pInfoInstance(CInfoInstance::GetInstance()) { }

void CCamera_Cinematic::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->Key_Pressing(DIK_HOME)) {
		if (m_pGameInstance->Key_Up(DIK_END)) {
			_string strCutSceneName = "RanrokIntro";
			m_pInfoInstance->Active_Event(strCutSceneName);
		}
	}
#endif // _DEBUG
	if (false == m_bActive) {
		return;
	}
	if (false == m_bIsCurrentTransition) {
		m_pLookTargetPart->Priority_Update(fTimeDelta);
		m_pFollowTargetPart->Priority_Update(fTimeDelta);
		m_pTransformCom->Set_State(STATE::POSITION, m_pFollowTargetPart->Get_WorldPostion());
		m_pTransformCom->LookAt(m_pLookTargetPart->Get_WorldPostion());
	}
	else {
		Transition(fTimeDelta);
	}
	m_bActive = true;
	Update_LerpTimer(fTimeDelta);
	__super::Bind_Matrices();
}

void CCamera_Cinematic::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
	if (false == m_bActive) {
		return;
	}
}
void CCamera_Cinematic::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
}
HRESULT CCamera_Cinematic::Render() { return S_OK; }
void CCamera_Cinematic::Active_Camera(pair<_float4, _float3>& pairTransitionInfo)
{
	if (true == m_bEnable_TransitionLerp) {
		_vector vNewPos = XMVectorSetW(XMLoadFloat3(&pairTransitionInfo.second), 1.f);
		m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(),
			XMLoadFloat4(&pairTransitionInfo.first), vNewPos));
		m_vTransitionTime.x = 0.f;
		m_bIsCurrentTransition = true;

		m_pFollowTargetPart->Set_WorldPostion(vNewPos);
		m_pLookTargetPart->Set_WorldPostion(vNewPos + m_pTransformCom->Get_State(STATE::LOOK) * 10.f);
	}
	m_bActive = true;
}

void CCamera_Cinematic::Update_LerpTimer(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
	Lerp_Translation(fTimeDelta);
	Lerp_Rotation(fTimeDelta);
	Lerp_FovY(fTimeDelta);
}

void CCamera_Cinematic::Set_Priority(_uint iPriority)
{
	m_iPriority = iPriority;
}

void CCamera_Cinematic::Set_LookTarget(CUnit* pTarget, const _float4x4* pTargetSocketMatrix)
{
	m_pLookTargetPart->Stalking_Target(pTarget, pTargetSocketMatrix);
}

void CCamera_Cinematic::Set_FollowTarget(CUnit* pTarget, const _float4x4* pTargetSocketMatrix)
{
	m_pFollowTargetPart->Stalking_Target(pTarget, pTargetSocketMatrix);
}

void CCamera_Cinematic::Trigger(CTimeSocket& Socket)
{
	SOCKETCONTENTS* pContents = &Socket.m_Contents;
	switch (pContents->eTypeFunc)
	{
	case TIMESOCKET_FUNC::BIND_CAMERA:
		m_pGameInstance->Bind_Camera(NEXT_LEVEL, pContents->wstrKeyName, true);
		break;
	case TIMESOCKET_FUNC::TRANSLATION:
	{
		m_pFollowTargetPart->Set_WorldPostion(XMVectorSetW(XMLoadFloat3((_float3*)&pContents->pxTransform.p), 1.f));
		Clear_Lerp_Translation();
	} break;
	case TIMESOCKET_FUNC::TRANSLATION_LERP:
	{
		Clear_Lerp_Translation();
		Start_Lerp_Translation(pContents->vParam_11.x, pContents->pxTransform);
	} break;
	case TIMESOCKET_FUNC::ROTATION: {
		m_pTransformCom->RotationQ(pContents->pxTransform.q);
		m_pLookTargetPart->Set_WorldPostion(m_pTransformCom->Get_State(STATE::POSITION) + m_pTransformCom->Get_State(STATE::LOOK) * 10.f);
		Clear_Lerp_Rotation();
	} break;
	case TIMESOCKET_FUNC::ROTATION_LERP:
	{
		Clear_Lerp_Rotation();
		Start_Lerp_Rotation(pContents->vParam_11.x, pContents->pxTransform);
	} break;
	case TIMESOCKET_FUNC::LOOK_AT:
	{
		CUnit* pUnit = (CUnit*)pContents->pOtherTarget;
		Set_LookTarget(pUnit, pUnit->Get_SocketMatrixPtr(pContents->vParam_12.c_str()));
		//Set_LookTarget(pUnit, nullptr);
	}break;
	case TIMESOCKET_FUNC::DONT_LOOK_AT:
	{
		CUnit* pUnit = (CUnit*)pContents->pOtherTarget;
		Set_FollowTarget(pUnit, pUnit->Get_SocketMatrixPtr(pContents->vParam_12.c_str()));
	}break;
	case TIMESOCKET_FUNC::ZOOM_IN:
	{
		Set_Fov(XMConvertToRadians(pContents->vParam_11.x));
	}break;
	case TIMESOCKET_FUNC::ZOOM_OUT:
	{
		Set_Fov(XMConvertToRadians(pContents->vParam_11.x));
	}break;
	case TIMESOCKET_FUNC::ZOOM_LERP:
	{
		Start_Lerp_FovY(pContents->vParam_11.z, { pContents->vParam_11.x, pContents->vParam_11.y });
		Set_Fov(XMConvertToRadians(pContents->vParam_11.x));
	}break;
	case TIMESOCKET_FUNC::END_CINEMATIC:
	{
		m_pGameInstance->Bind_Camera(CURRENT_LEVEL, Socket.m_Contents.wstrKeyName, true);
	}break;
	default:
		break;
	}
}

HRESULT CCamera_Cinematic::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCamera_Cinematic::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_SubPart())) {
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CCamera_Cinematic::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera_Cinematic::Ready_SubPart()
{
	{
		CCamPosition_Target::CAMERAPOSITION_TARGET_DESC Desc{};
		Desc.pParentTransform = m_pTransformCom;
		m_pFollowTarget = m_pFollowTargetPart = m_pGameInstance->Clone_Prototype< CCamPosition_Target>(g_iStaticLevel, &Desc);
		m_pFollowTargetPart->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(-34.f, 5, -10.4f, 1.f));
		m_pLookTarget = m_pLookTargetPart = m_pGameInstance->Clone_Prototype< CCamPosition_Target>(g_iStaticLevel, &Desc);
		m_pLookTargetPart->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(-34.f, 5, -11.4f, 1.f));
	}
	return S_OK;
}

HRESULT CCamera_Cinematic::Bind_ShaderResources()
{
	return S_OK;
}
void CCamera_Cinematic::Lerp_Translation(_float fTimeDelta)
{
	if (true == m_bLerpTranslation) {
		m_vLerpTranslationTimer.x += fTimeDelta;
		if (m_vLerpTranslationTimer.y < m_vLerpTranslationTimer.x) {
			m_vLerpTranslationTimer.x = m_vLerpTranslationTimer.y;
			m_bLerpTranslation = false;
		}
		
		_vector vPos = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vLerpTranslationStart), XMLoadFloat3(&m_vLerpTranslationEnd), m_vLerpTranslationTimer.x / m_vLerpTranslationTimer.y), 1.f);
		m_pFollowTargetPart->Set_WorldPostion(vPos);
	}
}
void CCamera_Cinematic::Start_Lerp_Translation(_float fTimeMaximum, PSX::PxTransform pxTransform)
{
	_float3 vTrans = { pxTransform.p.x , pxTransform.p.y, pxTransform.p.z };
	Start_Lerp_Translation(fTimeMaximum, vTrans);
}
void CCamera_Cinematic::Start_Lerp_Translation(_float fTimeMaximum, _float3& vTrans)
{
	m_bLerpTranslation = { fTimeMaximum != 0 };
	m_vLerpTranslationTimer = { 0.f, fTimeMaximum };
	XMStoreFloat3(&m_vLerpTranslationStart, m_pTransformCom->Get_State(STATE::POSITION));
	m_vLerpTranslationEnd = vTrans;
}
void CCamera_Cinematic::Clear_Lerp_Translation()
{
	m_bLerpTranslation = { false };
	m_vLerpTranslationTimer = {};
	m_vLerpTranslationStart = {};
	m_vLerpTranslationEnd = {};
}
void CCamera_Cinematic::Lerp_Rotation(_float fTimeDelta)
{
	if (true == m_bLerpRotiation) {
		m_vLerpRotiationTimer.x += fTimeDelta;
		if (m_vLerpRotiationTimer.y < m_vLerpRotiationTimer.x) {
			m_vLerpRotiationTimer.x = m_vLerpRotiationTimer.y;
			m_bLerpRotiation = false;
		}
		_vector vRotQ = XMQuaternionSlerp(XMLoadFloat4(&m_vLerpRotQStart), XMLoadFloat4(&m_vLerpRotQEnd), m_vLerpRotiationTimer.x / m_vLerpRotiationTimer.y);
		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
		m_pLookTargetPart->Get_Component<CTransform>()->RotationQ(vRotQ);
	}
}
void CCamera_Cinematic::Start_Lerp_Rotation(_float fTimeMaximum, PSX::PxTransform pxTransform)
{
	_float4 vRotQ = { pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w };
	Start_Lerp_Rotation(fTimeMaximum, vRotQ);
}
void CCamera_Cinematic::Start_Lerp_Rotation(_float fTimeMaximum, _float4& vRotQ)
{
	m_bLerpRotiation = { fTimeMaximum != 0 };
	m_vLerpRotiationTimer = { 0.f, fTimeMaximum };
	XMStoreFloat4(&m_vLerpRotQStart, m_pTransformCom->Get_QuarternionVector());
	m_vLerpRotQEnd = vRotQ;
}
void CCamera_Cinematic::Clear_Lerp_Rotation()
{
	m_bLerpRotiation = { false };
	m_vLerpRotiationTimer = {};
	m_vLerpRotQStart = {};
	m_vLerpRotQEnd = {};
}
void CCamera_Cinematic::Lerp_FovY(_float fTimeDelta)
{
	if (true == m_bLerpFovY) {
		m_vLerpFovYTimer.x += fTimeDelta;
		if (m_vLerpFovYTimer.y < m_vLerpFovYTimer.x) {
			m_vLerpFovYTimer.x = m_vLerpFovYTimer.y;
			m_bLerpFovY = false;
		}
		Set_Fov(XMConvertToRadians(CMyTools::Lerp_f1D(m_vFovYLerp.x, m_vFovYLerp.y, m_vLerpFovYTimer.x / m_vLerpFovYTimer.y)));
	}
}
void CCamera_Cinematic::Start_Lerp_FovY(_float fTimeMaximum, _float2 vRange)
{
	m_bLerpFovY = true;
	m_vFovYLerp = { vRange.x, vRange.y };
	m_vLerpFovYTimer = { 0.f, fTimeMaximum };
}
void CCamera_Cinematic::Clear_Lerp_FovY()
{
	m_bLerpFovY = false;
	m_vFovYLerp = { 60.f, 60.f };
	m_vLerpFovYTimer = { 0.f, 0.f };
}
void CCamera_Cinematic::Transition(_float fTimeDelta)
{
	if (false == m_bIsCurrentTransition) {
		return;
	}
	m_vTransitionTime.x += fTimeDelta;
	_float fRatio = m_vTransitionTime.x / m_vTransitionTime.y;
	if (fRatio > 1.f) {
		fRatio = 1.f;
		m_bIsCurrentTransition = false;
	}

	_vector vFollowTarget = m_pFollowTargetPart->Get_WorldPostion();
	_vector vLookTarget = m_pLookTargetPart->Get_WorldPostion();

	_matrix xmMatTarget = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vFollowTarget, vLookTarget, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float4x4 matOrigin = *m_pTransformCom->Get_WorldMatrixPtr();
	_float4x4 matTarget = {}; XMStoreFloat4x4(&matTarget, xmMatTarget);
	_float4x4 matResult = {};

	CMyTools::MatrixLerp(&matOrigin, &matTarget, matResult, fRatio);

	m_pTransformCom->Set_WorldMatrix(matResult);
}
CCamera_Cinematic* CCamera_Cinematic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Cinematic* pInstance = new CCamera_Cinematic(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Cinematic");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CGameObject* CCamera_Cinematic::Clone(void* pArg, CGameObject* pOwner)
{
	CCamera_Cinematic* pInstance = new CCamera_Cinematic(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Cinematic");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamera_Cinematic::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLookTargetPart);
	SAFE_RELEASE(m_pFollowTargetPart);
}

#ifdef _DEBUG

void CCamera_Cinematic::Describe_Entity()
{
	GUI::Begin("CAMERA", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Camera_Cinematic_Describe")) {
		if (GUI::SmallButton("Trigger_RanrokIntro")) {
			_string strCutSceneName = "RanrokIntro";
			m_pInfoInstance->Active_Event(strCutSceneName);
		}
		m_pTransformCom->Describe_Entity();
		GUI::Text("LOOK_SRC %.2f, %.2f, %.2f", m_vLookPos_Src.x, m_vLookPos_Src.y, m_vLookPos_Src.z);
		GUI::Text("LOOK_DST %.2f, %.2f, %.2f", m_vLookPos_Dest.x, m_vLookPos_Dest.y, m_vLookPos_Dest.z);
		GUI::Text("FOLLOW_SRC %.2f, %.2f, %.2f", m_vFollowPos_Src.x, m_vFollowPos_Src.y, m_vFollowPos_Src.z);
		GUI::Text("FOLLOW_DST %.2f, %.2f, %.2f", m_vFollowPos_Dest.x, m_vFollowPos_Dest.y, m_vFollowPos_Dest.z);

		GUI::DragFloat2("VerticalAngle", (_float*)&m_vAccRotDegree_VerticalLocks, 1.f, 0.f, 89.f);
		GUI::Checkbox("TransitionLerp", &m_bEnable_TransitionLerp);
		if (true == m_bEnable_TransitionLerp) {
			GUI::SameLine(); GUI::Text("Translation : %.1f %.1f", m_vTransitionTime.x, m_vTransitionTime.y);
			GUI::SliderFloat("TranslationTime", &m_vTransitionTime.y, 0.1f, 3.f, "%.1f");
		}

		GUI::Checkbox("LookLerp", &m_bEnable_LookLerp);
		if (true == m_bEnable_LookLerp) {
			GUI::SameLine(); GUI::Text("Translation : %.1f %.1f", m_vLookLerpTime.x, m_vLookLerpTime.y);
			GUI::SliderFloat("LookTime", &m_vLookLerpTime.y, 0.1f, 3.f, "%.1f");
		}

		GUI::Checkbox("FollowLerp", &m_bEnable_FollowLerp);
		if (true == m_bEnable_FollowLerp) {
			GUI::SameLine(); GUI::Text("Translation : %.1f %.1f", m_vFollowLerpTime.x, m_vFollowLerpTime.y);
			GUI::SliderFloat("FollowTime", &m_vFollowLerpTime.y, 0.1f, 3.f, "%.1f");
		}

	}
	GUI::End();
}

#endif // _DEBUG
