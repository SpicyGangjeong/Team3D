#include "pch.h"
#include "Dynamic_D6Joint.h"
#include "GameInstance.h"

CDynamic_D6Joint::CDynamic_D6Joint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CDynamic_D6Joint::CDynamic_D6Joint(const CDynamic_D6Joint& rhs)
	: CComponent(rhs)
{
}

HRESULT CDynamic_D6Joint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDynamic_D6Joint::Initialize(void* pArg)
{
	DYNAMICJOINT_DESC* pDesc = static_cast<DYNAMICJOINT_DESC*>(pArg);
	m_pJoint = m_pGameInstance->Create_PxD6Joint(pDesc->pActor0, pDesc->pActor1, *pDesc->pxJointWorldPos);
	return S_OK;
}
#ifdef _DEBUG

HRESULT CDynamic_D6Joint::Add_DebugShape()
{
	if (nullptr != m_pMainShape){
		m_pMainShape.reset();
	}
	m_pMainShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.05f, 10, false, false));

	return S_OK;
}
HRESULT CDynamic_D6Joint::Render()
{
	//PSX::PxTransform pxTranform = m_pRigidBody->getGlobalPose();
	//_matrix WorldMatrix = XMMatrixTranslation(m_vLocalTranslation.x, m_vLocalTranslation.y, m_vLocalTranslation.z) * XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorZero(), XMLoadFloat4((_float4*)&pxTranform.q), XMVectorSetW(XMLoadFloat3((_float3*)&pxTranform.p), 1.f));
	//_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	//_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	//_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x2fc48000, 1.f);

	//m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	return S_OK;
}

#endif // _DEBUG


CDynamic_D6Joint* CDynamic_D6Joint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamic_D6Joint* pInstance = new CDynamic_D6Joint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX("CDynamic_D6Joint Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CDynamic_D6Joint::Clone(void* pArg, CGameObject* pOwner)
{
	CDynamic_D6Joint* pInstance = new CDynamic_D6Joint(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX("CDynamic_D6Joint Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDynamic_D6Joint::Free()
{
	__super::Free();
}
#ifdef _DEBUG

void CDynamic_D6Joint::Describe_Entity()
{
}

#endif // _DEBUG
