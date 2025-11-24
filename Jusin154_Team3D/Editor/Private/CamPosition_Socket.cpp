#include "pch.h"

#include "CamPosition_Socket.h"

CCamPosition_Socket::CCamPosition_Socket(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamPosition(pDevice, pContext)
{
}

CCamPosition_Socket::CCamPosition_Socket(const CCamPosition_Socket& rhs)
	:CCamPosition(rhs)
{
}

void CCamPosition_Socket::Priority_Update(_float fTimeDelta)
{
	/*_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}*/
	m_pTransformCom->Set_WorldMatrix(/*socketMatrix * */XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CCamPosition_Socket::Update(_float fTimeDelta)
{
}

void CCamPosition_Socket::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamPosition_Socket::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamPosition_Socket::Initialize(void* pArg)
{
	CAMERAPOSITION_PLAYER_DESC* pDesc = static_cast<CAMERAPOSITION_PLAYER_DESC*> (pArg);
	//m_pSocketMatrices = pDesc->pSocketMatrices;
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamPosition_Socket::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamPosition_Socket::Bind_ShaderResources()
{
	return S_OK;
}

CCamPosition_Socket* CCamPosition_Socket::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_Socket* pInstance = new CCamPosition_Socket(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_Socket");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CCamPosition_Socket* CCamPosition_Socket::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_Socket* pInstance = new CCamPosition_Socket(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_Socket");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCamPosition_Socket::Free()
{
	__super::Free();
}

void CCamPosition_Socket::Describe_Entity()
{
}
