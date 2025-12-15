#include "pch.h"
#include "Dummy_PhysXDoorSet.h"
#include "Dummy_PhysXFixedDoor.h"
#include "Dummy_PhysXDoorFrame.h"
#include "GameInstance.h"


CDummy_PhysXDoorSet::CDummy_PhysXDoorSet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CDummy_PhysXDoorSet::CDummy_PhysXDoorSet(const CDummy_PhysXDoorSet& rhs)
	: CContainerObject(rhs)
{
}

HRESULT CDummy_PhysXDoorSet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXDoorSet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_PartObject(pArg))) {
		return E_FAIL;
	}

	PSX::PxTransform pxFrameTransform = {};
	PSX::PxTransform pxDoorTransform = {};
	PSX::PxRigidDynamic* pFrameBody = m_pFramePart->Get_Actor();
	PSX::PxRigidDynamic* pDoorBody = m_pDoorPart->Get_Actor();

	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	PSX::PxVec3 vHingeLocalPos = {};
	XMStoreFloat3((_float3*)&vHingeLocalPos, vPos);
	pxFrameTransform.p = pFrameBody->getGlobalPose().transformInv(vHingeLocalPos);
	pxDoorTransform.p	= pDoorBody->getGlobalPose().transformInv(vHingeLocalPos);
	
	pxDoorTransform.q = pxFrameTransform.q = PSX::PxQuat(XM_PIDIV2, PSX::PxVec3(0.f, 0.f, 1.f));

	m_pPSXJoint = m_pGameInstance->Create_PxRevoluteJoint(pFrameBody, pxFrameTransform, pDoorBody ,pxDoorTransform);
	
	return S_OK;
}

void CDummy_PhysXDoorSet::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CDummy_PhysXDoorSet::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CDummy_PhysXDoorSet::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXDoorSet::Render()
{
	return S_OK;
}

HRESULT CDummy_PhysXDoorSet::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pPhysXDummyDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pPhysXDummyDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pPhysXDummyDesc->vRotRPY.x, pPhysXDummyDesc->vRotRPY.y, pPhysXDummyDesc->vRotRPY.z);

	return S_OK;
}

HRESULT CDummy_PhysXDoorSet::Ready_PartObject(void* pArg)
{
	{
		CDummy_PhysXDoorFrame::PHYSXDUMMY_DESC Desc{};
		Desc.pParentTransform = m_pTransformCom;
		Desc.vPos = { -10.f, 3.f, -10.f };
		Desc.iSubKind = 23;
		if (FAILED(Add_PartObject<CDummy_PhysXDoorFrame>("DoorFrame", g_iStaticLevel, &m_pFramePart, &Desc))) {
			return E_FAIL;
		}
	}
	{
		CDummy_PhysXFixedDoor::PHYSXDUMMY_DESC Desc{};
		Desc.pParentTransform = m_pTransformCom;
		Desc.vPos = { 0.f, 3.f, 0.f };
		Desc.iSubKind = 25;
		if (FAILED(Add_PartObject<CDummy_PhysXFixedDoor>("DoorFixed", g_iStaticLevel, &m_pDoorPart, &Desc))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CDummy_PhysXDoorSet::Bind_ShaderResources()
{
	return S_OK;
}

CDummy_PhysXDoorSet* CDummy_PhysXDoorSet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXDoorSet* pInstance = new CDummy_PhysXDoorSet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXDoorSet");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXDoorSet::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXDoorSet* pInstance = new CDummy_PhysXDoorSet(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXDoorSet");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXDoorSet::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDoorPart);
	SAFE_RELEASE(m_pFramePart);
}

void CDummy_PhysXDoorSet::Describe_Entity()
{
}
