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
	if (m_vKinematicTimer.x != 0) {
		m_vKinematicTimer.x += fTimeDelta;
		if (m_vKinematicTimer.y < m_vKinematicTimer.x) {
			m_vKinematicTimer.x = 0.f;
		}
	}
	else {
		ActingIdle();
	}
}

void CMapElement_Door::ActingIdle()
{
	_float fCurrentDegree = XMConvertToDegrees(m_pJoint->getAngle());

	if (fabsf(fCurrentDegree) >= 2.f) { // 각도가 벌어져 있는데
		if (false == m_bDrive) { // 드라이브 중이 아니면 드라이브시킴
			m_bDrive = true;
			m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
			m_pJoint->setDriveForceLimit(120.f);
			
			// close
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::DOOR_CLOSE, SD_CHANNEL_GROUP::OBJECT, false, 0.8f);
		}
		if (true == m_bDrive) { // 드라이브 중이면
			if (fCurrentDegree > 0.f) {
				m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
				m_pJoint->setDriveVelocity(-fabsf(fCurrentDegree) * 0.2f);
			}
			else {
				m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eDRIVE_ENABLED, true);
				m_pJoint->setDriveVelocity(fabsf(fCurrentDegree) * 0.2f);
			}
		}
	}
	else { // 각도가 안벌어져 있음
		if (true == m_bDrive && fabsf(fCurrentDegree) < 0.3f) { // 드라이브 중지 트리거
			m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eDRIVE_ENABLED, false);
		}
	}
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
	if (m_bDead == true) {
		return S_OK;
	}
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

void CMapElement_Door::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (0.f != m_vKinematicTimer.x) {
		m_vKinematicTimer.x = FLT_EPSILON3;
		return;
	}
	// open
	m_pGameInstance->Sound_Play(SOUND::SD_KIND::DOOR_OPEN, SD_CHANNEL_GROUP::OBJECT, false, 0.8f);
	m_vKinematicTimer.x = FLT_EPSILON3;
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
		m_pRigidBody->Move_Kinematic(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_QuarternionVector(), true);
	}
	PSX::PxTransform doorWorldPose = m_pActor->getGlobalPose();
	PSX::PxVec3 hingePivotWorld = PSX::PxVec3(39.7f, 5.12f, 71.12f);
	PSX::PxVec3 hingeAxisWorld = doorWorldPose.q.rotate(PSX::PxVec3(0.f, 1.f, 0.f)).getNormalized();
	PSX::PxTransform jointWorldPose;
	jointWorldPose.p = hingePivotWorld;
	jointWorldPose.q = CMyTools::Calc_RevJointPosFromWorldHingeAxis(hingeAxisWorld);
	PSX::PxTransform localFrameDoor = doorWorldPose.transformInv(jointWorldPose);
	
	m_pActor->setSolverIterationCounts(8, 1);
	m_pJoint = (PSX::PxRevoluteJoint*)m_pGameInstance->Create_PxJoint(PHYSX_JOINT::REVOLUTE, nullptr, jointWorldPose, m_pActor, localFrameDoor);

	m_pJoint->setLimit(PSX::PxJointAngularLimitPair(-XMConvertToRadians(100.f), XMConvertToRadians(100.f)));
	m_pJoint->setRevoluteJointFlag(PSX::PxRevoluteJointFlag::eLIMIT_ENABLED, true);

	m_pJoint->setDriveForceLimit(10.f);

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
	GUI::Begin("PhysX", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Door")) {
		GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
		GUI::Text("m_vKinematicTimer %.3f", m_vKinematicTimer.x);
		_float fCurrentDegree = XMConvertToDegrees(m_pJoint->getAngle());
		GUI::Text("%.2f", fCurrentDegree);
		GUI::Text("%.2f", m_pJoint->getVelocity());
		if (GUI::CollapsingHeader("DOOR")) {
			{
				GUI::Text("Velo : %.2f", m_pJoint->getVelocity());
				PSX::PxTransform pxActorLocalPos = m_pJoint->getLocalPose(PSX::PxJointActorIndex::eACTOR1);
				if (true == CMyTools::DescribePxTransform(pxActorLocalPos, (size_t)this)) {
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
			m_pRigidBody->Describe_Entity();
		}
	}
	GUI::End();
}
#endif // _DEBUG


