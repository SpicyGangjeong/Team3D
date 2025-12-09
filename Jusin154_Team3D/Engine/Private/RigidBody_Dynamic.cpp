#include "pch.h"
#include "RigidBody_Dynamic.h"
#include "GameInstance.h"

CRigidBody_Dynamic::CRigidBody_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):
    CRigidBody(pDevice, pContext)
{
}

CRigidBody_Dynamic::CRigidBody_Dynamic(const CRigidBody_Dynamic& rhs) :
    CRigidBody(rhs),
    m_vMatInfo(rhs.m_vMatInfo),
    m_vhalfGeometryInfo(rhs.m_vhalfGeometryInfo),
	m_PxMassCenter(rhs.m_PxMassCenter),
	m_eLockFlag(rhs.m_eLockFlag),
	m_vDamping(rhs.m_vDamping)
{
}

#ifdef _DEBUG
HRESULT CRigidBody_Dynamic::Render()
{
	_matrix  WorldMatrix = XMMatrixTranslation(m_vLocalTranslation.x, m_vLocalTranslation.y, m_vLocalTranslation.z) * m_pTransform->Get_XMWorldMatrix();
	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x2fc48000, 1.f);

	switch (m_eActorType)
	{
	case ACTOR::BOX:
	{
		m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	case ACTOR::CAPSULE:
	{
		m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
		_matrix WorldUp = XMMatrixTranslationFromVector(m_pTransform->Get_State(STATE::UP) * m_vhalfGeometryInfo.y);
		_matrix WorldDown = XMMatrixTranslationFromVector(m_pTransform->Get_State(STATE::UP) * -m_vhalfGeometryInfo.y);
		m_pSubShape->Draw(WorldUp * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
		m_pSubShape->Draw(WorldDown * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	case ACTOR::SPHERE:
	{
		m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	default:
		break;
	}
	return S_OK;
}
#endif 
void CRigidBody_Dynamic::Set_HalfGeometryInfo(_float3 vhalfGeometryInfo)
{
	m_vhalfGeometryInfo = vhalfGeometryInfo;
	PSX::PxShape* pShape = { nullptr };
	m_pRigidBody->getShapes(&pShape, m_pRigidBody->getNbShapes());

	switch (m_eActorType)
	{
	case ACTOR::BOX:
	{
		PSX::PxBoxGeometry Geometry((const PSX::PxBoxGeometry&)pShape->getGeometry());
		Geometry.halfExtents = { vhalfGeometryInfo.x, vhalfGeometryInfo.y, vhalfGeometryInfo.z };
		if (false == Geometry.isValid()) {
			assert(false);
		}
		pShape->setGeometry(Geometry);
	} break;
	case ACTOR::CAPSULE:
	{
		PSX::PxCapsuleGeometry Geometry((const PSX::PxCapsuleGeometry&)pShape->getGeometry());
		Geometry.halfHeight = { vhalfGeometryInfo.y };
		Geometry.radius = { vhalfGeometryInfo.x };
		if (false == Geometry.isValid()) {
			assert(false);
		}
		pShape->setGeometry(Geometry);
	} break;
	case ACTOR::SPHERE:
	{
		PSX::PxSphereGeometry Geometry((const PSX::PxSphereGeometry&)pShape->getGeometry());
		Geometry.radius = { vhalfGeometryInfo.x };
		if (false == Geometry.isValid()) {
			assert(false);
		}
		pShape->setGeometry(Geometry);
	} break;
	default:
		break;
	}
#ifdef _DEBUG
	if (nullptr != m_pMainShape) {
		m_pMainShape.reset();
		m_pMainShape = nullptr;
	}
	if (nullptr != m_pSubShape) {
		m_pSubShape.reset();
		m_pSubShape = nullptr;
	}
	Add_DebugShape();
#endif // _DEBUG
}
void CRigidBody_Dynamic::Move_LocalPos(_float4 vNewRotQ, _float3 vNewTranslation)
{
	m_vLocalRotQ = vNewRotQ;
	m_vLocalTranslation = vNewTranslation;
	PSX::PxShape* pShape = { nullptr };
	m_pRigidBody->getShapes(&pShape, m_pRigidBody->getNbShapes());
	pShape->setLocalPose(PSX::PxTransform(vNewTranslation.x, vNewTranslation.y, vNewTranslation.z, PSX::PxQuat(vNewRotQ.x, vNewRotQ.y, vNewRotQ.z, vNewRotQ.w)));
	Add_DebugShape();
}
// _DEBUG
void CRigidBody_Dynamic::Add_Force(_fvector vForce, PSX::PxForceMode::Enum eType)
{
	PSX::PxVec3 vPxForce = {};
	XMStoreFloat3((_float3*)&vPxForce, vForce);
	m_pRigidBody->addForce(vPxForce, eType);
}

void CRigidBody_Dynamic::Add_Torque(_fvector vDirection, PSX::PxForceMode::Enum eType)
{
	PSX::PxVec3 vPxTorque = {};
	XMStoreFloat3((_float3*)&vPxTorque, vDirection);
	m_pRigidBody->addTorque(vPxTorque, eType);
}

void CRigidBody_Dynamic::Set_Kinematic(_bool bKinematic)
{
	m_pRigidBody->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, bKinematic);
}

HRESULT CRigidBody_Dynamic::ConvertToCCT(CCharacter_Controller& CCTOriginal)
{
	m_pTransform->Set_WorldMatrix(m_pRigidBody->getGlobalPose());
	CCTOriginal.Set_Position(m_pTransform->Get_State(STATE::POSITION));

	m_pRigidBody->setAngularVelocity(PSX::PxZERO()); // DO 객체 이동 제거
	m_pRigidBody->setLinearVelocity(PSX::PxZERO());

	Detach_Actor();
	CCTOriginal.SetActive(true);

	return E_FAIL;
}

void CRigidBody_Dynamic::Detach_Actor()
{
	m_pGameInstance->Detach_Actor(*m_pRigidBody); // 현재 액터 비활성화
	m_bActive = false;
}

_vector CRigidBody_Dynamic::Get_Position()
{
	PSX::PxVec3 pxPos = m_pRigidBody->getGlobalPose().p;
	_vector vPos = { pxPos.x, pxPos.y, pxPos.z };
	return vPos;
}

_float3 CRigidBody_Dynamic::Get_FootPosition()
{
	PSX::PxTransform pxTransform = m_pRigidBody->getGlobalPose();
	switch (m_eActorType)
	{
	case Engine::ACTOR::BOX:
		pxTransform.p.y -= m_vhalfGeometryInfo.y;
		break;
	case Engine::ACTOR::CAPSULE:
		pxTransform.p.y -= m_vhalfGeometryInfo.y * 0.5f + m_vhalfGeometryInfo.x;
		break;
	case Engine::ACTOR::SPHERE:
		pxTransform.p.y -= m_vhalfGeometryInfo.x;
		break;
	default:
		break;
	}
	_float3 vOut = { pxTransform.p.x , pxTransform.p.y , pxTransform.p.z };
	return vOut;
}

PSX::PxTransform CRigidBody_Dynamic::Get_FootPositionPxTransform()
{
	PSX::PxTransform pxTransform = m_pRigidBody->getGlobalPose();
	_float3 vFootPos = Get_FootPosition();
	pxTransform.p = { vFootPos.x, vFootPos.y, vFootPos.z };
	return pxTransform;
}

HRESULT CRigidBody_Dynamic::Initialize_Prototype(RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc)
{
	m_eActorType		= Desc.eType;
	m_ePxRigidBodyFlags = Desc.ePxRigidBodyFlags;
	m_ePxShapeFlags		= Desc.ePxShapeFlags;
	m_eMatType			= Desc.ePxMaterialTypes;
	m_vMatInfo			= Desc.vMatInfo;
	m_fContactOffset	= Desc.fContactOffset;
	m_vhalfGeometryInfo = Desc.vhalfGeometryInfo;
	m_fDensity			= Desc.fDensity;
	m_eLockFlag			= Desc.eLockFlag;
	m_PxMassCenter		= Desc.pxMassCenter;
	m_vDamping			= Desc.vAutoDamping;
	m_vLocalRotQ = Desc.vLocalRotQ;
	m_vLocalTranslation = Desc.vLocalTranslation;

	return S_OK;
}

HRESULT CRigidBody_Dynamic::Initialize(void* pArg)
{
	m_tagData.eKind = PHYSX_KIND::BODY_DYNAMIC;
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Add_DebugShape())) {
		return E_FAIL;
	}
#endif // _DEBUG
	m_tagData.pOwner = m_pOwner;
	XMStoreFloat4x4(&m_tagData.BeforeMatrix, m_pTransform->Get_XMWorldMatrix());
	m_tagData.pBody = this;
	m_pRigidBody = m_pGameInstance->Add_DynamicActor(*this);
	m_pRigidBody->userData = &m_tagData;
	m_pRigidBody->setCMassLocalPose(m_PxMassCenter);
	m_pRigidBody->setRigidDynamicLockFlags(m_eLockFlag);
	m_pRigidBody->setLinearDamping(m_vDamping.x);
	m_pRigidBody->setAngularDamping(m_vDamping.y);

	Move_LocalPos(m_vLocalRotQ, m_vLocalTranslation);
	//m_pRigidBody->joint;
	//PSX::PxRevol

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRigidBody_Dynamic::Add_DebugShape()
{
	if(nullptr != m_pMainShape)
		m_pMainShape.reset();

	switch (m_eActorType)
	{
	case ACTOR::BOX:
		_float3 vDXGeometricSize = { m_vhalfGeometryInfo.x * 2.f, m_vhalfGeometryInfo.y * 2.f, m_vhalfGeometryInfo.z * 2.f };
		m_pMainShape = (GeometricPrimitive::CreateBox(m_pContext, vDXGeometricSize, false, false));
		break;
	case ACTOR::CAPSULE:
		m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, m_vhalfGeometryInfo.x * 2.f, 10, false, false));
		m_pMainShape = (GeometricPrimitive::CreateCylinder(m_pContext, m_vhalfGeometryInfo.y * 2.f, m_vhalfGeometryInfo.x * 2.f, 10, false));
		break;
	case ACTOR::SPHERE:
		m_pMainShape = (GeometricPrimitive::CreateSphere(m_pContext, m_vhalfGeometryInfo.x * 2.f, 10, false, false));
		break;
	default:
		break;
	}
	return S_OK;
}
#endif // _DEBUG
CRigidBody_Dynamic* CRigidBody_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc)
{
	CRigidBody_Dynamic* pInstance = new CRigidBody_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc))) {
		MSG_BOX("CRigidBody_Dynamic Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody_Dynamic::Clone(void* pArg, CGameObject* pOwner)
{
	CRigidBody_Dynamic* pInstance = new CRigidBody_Dynamic(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX("CRigidBody_Dynamic Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRigidBody_Dynamic::Free()
{
	__super::Free();
	if (nullptr != m_pRigidBody) {
		m_pRigidBody = nullptr;
	}
}
#ifdef _DEBUG

void CRigidBody_Dynamic::Describe_Entity()
{
	GUI::BeginChild(("Child ID: " + to_string((size_t)(this))).c_str());
	GUI::Text("ReSize"); GUI::SameLine(); 
	if (GUI::DragFloat3("##Size", (_float*)&m_vhalfGeometryInfo, 0.01f, 0.01f, 10.f, "%.3f")) {
		m_vhalfGeometryInfo.x = max(0.01f, m_vhalfGeometryInfo.x);
		m_vhalfGeometryInfo.y = max(0.01f, m_vhalfGeometryInfo.y);
		m_vhalfGeometryInfo.z = max(0.01f, m_vhalfGeometryInfo.z);
		Set_HalfGeometryInfo(m_vhalfGeometryInfo);
	}
	GUI::Text("Translation"); GUI::SameLine();
	if (GUI::DragFloat3("##LocalPos", (_float*)&m_vLocalTranslation, 0.125f, -10.f, 10.f, "%.4f")) {
		Move_LocalPos(m_vLocalRotQ, m_vLocalTranslation);
	}
	_vector q = XMLoadFloat4(&m_vLocalRotQ);

	// 기본 방향벡터 (엔진 기준)
	_vector forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), q);
	_vector right = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), q);
	_vector up = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), q);

	// --- RPY 계산 ---
	_float fYaw = atan2f(XMVectorGetX(forward), XMVectorGetZ(forward));
	_float fPitch = asinf(-XMVectorGetY(forward));
	_float fRoll = atan2f(XMVectorGetY(right), XMVectorGetY(up)); // 수정된 Roll 계산

	_float3 vRPY = { fRoll, fPitch, fYaw }; // Roll-Pitch-Yaw 순서 정렬

	// --- GUI 편집 ---
	GUI::Text("Rotation (Roll Pitch Yaw)");
	GUI::SameLine();
	if (GUI::DragFloat3("##LocalRotq", (_float*)&vRPY,
		XMConvertToRadians(0.5f), -XM_2PI, XM_2PI, "%.5f"))
	{
		// XMQuaternionRotationRollPitchYaw는 (Yaw→Pitch→Roll) 순서
		_vector vRotq = XMQuaternionRotationRollPitchYaw(vRPY.z, vRPY.y, vRPY.x);
		XMStoreFloat4(&m_vLocalRotQ, vRotq);
		Move_LocalPos(m_vLocalRotQ, m_vLocalTranslation);
	}
	GUI::EndChild();
}
#endif // _DEBUG
