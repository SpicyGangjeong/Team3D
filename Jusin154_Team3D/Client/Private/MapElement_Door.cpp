#include "pch.h"
#include "MapElement_Door.h"

#include "GameInstance.h"
#include "RigidBody_Dynamic.h"
#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapElement_Door::CMapElement_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Door::CMapElement_Door(const CMapElement_Door& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Door::Initialize(void* pArg)
{
	ELEMENT_DOOR_DESC* pDesc = static_cast<ELEMENT_DOOR_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMapElement_Door::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Door::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CMapElement_Door::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Door::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	//if (FAILED(m_pRigidBody->Render())) {
	//	return E_FAIL;
	//}
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapElement_Door::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	ELEMENT_DOOR_DESC* pPhysXDummyDesc = static_cast<ELEMENT_DOOR_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pPhysXDummyDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pPhysXDummyDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pPhysXDummyDesc->vRotation.x), XMConvertToRadians(pPhysXDummyDesc->vRotation.y), XMConvertToRadians(pPhysXDummyDesc->vRotation.z));

	{ // RIGID_BODY
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::DOOR);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pActor = (PSX::PxRigidDynamic*)m_pRigidBody->Get_Actor();
	}
	
	PSX::PxTransform pxLocalPivot = {};
	pxLocalPivot.p = PSX::PxVec3();//PSX::PxVec3(0.f, pPhysXDummyDesc->vBoxLocalPosition.y * -0.5f, pPhysXDummyDesc->vBoxLocalPosition.z * 0.5f);
	XMStoreFloat4((_float4*)&pxLocalPivot.q, XMQuaternionRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XM_PIDIV2));

	PSX::PxTransform pxGlobalPivot = {};
	pxGlobalPivot.p = PSX::PxVec3(39.64f, 5.12f, 71.f);
	XMStoreFloat4((_float4*)&pxGlobalPivot.q, XMQuaternionRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XM_PIDIV2));
	//_vector vAxisWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//_vector vPhysXUp = XMVectorSet(1.0f, 0.0f, 0.0f, 0.f);

	//_vector vAxisWorldZ = XMVector3Cross(vAxisWorldUp, vPhysXUp);
	//_vector vAxisWorldY = XMVector3Cross(vAxisWorldZ, vAxisWorldUp);

	//_matrix rotMatrix = ;
	//_vector vRotQ = XMQuaternionRotationMatrix(rotMatrix);
	
	m_pJoint = (PSX::PxRevoluteJoint*)m_pGameInstance->Create_PxJoint(PHYSX_JOINT::REVOLUTE, nullptr, pxGlobalPivot, m_pActor, pxLocalPivot);

	m_pActor->setSolverIterationCounts(8, 2);
	m_pJoint->setLimit(PSX::PxJointAngularLimitPair(-XMConvertToRadians(120.f), XMConvertToRadians(120.f)));
	m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eLIMIT_ENABLED, true);

	m_pJoint->setDriveForceLimit(10.f);
	m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eDRIVE_ENABLED, true);

	m_pJoint->setConstraintFlag(PSX::PxConstraintFlag::eVISUALIZATION, true);
	m_pActor->setLinearVelocity(PSX::PxVec3(0.f, 0.f, 0.f));
	m_pActor->setAngularVelocity(PSX::PxVec3(0.f, 0.f, 0.f));
	m_pActor->putToSleep();
	return S_OK;
}

