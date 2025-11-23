#include "pch.h"
#include "CamPosition_Arm.h"
#include "GameInstance.h"

CCamPosition_Arm::CCamPosition_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamPosition(pDevice, pContext)
{
}
CCamPosition_Arm::CCamPosition_Arm(const CCamPosition_Arm& rhs)
	: CCamPosition(rhs)
{
}
HRESULT CCamPosition_Arm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamPosition_Arm::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	CameraArm_DESC* pDesc = static_cast<CameraArm_DESC*>(pArg);
	m_fMouseSensor = pDesc->fMouseSensor;
	m_fDistance = pDesc->fDistance;

	m_pTransformCom->Set_State(STATE::POSITION, m_pParentTransformCom->Get_State(STATE::POSITION) + XMVectorSetW(XMLoadFloat3(&pDesc->fEye), 0.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->fAt), 1.f));

	return S_OK;
}

void CCamPosition_Arm::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::LOOK, XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f)));
	m_pTransformCom->Rotation(XMConvertToRadians(60.f), XMConvertToRadians(m_fAccDegreeY), 0.f);
	m_pTransformCom->Set_State(STATE::POSITION, m_pParentTransformCom->Get_State(STATE::POSITION) - XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * m_fDistance);
}

void CCamPosition_Arm::Update(_float fTimeDelta)
{

}

void CCamPosition_Arm::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamPosition_Arm::Render()
{
	return S_OK;
}

HRESULT CCamPosition_Arm::Ready_Components(void* pArg)
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

HRESULT CCamPosition_Arm::Bind_ShaderResources()
{
	return S_OK;
}

CCamPosition_Arm* CCamPosition_Arm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamPosition_Arm* pInstance = new CCamPosition_Arm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamPosition_Arm");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
CCamPosition_Arm* CCamPosition_Arm::Clone(void* pArg, class CGameObject* pOWner)
{
	CCamPosition_Arm* pInstance = new CCamPosition_Arm(*this);
	pInstance->m_pOwner = pOWner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamPosition_Arm");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CCamPosition_Arm::Free()
{
	__super::Free();
}
void CCamPosition_Arm::Describe_Entity()
{
}
