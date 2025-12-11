#include "pch.h"
#include "MiniMap_Camera.h"
#include "InfoInstance.h"

CMiniMap_Camera::CMiniMap_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CMiniMap_Camera::CMiniMap_Camera(const CMiniMap_Camera& rhs)
	: CCamera(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

void CMiniMap_Camera::Priority_Update(_float fTimeDelta)
{
}

void CMiniMap_Camera::Update(_float fTimeDelta)
{
}

void CMiniMap_Camera::Update_LerpTimer(Engine::_float fTimeDelta)
{
}

void CMiniMap_Camera::Enable_FollowLerp()
{
}

void CMiniMap_Camera::Enable_LookLerp()
{
}

void CMiniMap_Camera::Toggle_Priority()
{
}

void CMiniMap_Camera::Late_Update(_float fTimeDelta)
{
}

HRESULT CMiniMap_Camera::Render()
{
	return S_OK;
}

HRESULT CMiniMap_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniMap_Camera::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMiniMap_Camera::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CMiniMap_Camera::Bind_ShaderResources()
{
	return S_OK;
}

CMiniMap_Camera* CMiniMap_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniMap_Camera* pInstance = new CMiniMap_Camera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMiniMap_Camera");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CGameObject* CMiniMap_Camera::Clone(void* pArg, CGameObject* pOwner)
{
	CMiniMap_Camera* pInstance = new CMiniMap_Camera(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMiniMap_Camera");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CMiniMap_Camera::Free()
{
	__super::Free();
}

void CMiniMap_Camera::Describe_Entity()
{
}
