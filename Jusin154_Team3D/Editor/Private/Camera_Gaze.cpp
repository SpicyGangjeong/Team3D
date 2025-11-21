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
	if (false == m_bIsCurrentTransition) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pFollowTarget->Get_Component<CTransform>()->Get_State(STATE::POSITION));
		m_pTransformCom->LookAt(m_pLookTarget->Get_Component<CTransform>()->Get_State(STATE::POSITION));
	}
	m_bActive = true;
	__super::Bind_Matrices();
}

void CCamera_Gaze::Update(_float fTimeDelta)
{
}

void CCamera_Gaze::Late_Update(_float fTimeDelta)
{
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

}
