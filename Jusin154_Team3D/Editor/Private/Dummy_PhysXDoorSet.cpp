#include "pch.h"
#include "Dummy_PhysXDoorSet.h"
#include "Dummy_PhysXFixedDoor.h"
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
	PSX::PxRigidDynamic* doorRigidDynamicActor = m_pDoorPart->Get_Actor();
	PSX::PxTransform doorGlobalPose = doorRigidDynamicActor->getGlobalPose();

	PSX::PxVec3 hingeLocalPositionInDoor = PSX::PxVec3(-1.5f, 0.0f, 0.0f);

	// 2) 힌지 월드 위치 = 도어 글로벌 포즈로 로컬 점 변환
	PSX::PxVec3 hingeWorldPosition = doorGlobalPose.transform(hingeLocalPositionInDoor);

	// 3) 힌지 회전축(월드 Y)
	PSX::PxVec3 hingeWorldAxis = PSX::PxVec3(0.f, 1.f, 0.f);

	// 4) Revolute는 프레임 X축이 회전축이므로 X->힌지축 회전
	PSX::PxQuat jointWorldOrientation = PSX::PxShortestRotation(PSX::PxVec3(1.f, 0.f, 0.f), hingeWorldAxis);

	// 5) 월드 프레임 구성
	PSX::PxTransform jointWorldFrame = PSX::PxTransform(hingeWorldPosition, jointWorldOrientation);

	// 6) 도어 로컬 프레임(도어 기준)
	PSX::PxTransform jointLocalFrameInDoor = doorGlobalPose.transformInv(jointWorldFrame);

	// 7) 조인트 생성
	m_pPSXJoint = (PSX::PxRevoluteJoint*)m_pGameInstance->Create_PxJoint(
		PHYSX_JOINT::REVOLUTE,
		doorRigidDynamicActor,
		jointLocalFrameInDoor,
		nullptr,
		jointWorldFrame
	);
	m_pPSXJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
	m_pPSXJoint->setDriveVelocity(2.f);
	m_pPSXJoint->setDriveForceLimit(5000.f);
	//m_pPSXJoint->setLimit(PSX::PxJointAngularLimitPair(XMConvertToRadians(-70.f), XMConvertToRadians(70.f)));
	//m_pPSXJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	//m_pPSXJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eLIMIT_ENABLED, false);

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
}

void CDummy_PhysXDoorSet::Describe_Entity()
{
}
