#include "pch.h"
#include "Camera_Gaze.h"

CCamera_Gaze::CCamera_Gaze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Gaze::CCamera_Gaze(const CCamera_Gaze& rhs)
	: CCamera(rhs)
{
}

void CCamera_Gaze::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
	if (false == m_bIsCurrentTransition) {
		if (m_bEnable_FollowLerp) {
			_vector vNewPos = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vFollowPos_Src), XMLoadFloat3(&m_vFollowPos_Dest), m_vFollowLerpTime.x / m_vFollowLerpTime.y), 1.f);
			m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
		}
		else {
			m_pTransformCom->Set_State(STATE::POSITION, m_pFollowTarget->Get_Component<CTransform>()->Get_State(STATE::POSITION));
		}
		if (m_bEnable_LookLerp) {
			_vector vNewLook = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vLookPos_Src), XMLoadFloat3(&m_vLookPos_Dest), m_vLookLerpTime.x / m_vLookLerpTime.y), 1.f);
			m_pTransformCom->LookAt(vNewLook);
		}
		else {
			m_pTransformCom->LookAt(m_pLookTarget->Get_Component<CTransform>()->Get_State(STATE::POSITION));
		}
	}
	else {

	}
	m_bActive = true;
	__super::Bind_Matrices();
}

void CCamera_Gaze::Update(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
	Transition(fTimeDelta);
	Update_LerpTimer(fTimeDelta);

	Describe_Entity();
}

void CCamera_Gaze::Update_LerpTimer(Engine::_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
	if (true == m_bEnable_LookLerp) {
		m_vLookLerpTime.x += fTimeDelta;
		if (m_vLookLerpTime.y <= m_vLookLerpTime.x) {
			m_vLookLerpTime.x = 0.f;
			m_bEnable_LookLerp = false;
			XMStoreFloat3(&m_vLookPos_Src, XMLoadFloat3(&m_vLookPos_Dest));
			XMStoreFloat3(&m_vLookPos_Dest, m_pLookTarget->Get_WorldPostion());
		}
	}

	if (true == m_bEnable_FollowLerp) {
		m_vFollowLerpTime.x += fTimeDelta;
		if (m_vFollowLerpTime.y <= m_vFollowLerpTime.x) {
			m_vFollowLerpTime.x = 0.f;
			m_bEnable_FollowLerp = false;
			XMStoreFloat3(&m_vFollowPos_Src, m_pTransformCom->Get_State(STATE::POSITION));
			XMStoreFloat3(&m_vFollowPos_Dest, m_pFollowTarget->Get_WorldPostion());
		}
	}
}

void CCamera_Gaze::Enable_FollowLerp()
{
	m_bEnable_FollowLerp = true;
	m_vFollowLerpTime.x = 0.f;
	XMStoreFloat3(&m_vFollowPos_Src, m_pFollowTarget->Get_WorldPostion());
}

void CCamera_Gaze::Enable_LookLerp()
{
	m_bEnable_LookLerp = true;
	m_vLookLerpTime.x = 0.f;
	XMStoreFloat3(&m_vLookPos_Src, m_pLookTarget->Get_WorldPostion());
}

void CCamera_Gaze::Toggle_Priority()
{
	if (m_iPriority == 99) {
		m_iPriority = 51;
	}
	else {
		m_iPriority = 99;
		m_pGameInstance->Bind_Camera(g_iStaticLevel, CAMERA_SHOULDER, false);
	}
}

void CCamera_Gaze::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive) {
		return;
	}
}
HRESULT CCamera_Gaze::Render()
{
	return S_OK;
}
HRESULT CCamera_Gaze::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCamera_Gaze::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CCamera_Gaze::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CCamera_Gaze::Bind_ShaderResources()
{
	return S_OK;
}
CCamera_Gaze* CCamera_Gaze::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Gaze* pInstance = new CCamera_Gaze(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Gaze");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CGameObject* CCamera_Gaze::Clone(void* pArg, CGameObject* pOwner)
{
	CCamera_Gaze* pInstance = new CCamera_Gaze(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Gaze");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamera_Gaze::Free()
{
	__super::Free();
}
void CCamera_Gaze::Describe_Entity()
{
	GUI::Begin("Camera_Gaze_Describe");
	m_pTransformCom->Describe_Entity();
	GUI::Text("LOOK_SRC %.2f, %.2f, %.2f", m_vLookPos_Src.x, m_vLookPos_Src.y, m_vLookPos_Src.z);
	GUI::Text("LOOK_DST %.2f, %.2f, %.2f", m_vLookPos_Dest.x, m_vLookPos_Dest.y, m_vLookPos_Dest.z);
	GUI::Text("FOLLOW_SRC %.2f, %.2f, %.2f", m_vFollowPos_Src.x, m_vFollowPos_Src.y, m_vFollowPos_Src.z);
	GUI::Text("FOLLOW_DST %.2f, %.2f, %.2f", m_vFollowPos_Dest.x, m_vFollowPos_Dest.y, m_vFollowPos_Dest.z);
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

	GUI::End();
}
