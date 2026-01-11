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
#ifdef _DEBUG
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#endif // _DEBUG
		return;
	}
	else {
	}
}
HRESULT CCamera_Cinematic::Render() {

	RENDER eType = m_pGameInstance->Get_CurrentRenderPass();
	if (RENDER::NONBLEND == eType) {
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::WIREFRAME)))) {
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i))) {
				return E_FAIL;
			}
		}
	}
	else if (RENDER::NONLIGHT == eType) {
#ifdef _DEBUG
		m_Batch->Begin();

		_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
		_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

		_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x0000ff, 1.f);
		m_pSubShape->Draw(m_pLookTargetPart->Get_XMWorldMatrix(), ViewMatrix, ProjMatrix, vColor, nullptr, true);
		vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x00ff00, 1.f);
		m_pSubShape->Draw(m_pFollowTargetPart->Get_XMWorldMatrix(), ViewMatrix, ProjMatrix, vColor, nullptr, true);

		m_Batch->End();
#endif // _DEBUG


	}
	return S_OK;
}
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
	case TIMESOCKET_FUNC::TELEPORTATION:
	{
		m_pTransformCom->Set_WorldMatrix(pContents->pxTransform);
		m_pFollowTargetPart->Set_WorldPostion(m_pTransformCom->Get_State(STATE::POSITION));
		m_pLookTargetPart->Set_WorldPostion(m_pTransformCom->Get_State(STATE::POSITION) + m_pTransformCom->Get_State(STATE::LOOK));
		Clear_Lerp_Translation();
	} break;
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
		Rotation(XMLoadFloat4((_float4*)&pContents->pxTransform.q));
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
	}break;
	case TIMESOCKET_FUNC::DONT_LOOK_AT:
	{
		m_pLookTargetPart->Stop_Stalking(); 
	}break;
	case TIMESOCKET_FUNC::FOLLOW:
	{
		CUnit* pUnit = (CUnit*)pContents->pOtherTarget;
		const _char* pName = pContents->vParam_12.c_str();
		const _float4x4* pSocketMatrix = pUnit->Get_SocketMatrixPtr(pName);
		Set_FollowTarget(pUnit, pSocketMatrix);
	}break;
	case TIMESOCKET_FUNC::DONT_FOLLOW:
	{
		m_pFollowTargetPart->Stop_Stalking();
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


#ifdef _DEBUG
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.25f, 12, false, false));
#endif // _DEBUG
	return S_OK;
}
HRESULT CCamera_Cinematic::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Camera_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}


	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
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
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
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
	XMStoreFloat3(&m_vLerpTranslationStart, m_pFollowTarget->Get_WorldPostion());
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
		_vector vRotQ = XMQuaternionNormalize(XMQuaternionSlerp(XMLoadFloat4(&m_vLerpRotQStart), XMLoadFloat4(&m_vLerpRotQEnd), m_vLerpRotiationTimer.x / m_vLerpRotiationTimer.y));
		Rotation(vRotQ);
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
void CCamera_Cinematic::Rotation(_fvector vRotQ)
{
	_vector vPos = m_pFollowTargetPart->Get_WorldPostion();
	_vector vLookPos = m_pLookTargetPart->Get_WorldPostion();
	_float fDistance = XMVectorGetX(XMVector3Length(vLookPos - vPos));
	_vector vLookDir = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), vRotQ);

	if (true == m_pLookTargetPart->IsStalking()) { // 스토킹 중이면 팔로우가 움직이고 아니면 룩타겟이 움직임
		m_pFollowTargetPart->Set_WorldPostion(XMVectorSetW(vLookPos - (fDistance * vLookDir), 1.f));
	}
	else {
		m_pLookTargetPart->Set_WorldPostion(XMVectorSetW(vPos + (fDistance * vLookDir), 1.f));
	}
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
#ifdef _DEBUG
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
#endif // _DEBUG
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
		if (GUI::SmallButton("Trigger_CarriageIntro")) {
			_string strCutSceneName = "CarriageIntro";
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