HRESULT CMapElement_Door::Bind_ShaderResources()
{
	_float3 vScale = m_pTransformCom->Get_Scale();
	_float4x4 WorldMatirx = {};
	PSX::PxTransform pxPos = m_pRigidBody->Get_GlobalPosition();
	XMStoreFloat4x4(&WorldMatirx, XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorZero(), XMLoadFloat4((_float4*)&pxPos.q), XMVectorSetW(XMLoadFloat3((_float3*)&pxPos.p), 1.f)));
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatirx))) {
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

CMapElement_Door* CMapElement_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Door* pInstance = new CMapElement_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Door");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Door::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Door* pInstance = new CMapElement_Door(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Door");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Door::Free()
{
	//if (nullptr != m_pRigidBody) {
	//	m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	//}
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Door::Describe_Entity()
{

	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(80);
	if (GUI::CollapsingHeader("DOOR")) {
		{
			m_pActor->wakeUp();
			_vector vWorldPos = Get_WorldPostion();
			PSX::PxTransform pxActorLocalPos = m_pJoint->getLocalPose(PSX::PxJointActorIndex::eACTOR1);
			PSX::PxVec3 vAxis = {};
			_float fAngle = {};
			pxActorLocalPos.q.toRadiansAndUnitAxis(fAngle, vAxis);
			GUI::Text("Pos : %.2f %.2f %.2f", pxActorLocalPos.p.x, pxActorLocalPos.p.y, pxActorLocalPos.p.z);
			GUI::Text("fAngle vAxis : %.2f %.2f %.2f %.2f", fAngle, vAxis.x, vAxis.y, vAxis.z);
			GUI::Text("Velo : %.2f", m_pJoint->getVelocity());
			if (GUI::DragFloat3("Pos ", (_float*)&pxActorLocalPos.p, 0.01f, -2.f, 2.f, "%.2f")) {
				m_pJoint->setLocalPose(PSX::PxJointActorIndex::eACTOR1, pxActorLocalPos);
			}
			if (GUI::DragFloat("fAngle", &fAngle, XMConvertToRadians(1.f), XMConvertToRadians(0.1f), 2.f * XM_2PI, "%.2f")) {
				pxActorLocalPos.q = PSX::PxQuat(fAngle, vAxis);
				m_pJoint->setLocalPose(PSX::PxJointActorIndex::eACTOR1, pxActorLocalPos);
			}
		} 
		{
			PSX::PxTransform pxRelativeLocalPos = m_pJoint->getRelativeTransform();
			PSX::PxVec3 vAxis = {};
			_float fAngle = {};
			pxRelativeLocalPos.q.toRadiansAndUnitAxis(fAngle, vAxis);
			GUI::Text("RelPos : %.2f %.2f %.2f", pxRelativeLocalPos.p.x, pxRelativeLocalPos.p.y, pxRelativeLocalPos.p.z);
			GUI::Text("fAngle vAxis : %.2f %.2f %.2f %.2f", fAngle, vAxis.x, vAxis.y, vAxis.z);
			
		}
		_float fDrive = m_pJoint->getDriveVelocity();
		_float fDriveDegree = XMConvertToDegrees(fDrive);
		if (GUI::DragFloat("Drive", &fDriveDegree, 1.f, -180.f, 180.f, "%.2f")) {
			fDrive = XMConvertToRadians(fDriveDegree);
			m_pJoint->setDriveVelocity(fDrive);
		}
		_float2 vDriveLimit = {};
		_float2 vSpring = {};
		{
			auto pxDriveLimit = m_pJoint->getLimit();
			vDriveLimit = { pxDriveLimit.lower,  pxDriveLimit.upper };
			vSpring.x = pxDriveLimit.stiffness;
			vSpring.y = pxDriveLimit.damping;
			//pxDriveLimit.;
		}
		if (GUI::DragFloat2("Limit ", (_float*)&vDriveLimit, XMConvertToRadians(1.f), -XMConvertToRadians(180.f), XMConvertToRadians(180.f), "%.2f")) {
			m_pJoint->setLimit(PSX::PxJointAngularLimitPair(vDriveLimit.x, vDriveLimit.y, PSX::PxSpring(vSpring.x, vSpring.y)));
		}
		if (GUI::DragFloat2("Spring Damping", (_float*)&vSpring, 1.f, 0.f, 100.f, "%.2f")) {
			m_pJoint->setLimit(PSX::PxJointAngularLimitPair(vDriveLimit.x, vDriveLimit.y, PSX::PxSpring(vSpring.x, vSpring.y)));
		}
		static _bool bKinematic = false;
		if (GUI::Checkbox("KINE", &bKinematic)) {
			m_pRigidBody->Set_Kinematic(bKinematic);
		}
		
	}
	GUI::End();
}
#endif // _DEBUG


