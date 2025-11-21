#include "pch.h"

#include "CamPosition_Player.h"

CCamPosition_Player::CCamPosition_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamPosition(pDevice, pContext)
{
}

CCamPosition_Player::CCamPosition_Player(const CCamPosition_Player& rhs)
	:CCamPosition(rhs)
{
}

void CCamPosition_Player::Priority_Update(_float fTimeDelta)
{
	/*_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}*/
	m_pTransformCom->Set_WorldMatrix(/*socketMatrix * */XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CCamPosition_Player::Update(_float fTimeDelta)
{
}

void CCamPosition_Player::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamPosition_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamPosition_Player::Initialize(void* pArg)
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

HRESULT CCamPosition_Player::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) 
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamPosition_Player::Bind_ShaderResources()
{
	return S_OK;
}

CCamPosition_Player* CCamPosition_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_Player* pInstance = new CCamPosition_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_Player");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CCamPosition_Player* CCamPosition_Player::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_Player* pInstance = new CCamPosition_Player(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_Player");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCamPosition_Player::Free()
{
	__super::Free();
}

void CCamPosition_Player::Describe_Entity()
{
}
