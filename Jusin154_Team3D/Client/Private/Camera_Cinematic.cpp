#include "pch.h"
#include "Camera_Cinematic.h"
#include "InfoInstance.h"
#include "TimeSocket.h"
#include "Layer.h"


CCamera_Cinematic::CCamera_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Cinematic::CCamera_Cinematic(const CCamera_Cinematic& rhs)
	: CCamera(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

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
	m_bActive = true;
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
	Transition(fTimeDelta);
	Update_LerpTimer(fTimeDelta);

}
void CCamera_Cinematic::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
}
HRESULT CCamera_Cinematic::Render()
{
	return S_OK;
}

void CCamera_Cinematic::Update_LerpTimer(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
	Lerp_Translation(fTimeDelta);
	Lerp_FovY(fTimeDelta);
	if (true == m_bEnable_LookLerp) {
		m_vLookLerpTime.x += fTimeDelta;
		if (m_vLookLerpTime.y <= m_vLookLerpTime.x) {
			m_vLookLerpTime.x = 0.f;
			XMStoreFloat3(&m_vLookPos_Src, XMLoadFloat3(&m_vLookPos_Dest));
			XMStoreFloat3(&m_vLookPos_Dest, m_pLookTarget->Get_WorldPostion());
		}
	}

	if (true == m_bEnable_FollowLerp) {
		m_vFollowLerpTime.x += fTimeDelta;
		if (m_vFollowLerpTime.y <= m_vFollowLerpTime.x) {
			m_vFollowLerpTime.x = 0.f;
			XMStoreFloat3(&m_vFollowPos_Src, m_pTransformCom->Get_State(STATE::POSITION));
			XMStoreFloat3(&m_vFollowPos_Dest, m_pFollowTarget->Get_WorldPostion());
		}
	}

}

void CCamera_Cinematic::Set_Priority(_uint iPriority)
{
	m_iPriority = iPriority;
}

void CCamera_Cinematic::Set_LookTarget(CGameObject* pTarget)
{
	SAFE_RELEASE(m_pLookTarget);
	m_pLookTarget = pTarget;
	SAFE_ADDREF(m_pLookTarget);
}

void CCamera_Cinematic::Set_FollowTarget(CGameObject* pTarget)
{
	SAFE_RELEASE(m_pFollowTarget);
	m_pFollowTarget = pTarget;
	SAFE_ADDREF(m_pFollowTarget);
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
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3((_float3*)&pContents->pxTransform.p), 1.f));
		m_pTransformCom->RotationQ(pContents->pxTransform.q);
		Clear_Lerp_Translation();
		break;
	case TIMESOCKET_FUNC::TRANSLATION_LERP:
	{
		Clear_Lerp_Translation();
		Start_Lerp_Translation(pContents->vParam_11.x, pContents->pxTransform);
	} break;
	case TIMESOCKET_FUNC::LOOK_AT:
	{
		Set_LookTarget(pContents->pOtherTarget);
	}break;
	case TIMESOCKET_FUNC::DONT_LOOK_AT:
	{
		Set_LookTarget(pContents->pOtherTarget);
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

	return S_OK;
}
HRESULT CCamera_Cinematic::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
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
		_float4x4 ResultMatrix = {};
		CMyTools::MatrixLerp(&m_vLerpTranslationStart, &m_vLerpTranslationEnd, &m_vLerpRotQStart, &m_vLerpRotQEnd, 
			ResultMatrix, m_vLerpTranslationTimer.x / m_vLerpTranslationTimer.y);
		m_pTransformCom->Set_WorldMatrix(ResultMatrix);
	}
}
void CCamera_Cinematic::Start_Lerp_Translation(_float fTimeMaximum, PSX::PxTransform pxTransform)
{
	_float3 vTrans = { pxTransform.p.x , pxTransform.p.y, pxTransform.p.z };
	_float4 vRotQ = { pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w };
	Start_Lerp_Translation(fTimeMaximum, vTrans, vRotQ);
}
void CCamera_Cinematic::Start_Lerp_Translation(_float fTimeMaximum, _float3& vTrans, _float4& vRotQ)
{
	m_bLerpTranslation = { fTimeMaximum != 0 };
	m_vLerpTranslationTimer = { 0.f, fTimeMaximum };
	m_pTransformCom->Compress_WorldMatrix(m_vLerpTranslationStart, m_vLerpRotQStart);
	m_vLerpTranslationEnd = vTrans;
	m_vLerpRotQEnd = vRotQ;
}
void CCamera_Cinematic::Clear_Lerp_Translation()
{
	m_bLerpTranslation = { false };
	m_vLerpTranslationTimer = {};
	m_vLerpTranslationStart = {};
	m_vLerpTranslationEnd = {};
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
