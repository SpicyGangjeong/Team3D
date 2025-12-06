#include "pch.h"
#include "Camera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }
    D3D11_VIEWPORT vp = {};
    _uint iTargetVP = 1;
    m_pContext->RSGetViewports(&iTargetVP, &vp);

    CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);
    m_fFovy = pDesc->fFovy;
    m_fAspect = vp.Width / vp.Height;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;
    m_bActive = false;
    m_iPriority = pDesc->iPriority;
    m_pCameraKey = pDesc->pCameraKey;


    m_pFollowTarget = pDesc->pFollowTarget;
    SAFE_ADDREF(m_pFollowTarget);
    m_pLookTarget = pDesc->pLookTarget;
    SAFE_ADDREF(m_pLookTarget);
    m_bEnable_TransitionLerp = pDesc->bEnableTransitionLerp;
    m_vTransitionTime = pDesc->vTransitionTime;
    m_bEnable_FollowLerp = pDesc->bEnableFollowLerp;
    m_vLookLerpTime = pDesc->vLookLerpTime;
    m_bEnable_LookLerp = pDesc->bEnableLookLerp;
    m_vFollowLerpTime = pDesc->vFollowLerpTime;

    return S_OK;
}

HRESULT CCamera::Bind_Matrices()
{
    if (true == m_bActive) {
        m_pGameInstance->Set_Transform(D3DTS::VIEW, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr())));
        m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
        Ready_Shadow();
    }
    return S_OK;
}

void CCamera::Transition(_float fTimeDelta)
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

    _vector vFollowTarget = m_pFollowTarget->Get_Component<CTransform>()->Get_State(STATE::POSITION);
    _vector vLookTarget = m_pLookTarget->Get_Component<CTransform>()->Get_State(STATE::POSITION);

    _matrix xmMatTarget = XMMatrixInverse(nullptr, XMMatrixLookAtLH(vFollowTarget, vLookTarget, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    _float4x4 matOrigin = *m_pTransformCom->Get_WorldMatrixPtr();
    _float4x4 matTarget = {}; XMStoreFloat4x4(&matTarget, xmMatTarget);
    _float4x4 matResult = {};

    CMyTools::MatrixLerp(&matOrigin, &matTarget, matResult, fRatio);

    m_pTransformCom->Set_WorldMatrix(matResult);
}

void CCamera::Active_Camera(pair<_float4, _float3>& pairTransitionInfo)
{
    if (true == m_bEnable_TransitionLerp) {
        m_pTransformCom->Set_WorldMatrix(XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(),
            XMLoadFloat4(&pairTransitionInfo.first), XMLoadFloat3(&pairTransitionInfo.second)));
        m_vTransitionTime.x = 0.f;
        m_bIsCurrentTransition = true;
    }
    m_bActive = true;
}

void CCamera::DeActive_Camera(pair<_float4, _float3>& pairTransition)
{
    m_bActive = false; 
    m_bIsCurrentTransition = false;
    _vector vScale{}, vRotq{}, vTrans{};
    XMMatrixDecompose(&vScale, &vRotq, &vTrans, m_pTransformCom->Get_XMWorldMatrix());
    XMStoreFloat4(&pairTransition.first, vRotq);
    XMStoreFloat3(&pairTransition.second, vTrans);
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{

    return S_OK;
}

HRESULT CCamera::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg))) {
        return E_FAIL;
    }

    return S_OK;
}

_bool CCamera::IsImportantThan(CCamera* pOther) const
{
    return m_iPriority > pOther->m_iPriority;
}

const _float* CCamera::Get_CurrentFar()
{
    return &m_fFar;
}

HRESULT CCamera::Ready_Shadow()
{
    if (nullptr == m_pLookTarget) {
        return S_OK;
    }
    ShadowDesc = *m_pGameInstance->Get_ShadowDesc();

    ShadowDesc.fFar = 200.f;
    ShadowDesc.fNear = 0.1f;
    ShadowDesc.fWidth = 192.f;
    ShadowDesc.fHeight = 168.f;

    XMStoreFloat4(&ShadowDesc.vAt, m_pLookTarget->Get_WorldPostion());
    _matrix matRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_vRollPichYaw.x), XMConvertToRadians(m_vRollPichYaw.y), XMConvertToRadians(m_vRollPichYaw.z));
    XMStoreFloat4(&ShadowDesc.vEye, XMLoadFloat4(&ShadowDesc.vAt) - matRotation.r[2] * (ShadowDesc.fFar * 0.25f));

#ifdef _DEBUG
    GUI::Begin("Renderer");
    if (GUI::CollapsingHeader("Near_Camera_ShadowDesc")) {
        //GUI::SliderFloat3("RPY", (_float*)&m_vRollPichYaw, -360.f, 360.f);
        GUI::DragFloat("Far", &ShadowDesc.fFar, 1.f, 20, 300.f);
        GUI::DragFloat("width", &ShadowDesc.fWidth, 1.f, 20.f, 100.f, "%.1f");
        GUI::DragFloat("height", &ShadowDesc.fHeight, 1.f, 20.f, 100.f, "%.1f");
        GUI::Text("%f %f %f %f", ShadowDesc.vAt.x, ShadowDesc.vAt.y, ShadowDesc.vAt.z, ShadowDesc.vAt.w);
    }
    GUI::End();
#endif // _DEBUG

    if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc))) {
        return E_FAIL;
    }
    return S_OK;
}

void CCamera::ZoomIn(_float fTimeDelta)
{
    if (m_fFovy > XMConvertToRadians(40.f))
    {
        m_fFovy -= (fTimeDelta * 1.2f);
    }
  
}

void CCamera::Set_Fov(_float fFovy, _float fTimeDelta,_bool& bZoomIn)
{
    if (fFovy > m_fFovy)
    {
        m_fFovy += (fTimeDelta * 0.6f);
        if (fFovy < m_fFovy) {
            m_fFovy = fFovy;
        }
    }
    else if(fFovy < m_fFovy)
    {
        m_fFovy -= (fTimeDelta * 0.6f);
        if (fFovy > m_fFovy) {
            m_fFovy = fFovy;
        }
    }

    if (fabsf(fFovy - m_fFovy) <= FLT_EPSILON3)
    {
        m_fFovy = fFovy;
        bZoomIn = false;
    }
}

_float CCamera::Get_Fov()
{
    return m_fFovy;
}

void CCamera::Free()
{
    __super::Free();

    SAFE_RELEASE(m_pFollowTarget);
    SAFE_RELEASE(m_pLookTarget);
}
