#include "pch.h"
#include "PhysX_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "RigidBody_Dynamic.h"
#include "RigidBody_Static.h"
#include "Mesh.h"

CPhysX_Manager::CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

PSX::PxRigidDynamic* CPhysX_Manager::Add_DynamicActor(CRigidBody_Dynamic& RigidBody, _uint iLevel)
{
	_matrix WorldMatrix = RigidBody.Get_TransformPtr()->Get_XMWorldMatrix();
	PSX::PxTransform pxWorldMatrix = XMWorldToPx_NoScaleNoFlip(WorldMatrix);

	// PxRigidDynamic		씬의 동적 바디 인터페이스
	PSX::PxRigidDynamic* pActorDynamic = m_pPhysics->createRigidDynamic(pxWorldMatrix);
	PSX::PxShape* pShape = nullptr;
	PSX::PxGeometryHolder geoHolder = {};

	_float3 vVolume = RigidBody.Get_HalfGeometryInfo();

	switch (RigidBody.Get_Type()) {
	case ACTOR::BOX:
		geoHolder = PSX::PxBoxGeometry(PSX::PxVec3(vVolume.x, vVolume.y, vVolume.z));
		pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActorDynamic, geoHolder.box(), *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);
		break;
	case ACTOR::CAPSULE:
		geoHolder = PSX::PxCapsuleGeometry(vVolume.x, vVolume.y);
		pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActorDynamic, geoHolder.capsule(), *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);
		FixCapsuleShapeAxisToY(pShape);
		break;
	case ACTOR::SPHERE:
		geoHolder = PSX::PxSphereGeometry(vVolume.x);
		pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActorDynamic, geoHolder.sphere(), *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);
		break;
	default:
		assert(false); 
		break;
	}
	assert(nullptr != pShape);

	// createExclusiveShape를 하면 쉐이프가 actor에 attach 된 상태로 나오기 때문에
	// 쉐이프 속성만 건드려서 액터의 쉐이프를 수정함
	pShape->setFlags(RigidBody.Get_ShapeFlags());
	pShape->setContactOffset(RigidBody.Get_ContactOffset());
	pShape->setRestOffset(0.f);

	PSX::PxRigidBodyFlags pxRigidFlags = RigidBody.Get_RigidBodyFlags();
	pActorDynamic->setRigidBodyFlags(pxRigidFlags);


	PSX::PxRigidBodyExt::updateMassAndInertia(*pActorDynamic, (PSX::PxReal)RigidBody.Get_Density());
	m_pRestBodies[iLevel].insert(pActorDynamic);
	pActorDynamic->userData = RigidBody.Get_UserDataPtr();
	ApplyFilterData(pActorDynamic);
	Attach_Actor(*pActorDynamic, iLevel);

	return pActorDynamic;
}

PSX::PxRigidStatic* CPhysX_Manager::Add_StaticActor(CRigidBody_Static& RigidBody, _uint iLevel)
{
	_matrix WorldMatrix = RigidBody.Get_TransformPtr()->Get_XMWorldMatrix();
	 PSX::PxTransform pxWorldMatrix = XMWorldToPx_NoScaleNoFlip(WorldMatrix);

	// PxRigidStatic		씬의 정적 바디 인터페이스
	PSX::PxRigidStatic* pActorStatic = m_pPhysics->createRigidStatic(pxWorldMatrix);
	PSX::PxShape* pShape = { nullptr };
	PSX::PxGeometry* pGeometry = { nullptr };

	switch (RigidBody.Get_Type())
	{
	case ACTOR::PLANE:
		break;
	case ACTOR::TRIANGLEMESH:
		{ // SetUp Geometry
			PSX::PxTriangleMesh* pPxMesh = Find_TriangleMesh(RigidBody.Get_PxMeshKey(), iLevel);
			PSX::PxTriangleMeshGeometry* pPxMeshGeometry = { nullptr };

			_vector vPos, vRotq, vScale;
			XMMatrixDecompose(&vScale, &vRotq, &vPos, WorldMatrix);
			vRotq = XMQuaternionNormalize(vRotq);

			PSX::PxTransform out;
			XMStoreFloat3((_float3*)&out.p, vPos);
			XMStoreFloat4((_float4*)&out.q, vRotq);
			PSX::PxMeshScale meshScale(
				PSX::PxVec3(fabsf(vScale.m128_f32[0]), fabsf(vScale.m128_f32[1]), fabsf(vScale.m128_f32[2])),
				PSX::PxQuat(PSX::PxIdentity) // 스케일 축은 로컬 기준
			);
			pPxMeshGeometry = new PSX::PxTriangleMeshGeometry(pPxMesh, meshScale);

			// GeoFlag 중 더블사이드는 이제 지원 안함, 할거면 노말 뒤집어서 하라고 함
			// pPxMeshGeometry->meshFlags |= PSX::PxMeshGeometryFlag::eDOUBLE_SIDED;
			// 유효성 체크
			PX_ASSERT(pPxMeshGeometry->isValid());
			pGeometry = pPxMeshGeometry;
			_bool bEmplaceSuccess = m_TriangleMeshGeometry[iLevel].emplace(RigidBody.Get_PxMeshKey(), pPxMeshGeometry).second;
			if (false == bEmplaceSuccess) {
				Safe_Delete(pPxMeshGeometry);
				pGeometry = (*m_TriangleMeshGeometry[iLevel].find(RigidBody.Get_PxMeshKey())).second;
			}
		}
		break;
	case ACTOR::HEIGHTFIELD:
	{ // SetUp HeightField
		PSX::PxHeightField* pHeightField = Find_HeightField(RigidBody.Get_PxMeshKey(), iLevel);
		PSX::PxHeightFieldGeometry* pPxHeightGeometry = new PSX::PxHeightFieldGeometry(pHeightField);
		pPxHeightGeometry->heightScale = 0.01f/* 기본 정밀도 100배 0.01 -> 1cm, 1 -> 1m */;

		// 유효성 체크
		PX_ASSERT(pPxHeightGeometry->isValid());
		pGeometry = pPxHeightGeometry;
		_bool bEmplaceSuccess = m_HeightFieldGeometry[iLevel].emplace(RigidBody.Get_PxMeshKey(), pPxHeightGeometry).second;
		if (false == bEmplaceSuccess) {
			Safe_Delete(pPxHeightGeometry);
			pGeometry = (*m_HeightFieldGeometry[iLevel].find(RigidBody.Get_PxMeshKey())).second;
		}
	}
	break;
	default:
		assert(false);
		break;
	}

	pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActorStatic, *pGeometry, *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);

	pShape->setFlags(RigidBody.Get_ShapeFlags());
	pShape->setContactOffset(RigidBody.Get_ContactOffset());
	pShape->setRestOffset(0.f);
	
	m_pRestBodies[iLevel].insert(pActorStatic);
	ApplyFilterData(pActorStatic);
	Attach_Actor(*pActorStatic, iLevel);

	return pActorStatic;
}

PSX::PxRigidStatic* CPhysX_Manager::Add_StaticActor(CRigidBody_Static& RigidBody, _uint iLevel, const _float4x4* pWorldMatrix)
{
	_matrix WorldMatrix = XMLoadFloat4x4(pWorldMatrix);

	PSX::PxTransform pxWorldMatrix = XMWorldToPx_NoScaleNoFlip(WorldMatrix);

	// PxRigidStatic		씬의 정적 바디 인터페이스
	PSX::PxRigidStatic* pActorStatic = m_pPhysics->createRigidStatic(pxWorldMatrix);
	PSX::PxShape* pShape = { nullptr };
	PSX::PxGeometry* pGeometry = { nullptr };

	switch (RigidBody.Get_Type())
	{
	case ACTOR::PLANE:
		break;
	case ACTOR::TRIANGLEMESH:
	{ // SetUp Geometry
		PSX::PxTriangleMesh* pPxMesh = Find_TriangleMesh(RigidBody.Get_PxMeshKey(), iLevel);
		PSX::PxTriangleMeshGeometry* pPxMeshGeometry = { nullptr };

		_vector vPos, vRotq, vScale;
		XMMatrixDecompose(&vScale, &vRotq, &vPos, WorldMatrix);
		vRotq = XMQuaternionNormalize(vRotq);

		PSX::PxTransform out;
		XMStoreFloat3((_float3*)&out.p, vPos);
		XMStoreFloat4((_float4*)&out.q, vRotq);
		PSX::PxMeshScale meshScale(
			PSX::PxVec3(fabsf(vScale.m128_f32[0]), fabsf(vScale.m128_f32[1]), fabsf(vScale.m128_f32[2])),
			PSX::PxQuat(PSX::PxIdentity) // 스케일 축은 로컬 기준
		);
		pPxMeshGeometry = new PSX::PxTriangleMeshGeometry(pPxMesh, meshScale);

		// GeoFlag 중 더블사이드는 이제 지원 안함, 할거면 노말 뒤집어서 하라고 함
		// pPxMeshGeometry->meshFlags |= PSX::PxMeshGeometryFlag::eDOUBLE_SIDED;
		// 유효성 체크
		PX_ASSERT(pPxMeshGeometry->isValid());
		pGeometry = pPxMeshGeometry;
		_bool bEmplaceSuccess = m_TriangleMeshGeometry[iLevel].emplace(RigidBody.Get_PxMeshKey(), pPxMeshGeometry).second;
		if (false == bEmplaceSuccess) {
			Safe_Delete(pPxMeshGeometry);
			pGeometry = (*m_TriangleMeshGeometry[iLevel].find(RigidBody.Get_PxMeshKey())).second;
		}
	}
	break;
	case ACTOR::HEIGHTFIELD:
	{ // SetUp HeightField
		PSX::PxHeightField* pHeightField = Find_HeightField(RigidBody.Get_PxMeshKey(), iLevel);
		PSX::PxHeightFieldGeometry* pPxHeightGeometry = new PSX::PxHeightFieldGeometry(pHeightField);
		pPxHeightGeometry->heightScale = 0.01f/* 기본 정밀도 100배 0.01 -> 1cm, 1 -> 1m */;

		// 유효성 체크
		PX_ASSERT(pPxHeightGeometry->isValid());
		pGeometry = pPxHeightGeometry;
		_bool bEmplaceSuccess = m_HeightFieldGeometry[iLevel].emplace(RigidBody.Get_PxMeshKey(), pPxHeightGeometry).second;
		if (false == bEmplaceSuccess) {
			Safe_Delete(pPxHeightGeometry);
			pGeometry = (*m_HeightFieldGeometry[iLevel].find(RigidBody.Get_PxMeshKey())).second;
		}
	}
	break;
	default:
		assert(false);
		break;
	}

	pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActorStatic, *pGeometry, *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);

	pShape->setFlags(RigidBody.Get_ShapeFlags());
	pShape->setContactOffset(RigidBody.Get_ContactOffset());
	pShape->setRestOffset(0.f);

	m_pRestBodies[iLevel].insert(pActorStatic);
	ApplyFilterData(pActorStatic);
	Attach_Actor(*pActorStatic, iLevel);

	return pActorStatic;
}

PSX::PxJoint* CPhysX_Manager::Create_PxJoint(PHYSX_JOINT eType, PSX::PxRigidActor* pActor0, PSX::PxTransform& pxLocalFrame0, PSX::PxRigidActor* pActor1, PSX::PxTransform& pxLocalFrame1)
{
	PSX::PxJoint* pOut = { nullptr };
	switch (eType)
	{
	case PHYSX_JOINT::D6:
	{
		PSX::PxD6Joint* pJoint = PSX::PxD6JointCreate(*m_pPhysics, pActor0, pxLocalFrame0, pActor1, pxLocalFrame1);
		pOut = pJoint;
	}
		break;
	case PHYSX_JOINT::DISTANCE:
	{
		PSX::PxDistanceJoint* pJoint = PSX::PxDistanceJointCreate(*m_pPhysics, pActor0, pxLocalFrame0, pActor1, pxLocalFrame1);
		pOut = pJoint;
	}
		break;
	case PHYSX_JOINT::REVOLUTE:
	{
		PSX::PxRevoluteJoint* pJoint = PSX::PxRevoluteJointCreate(*m_pPhysics, pActor0, pxLocalFrame0, pActor1, pxLocalFrame1);
		pOut = pJoint;
	}
		break;
	case PHYSX_JOINT::FIXED:
	{
		PSX::PxFixedJoint* pJoint = PSX::PxFixedJointCreate(*m_pPhysics, pActor0, pxLocalFrame0, pActor1, pxLocalFrame1);
		pOut = pJoint;
	}
		break;
	default:
		break;
	}
	assert(nullptr != pOut);

	return pOut;
}

PSX::PxD6Joint* CPhysX_Manager::Create_BasicPxD6Joint(PSX::PxRigidDynamic* pActor0, PSX::PxRigidDynamic* pActor1, const PSX::PxTransform& pxJointWorldPos)
{
	const PSX::PxTransform pxWorldParentPos = pActor0->getGlobalPose();
	const PSX::PxTransform pxWorldChildPos = pActor1->getGlobalPose();

	const PSX::PxTransform pxLocalParentPOs = pxWorldParentPos.transformInv(pxJointWorldPos);
	const PSX::PxTransform pxLocalChildPos = pxWorldChildPos.transformInv(pxJointWorldPos);

	PSX::PxD6Joint* pJoint = PSX::PxD6JointCreate(*m_pPhysics, pActor0, pxLocalParentPOs, pActor1, pxLocalChildPos);

#ifdef 기무리
	pActor0->setSolverIterationCounts(8, 2);
	pActor1->setSolverIterationCounts(8, 2);
#elif Bin
	pActor0->setSolverIterationCounts(8, 2);
	pActor1->setSolverIterationCounts(8, 2);
#else
	pActor0->setSolverIterationCounts(4, 1);
	pActor1->setSolverIterationCounts(4, 1);
#endif // 기무리

	pJoint->setConstraintFlag(PSX::PxConstraintFlag::eVISUALIZATION, true);
	pJoint->setConstraintFlag(PSX::PxConstraintFlag::eIMPROVED_SLERP, true);
	pJoint->setConstraintFlag(PSX::PxConstraintFlag::eCOLLISION_ENABLED, false);

	pJoint->setBreakForce(1e30f, 1e30f);
	return pJoint;
}

PSX::PxFixedJoint* CPhysX_Manager::Create_BasicPxFixedJoint(PSX::PxRigidDynamic* pActor0, PSX::PxRigidDynamic* pActor1, const PSX::PxTransform& pxJointWorldPos)
{
	const PSX::PxTransform pxWorldParentPos = pActor0->getGlobalPose();
	const PSX::PxTransform pxWorldChildPos = pActor1->getGlobalPose();

	const PSX::PxTransform pxLocalParentPOs = pxWorldParentPos.transformInv(pxJointWorldPos);
	const PSX::PxTransform pxLocalChildPos = pxWorldChildPos.transformInv(pxJointWorldPos);

	PSX::PxFixedJoint* pJoint = PSX::PxFixedJointCreate(*m_pPhysics, pActor0, pxLocalParentPOs, pActor1, pxLocalChildPos);

	pJoint->setConstraintFlag(PSX::PxConstraintFlag::eVISUALIZATION, true);
	pJoint->setConstraintFlag(PSX::PxConstraintFlag::eCOLLISION_ENABLED, false);

	pJoint->setBreakForce(1e30f, 1e30f);
	return pJoint;
}

PSX::PxMaterial* CPhysX_Manager::Create_Material(const _float3* vMatInfo)
{
	PSX::PxMaterial* pPxMaterial = m_pPhysics->createMaterial(vMatInfo->x, vMatInfo->y, vMatInfo->z);
	return pPxMaterial;
}

_bool CPhysX_Manager::SphereCast(_float fRadius, _float3 vStartPos, _float3 vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer)
{
	return SphereCast(fRadius, XMLoadFloat3(&vStartPos), XMLoadFloat3(&vDir), fDistance, flagHitsData, flagQuery, hitBuffer);
}

_bool CPhysX_Manager::SphereCast(_float fRadius, _fvector _vStartPos, _gvector _vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer)
{
	if (fDistance < FLT_EPSILON3) {
		return false;
	}
	_float3 vDir = {};
	_float3 vStartPos = {};

	XMStoreFloat3(&vDir, _vDir);
	XMStoreFloat3(&vStartPos, _vStartPos);

	PSX::PxSphereGeometry sphereGeom(fRadius);

	PSX::PxVec3 startPos(vStartPos.x, vStartPos.y, vStartPos.z);
	PSX::PxQuat startRot(PSX::PxIdentity);

	PSX::PxTransform startPose(startPos, startRot);

	PSX::PxVec3 sweepDir(vDir.x, vDir.y, vDir.z);

	sweepDir.normalize();

	PSX::PxQueryFilterData filterData(flagQuery);

	_bool bHit = m_pScene->sweep(sphereGeom, startPose, sweepDir, fDistance, hitBuffer, flagHitsData, filterData);

	return bHit;
}

_bool CPhysX_Manager::Overlap(_float fRadius, _fvector vCenter, PSX::PxQueryFlags queryFlags, PSX::PxOverlapCallback& overlapBuffer, PSX::PxQueryFilterCallback* filterCallback)
{
	const PSX::PxSphereGeometry sphereGeometry(fRadius);
	PSX::PxVec3 pxCenter = {};
	XMStoreFloat3((_float3*)&pxCenter, vCenter);
	const PSX::PxTransform spherePose(pxCenter);

	PSX::PxQueryFilterData filterData;
	filterData.flags = queryFlags;

	if (filterCallback != nullptr)
		filterData.flags |= PSX::PxQueryFlag::ePREFILTER;

	_bool hit = m_pScene->overlap( sphereGeometry, spherePose, overlapBuffer, filterData, filterCallback );

	return hit ? true : false;
}

_bool CPhysX_Manager::RayCast(_float3 _vStartPos, _float3 _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount)
{
	return RayCast(XMLoadFloat3(&_vStartPos), XMLoadFloat3(&_vDir), fDistance, pRayHitArray, iMaxHitCapacity, iOutHitCount);
}

bool CPhysX_Manager::RayCast(_fvector _vStartPos, _gvector _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount )
{
	iOutHitCount = 0;

	if (pRayHitArray == nullptr || iMaxHitCapacity == 0) {
		return false;
	}

	_float3 vDir = {};
	_float3 vStartPos = {};

	XMStoreFloat3(&vDir, _vDir);
	XMStoreFloat3(&vStartPos, _vStartPos);

	PSX::PxVec3 startPos(vStartPos.x, vStartPos.y, vStartPos.z);
	PSX::PxVec3 sweepDir(vDir.x, vDir.y, vDir.z);

	sweepDir.normalize();

	PSX::PxRaycastBuffer raycastBuffer(pRayHitArray, iMaxHitCapacity);

	const PSX::PxHitFlags hitFlags = PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL;

	PSX::PxQueryFilterData queryFilterData;
	queryFilterData.flags = PSX::PxQueryFlag::eSTATIC | PSX::PxQueryFlag::eDYNAMIC;

	const _bool bHit = m_pScene->raycast(startPos, sweepDir, fDistance, raycastBuffer, hitFlags, queryFilterData );

	if (bHit == false) {
		return false;
	}

	iOutHitCount = raycastBuffer.getNbAnyHits(); // block + touches 합
	return bHit;
}


void CPhysX_Manager::RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh, _uint iLevel)
{
	m_TriangleMeshes[iLevel].emplace(CMyTools::ToWstring(pName), pPxTriMesh);
}
void CPhysX_Manager::RegistHeight(const _tchar* pName, PSX::PxHeightFieldDesc& Desc, _uint iLevel)
{
	PSX::PxHeightField* pHeightField = PxCreateHeightField(Desc);
	if (nullptr == pHeightField) {
		assert(false);
	}
	else {
		m_HeightFields[iLevel].emplace(pName, pHeightField);
	}

}
HRESULT CPhysX_Manager::ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix)
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		PSX::PxTriangleMesh* pTriangleMesh = Meshes[i]->ConvertToPxMesh(m_pCookingParam, m_pPhysics, WorldMatrix);
		PX_ASSERT(pTriangleMesh);
		PX_ASSERT(pTriangleMesh->getReferenceCount() == 1);
		if (nullptr == pTriangleMesh) {
			return E_FAIL;
		}
		pxTriMeshes.push_back(pTriangleMesh);
	}
	pxTriMeshes.shrink_to_fit();

	return S_OK;
}

#ifdef EDITOR_PROJECT

HRESULT CPhysX_Manager::SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	filesystem::path pathPhysX = pPath;
	pathPhysX.replace_extension(".Pxml");
	if (TriMeshes.empty()){
		return E_FAIL;
	}

	filesystem::create_directories(pathPhysX.parent_path());

	PSX::PxSerializationRegistry* pSerializationRegistry = PSX::PxSerialization::createSerializationRegistry(*m_pPhysics);
	PSX::PxCollection* pCollections = PxCreateCollection();
	if (nullptr == pCollections) { 
		pSerializationRegistry->release();
		assert(pCollections);
		return E_FAIL;
	}

	for (PSX::PxTriangleMesh* pTriMesh : TriMeshes) {
		pCollections->add(*pTriMesh);
	}

	PSX::PxSerialization::complete(*pCollections, *pSerializationRegistry);
	PSX::PxDefaultFileOutputStream out(pathPhysX.string().c_str());
	PSX::PxCollection* extRefs = PxCreateCollection();
	const _bool ok = PSX::PxSerialization::serializeCollectionToXml(out, *pCollections, *pSerializationRegistry, m_pCookingParam, extRefs);

	extRefs->release();
	pCollections->release();
	pSerializationRegistry->release();
	assert(ok);
	return (ok ? S_OK : E_FAIL);
}
void CPhysX_Manager::Add_Editor_Plane(PHYSX_USERDATA& PlaneData)
{
	PlaneData.eKind = PHYSX_KIND::BODY_STATIC;
	PlaneData.iSubKind = UINT_MAX;
	PlaneData.pOwner = nullptr;
	PlaneData.pBody = nullptr;

	PSX::PxRigidStatic* pGroundPlane = PxCreatePlane(*m_pPhysics, physx::PxPlane(0, 1, 0, 0), *m_pMaterials[ENUM_CLASS(PXMATERIAL::DEFAULT)]);
	pGroundPlane->userData = &PlaneData;
	pGroundPlane->setName("PHYSX_EDITPLANE");
	m_pScene->addActor(*pGroundPlane);
}

#endif // EDITOR_PROJECT

HRESULT CPhysX_Manager::LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	filesystem::path pathPhysX = pPath;
	pathPhysX.replace_extension(".Pxml");

	PSX::PxSerializationRegistry* pSerializationRegistry = PSX::PxSerialization::createSerializationRegistry(*m_pPhysics);

	PSX::PxDefaultFileInputData InputData(pathPhysX.string().c_str());
	PSX::PxCollection* extRefs = PxCreateCollection();
	
	PSX::PxCollection* pCollections = PSX::PxSerialization::createCollectionFromXml(InputData, *m_pCookingParam, *pSerializationRegistry, extRefs);
	if (nullptr == pCollections) { 
		extRefs->release();
		pSerializationRegistry->release(); 
		assert(pCollections);
		return E_FAIL;
	}

	const PSX::PxU32 iNumTriMeshes = pCollections->getNbObjects();
	TriMeshes.reserve(iNumTriMeshes);
	for (PSX::PxU32 i = 0; i < iNumTriMeshes; ++i) {
		PSX::PxBase& pObject = pCollections->getObject(i);

		if (pObject.getConcreteType() == PSX::PxConcreteType::eTRIANGLE_MESH_BVH34) {
			PSX::PxTriangleMesh* pTriMesh = static_cast<PSX::PxTriangleMesh*>(&pObject);
			pTriMesh->acquireReference();
			TriMeshes.push_back(pTriMesh);
		}
	}

	extRefs->release();
	pCollections->release();
	pSerializationRegistry->release();
	return S_OK;
}

PSX::PxTriangleMesh* CPhysX_Manager::Find_TriangleMesh(const _tchar* pMeshName, _uint iLevel)
{
	map<_wstring, PSX::PxTriangleMesh*>::iterator iter = m_TriangleMeshes[iLevel].find(pMeshName);
	if (iter != m_TriangleMeshes[iLevel].end()) {
		return iter->second;
	}
	return nullptr;
}

PSX::PxHeightField* CPhysX_Manager::Find_HeightField(const _tchar* pFieldName, _uint iLevel)
{
	map<_wstring, PSX::PxHeightField*>::iterator iter = m_HeightFields[iLevel].find(pFieldName);
	if (iter != m_HeightFields[iLevel].end()) {
		return iter->second;
	}
	return nullptr;
}

void CPhysX_Manager::Update_Kinematic()
{
	list<PSX::PxActor*> ReleasedList = {};
	for (_uint iLevel = 0; iLevel < m_iMaxLevel; ++iLevel) {
		for (auto& pBody : m_pActiveBodys[iLevel]) {
			PSX::PxRigidDynamic* pActor = pBody->is<PSX::PxRigidDynamic>();
			if (nullptr == pActor) {
				continue;
			}

			if (pActor->getRigidBodyFlags() & PSX::PxRigidBodyFlag::eKINEMATIC)
			{
				if (nullptr == pBody->userData) {
					ReleasedList.emplace_back(pBody);
					continue;
				}
				PHYSX_USERDATA* pUserData = (PHYSX_USERDATA*)pBody->userData;
				if (false == pUserData->bAutoOwnerTranslation) {
					continue;
				}
				const CTransform* pTransform = pUserData->pOwner->Get_Component<CTransform>();

				pActor->setKinematicTarget(XMWorldToPx_NoScaleNoFlip(pTransform->Get_XMWorldMatrix()));
			}
		}
	}
	for (list<PSX::PxActor*>::iterator ReleasedActor = ReleasedList.begin(); ReleasedActor != ReleasedList.end();) {
		Detach_Actor(*(*ReleasedActor), m_pGameInstance->Get_CurrentLevelID());
		ReleasedActor = ReleasedList.erase(ReleasedActor);
	}
}

void CPhysX_Manager::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_PhysX"));
#endif // _DEBUG
	if (nullptr == m_pScene) {
		return;
	}
	{ // Prev
		Update_Kinematic();
	}
	{ // Simulate
		m_pScene->simulate((PSX::PxReal)fTimeDelta);
		_bool bResult = m_pScene->fetchResults(true);
	}
	{ // Post
		 Update_Dynamic_ActiveActors();
		//Update_Dynamic_AllActors();
	}
#ifdef _DEBUG
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_PhysX"));
#endif // _DEBUG
}
void CPhysX_Manager::Update_Dynamic_ActiveActors()
{
	list<PSX::PxActor*> ReleasedList = {};
	PSX::PxU32 iNumActiveActor = {};
	PSX::PxActor** ppActiveActors = m_pScene->getActiveActors(iNumActiveActor);

	for (PSX::PxU32 i = 0; i < iNumActiveActor; ++i) {
		PHYSX_USERDATA* pUserData = (PHYSX_USERDATA*)ppActiveActors[i]->userData;
		PSX::PxRigidDynamic* pActorDynamic = ppActiveActors[i]->is<PSX::PxRigidDynamic>();
		if (nullptr != pActorDynamic) {
			if (nullptr == ppActiveActors[i]->userData) {
				ReleasedList.emplace_back(ppActiveActors[i]);
				continue;
			}
			if (false == pUserData->bAutoOwnerTranslation) {
				continue;
			}
			CTransform* pTransform = pUserData->pOwner->Get_Component<CTransform>();

			PSX::PxTransform pPxTransform = pActorDynamic->getGlobalPose();
			_float3 vOriginalScale = pTransform->Get_Scale();
			_matrix WorldMatrix = {};

			WorldMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vOriginalScale), XMVectorZero(), XMLoadFloat4((_float4*)&pPxTransform.q), XMLoadFloat3((_float3*)&pPxTransform.p));
			pTransform->Set_WorldMatrix(WorldMatrix);
		}
	}
	for (list<PSX::PxActor*>::iterator ReleasedActor = ReleasedList.begin(); ReleasedActor != ReleasedList.end();) {
		Detach_Actor(*(*ReleasedActor), m_pGameInstance->Get_CurrentLevelID());
		ReleasedActor = ReleasedList.erase(ReleasedActor);
	}
}

void CPhysX_Manager::FixCapsuleShapeAxisToY(PSX::PxShape* pShape)
{
	PSX::PxTransform shapeLocalPose = pShape->getLocalPose();

	// X -> Y : Z축 기준 +90도 회전
	shapeLocalPose.q = PSX::PxQuat(PSX::PxHalfPi, PSX::PxVec3(0.f, 0.f, 1.f)) * shapeLocalPose.q;

	pShape->setLocalPose(shapeLocalPose);
}

uint32_t CPhysX_Manager::ConvertPhysxKindToBit(const PHYSX_USERDATA& physxUserData)
{
	if (physxUserData.eKind == PHYSX_KIND::BODY_STATIC)
		return COLLISIONCATERGORY_BIT32::WorldStatic;

	if (physxUserData.eKind == PHYSX_KIND::CCTActor)
		return COLLISIONCATERGORY_BIT32::CharacterController;

	if (physxUserData.eKind == PHYSX_KIND::BODY_DYNAMIC)
	{
		_bool isJointRelatedPart =
			physxUserData.iSubKind == ENUM_CLASS(PXOBJECT::JOINT_ANCHOR) ||
			physxUserData.iSubKind == ENUM_CLASS(PXOBJECT::JOINT_ROUTE);

		if (true == isJointRelatedPart)
			return COLLISIONCATERGORY_BIT32::JointedParts;

		return COLLISIONCATERGORY_BIT32::WorldDynamic;
	}

	return 0u;
}

uint32_t CPhysX_Manager::BuildCollisionMaskFromCategoryBit(uint32_t iMyBit)
{
	if (iMyBit == COLLISIONCATERGORY_BIT32::CharacterController)
	{
		// CCT는 조인트 파츠 충돌x
		return COLLISIONCATERGORY_BIT32::WorldStatic | COLLISIONCATERGORY_BIT32::WorldDynamic;
	}

	if (iMyBit == COLLISIONCATERGORY_BIT32::JointedParts)
	{
		// 조인트 파츠도 CCT 충돌x
		return COLLISIONCATERGORY_BIT32::WorldStatic | COLLISIONCATERGORY_BIT32::WorldDynamic;
	}

	// 기본값
	return COLLISIONCATERGORY_BIT32::WorldStatic | COLLISIONCATERGORY_BIT32::WorldDynamic | COLLISIONCATERGORY_BIT32::CharacterController | COLLISIONCATERGORY_BIT32::JointedParts;
}

void CPhysX_Manager::ApplyFilterData(PSX::PxRigidActor* pRigidActor)
{
	if (pRigidActor == nullptr){
		return;
	}

	PHYSX_USERDATA* physxUserData = reinterpret_cast<PHYSX_USERDATA*>(pRigidActor->userData);
	if (physxUserData == nullptr){
		return;
	}

	const uint32_t CategoryBit = ConvertPhysxKindToBit(*physxUserData); // who am i
	const uint32_t MaskBits = BuildCollisionMaskFromCategoryBit(CategoryBit); // for what

	PSX::PxFilterData DATAfilter = {};
	DATAfilter.word0 = CategoryBit; // 나는 누구인가
	DATAfilter.word1 = MaskBits;    // 누구와 상호작용(충돌/쿼리)할 것인가

	const PSX::PxU32 shapeCount = pRigidActor->getNbShapes();
	vector<PSX::PxShape*> shapes(shapeCount);
	pRigidActor->getShapes(shapes.data(), shapeCount);

	for (PSX::PxShape* shape : shapes)
	{
		if (!shape) continue;

		// 시뮬레이션 필터
		shape->setSimulationFilterData(DATAfilter);

		// 씬쿼리(Overlap/Sweep/Raycast) 필터
		shape->setQueryFilterData(DATAfilter);

		// 씬쿼리 대상 플래그 보장
		shape->setFlag(PSX::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	}

}

PSX::PxFilterFlags CPhysX_Manager::SimulationFilterShader(PSX::PxFilterObjectAttributes pxAttributes0, PSX::PxFilterData pxFilterData0, PSX::PxFilterObjectAttributes pxAttributes1, PSX::PxFilterData pxFilterData1, PSX::PxPairFlags& pxPairFlags, const void* pConstantBlock, PSX::PxU32 iConstantBlockSize)
{
	_bool bFilter0_1 = (pxFilterData0.word0 & pxFilterData1.word1) != 0u;
	_bool bFilter1_0 = (pxFilterData1.word0 & pxFilterData0.word1) != 0u;

	if (!bFilter0_1 || !bFilter1_0)
	{
		// Event. 접촉제거
		return PSX::PxFilterFlag::eSUPPRESS;
	}

	if (PSX::PxFilterObjectIsTrigger(pxAttributes0) || PSX::PxFilterObjectIsTrigger(pxAttributes1))
	{
		// 트리거 처리
		pxPairFlags = PSX::PxPairFlag::eTRIGGER_DEFAULT;
		return PSX::PxFilterFlag::eDEFAULT;
	}

	pxPairFlags = PSX::PxPairFlag::eCONTACT_DEFAULT;
	return PSX::PxFilterFlag::eDEFAULT;
}

void CPhysX_Manager::ClearScene(_uint iLevel)
{
	for (auto& pBody : m_pActiveBodys[iLevel]) {
		pBody->release();
	} m_pActiveBodys[iLevel].clear();
	for (auto& pBody : m_pRestBodies[iLevel]) {
		pBody->release();
	} m_pRestBodies[iLevel].clear();
	for (auto& pMeshes : m_TriangleMeshes[iLevel]) {
		pMeshes.second->release();
	} m_TriangleMeshes[iLevel].clear();

	for (auto& pGeometry : m_TriangleMeshGeometry[iLevel]) {
		Safe_Delete(pGeometry.second);
	} m_TriangleMeshGeometry[iLevel].clear();

	for (auto& pGeometry : m_HeightFieldGeometry[iLevel]) {
		Safe_Delete(pGeometry.second);
	} m_HeightFieldGeometry[iLevel].clear();

	for (auto& pField : m_HeightFields[iLevel]) {
		pField.second->release();
	} m_HeightFields[iLevel].clear();

}
void CPhysX_Manager::Attach_Actor(PSX::PxActor& Actor, _uint iLevel)
{
	unordered_set<PSX::PxActor*>::iterator iter = m_pRestBodies[iLevel].find(&Actor);
	if (m_pRestBodies[iLevel].end() == iter) {
		return;
	}
	else {
		m_pRestBodies[iLevel].erase(&Actor);
	}
	m_pActiveBodys[iLevel].insert(&Actor);
	if (m_pScene != nullptr) {
		m_pScene->addActor(Actor);
	}
}

unordered_set<PSX::PxActor*>::iterator CPhysX_Manager::Detach_Actor(PSX::PxActor& Actor, _uint iLevel)
{
	unordered_set<PSX::PxActor*>::iterator iterOut = m_pRestBodies[iLevel].end();
	unordered_set<PSX::PxActor*>::iterator iter = m_pActiveBodys[iLevel].find(&Actor);
	if (m_pActiveBodys[iLevel].end() != iter) {
		m_pActiveBodys[iLevel].erase(iter);						// 액터를 활성화 맵에서 분리 해주고
		iterOut = m_pRestBodies[iLevel].insert(&Actor).first;	// 액터를 다시 쓸 수 있기 때문에 따로 보관해줌
	}

	PSX::PxScene* pOwnerScene = Actor.getScene();     

	if (pOwnerScene == nullptr) { // 액터가 씬에 없는 경우 nullptr이 나옴
		return iterOut;
	}

	if (pOwnerScene != m_pScene) { // 액터가 속한 씬이 아닌경우 빼면 안됨
		return iterOut;
	}
	m_pScene->removeActor(Actor);

	return iterOut;
}

void CPhysX_Manager::Release_Actor(PSX::PxActor& Actor)
{
	// 혹시 활성화 맵에 들어있을 수도 있으니 검사
	unordered_set<PSX::PxActor*>::iterator iterResult = {};
	_uint iReleasedLevel = UINT_MAX;
	for (_uint iLevel = 0; iLevel < m_iMaxLevel; ++iLevel) {
		iterResult = Detach_Actor(Actor, iLevel);
		if (m_pRestBodies[iLevel].end() != iterResult) { // 방금 전까지만 해도 활성화 중이었던 액터 라면
			m_pRestBodies[iLevel].erase(iterResult); // 바로 지워줌
			iReleasedLevel = iLevel;
			break;
		}
	}
	if (UINT_MAX == iReleasedLevel) {
		m_pActiveBodys[iReleasedLevel].erase(&Actor);
		m_pRestBodies[iReleasedLevel].erase(&Actor);
	}
	//Actor.release();
}

PSX::PxController* CPhysX_Manager::Add_CapsuleController(PSX::PxCapsuleControllerDesc& Desc)
{
	PSX::PxController* pController = { nullptr };

	pController = m_pCCTManager->createController(Desc);

	return pController;
}

PSX::PxController* CPhysX_Manager::Add_BoxController(PSX::PxBoxControllerDesc& Desc)
{
	PSX::PxController* pController = { nullptr };

	pController = m_pCCTManager->createController(Desc);

	return pController;
}

PSX::PxController* CPhysX_Manager::Get_Controller(_uint iControllerIndex)
{
	return m_pCCTManager->getController((PSX::PxU32)iControllerIndex);
}

void CPhysX_Manager::ReleaseController(_uint iControllerIndex)
{
	if (iControllerIndex >= Get_NumCurrentController()) {
		assert(false);
		return;
	}

	m_pCCTManager->getController((PSX::PxU32)iControllerIndex)->release();
}

_uint CPhysX_Manager::Get_NumCurrentController()
{
	return (_uint)m_pCCTManager->getNbControllers();
}

HRESULT CPhysX_Manager::PurgeAllController()
{
	m_pCCTManager->purgeControllers();
	return S_OK;
}


HRESULT CPhysX_Manager::Initialize(_uint iLevel)
{
	m_iMaxLevel = iLevel;
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_AllocatorCallBack, m_ErrorCallBack);
	if (nullptr == m_pFoundation) {
		assert(false);
		return E_FAIL;
	}
#ifdef _DEBUG
	if (FAILED(Connect_DebugServer())) {
		//ASSERT_NURI(false);
		//ASSERT_JINWOO(false);
	}
#endif // _DEBUG

	{ // 씬 세팅
		m_ToleranceScale.length = 1.f; // 1 meter
		m_ToleranceScale.speed = GRAVITY; // 
		
#ifdef _DEBUG
		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, m_ToleranceScale, true, m_pPvd);
#endif // _DEBUG
#ifndef _DEBUG
		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, m_ToleranceScale, true);
#endif // _DEBUG

		PxInitExtensions(*m_pPhysics, m_pPvd);

		m_pCookingParam = new PSX::PxCookingParams(m_pPhysics->getTolerancesScale());
		m_pCookingParam->meshWeldTolerance = FLT_EPSILON3;
		m_pCookingParam->meshPreprocessParams |= PSX::PxMeshPreprocessingFlag::eWELD_VERTICES;
		PSX::PxSceneDesc sceneDesc = { m_pPhysics->getTolerancesScale() };

		sceneDesc.gravity = PSX::PxVec3(0.f, -GRAVITY, 0.f);
#ifndef _DEBUG
		m_pDispatcher = PSX::PxDefaultCpuDispatcherCreate(4 /*cpu 코어 스레드 갯수에 따라 유동적으로 선택*/);
#endif // _DEBUG
#ifdef _DEBUG
		m_pDispatcher = PSX::PxDefaultCpuDispatcherCreate(2 /*cpu 코어 스레드 갯수에 따라 유동적으로 선택*/);
#endif // _DEBUG
		
		if (nullptr == m_pDispatcher) {
			assert(false);
			return E_FAIL;
		}
		sceneDesc.solverType = PSX::PxSolverType::Enum::eTGS;
		sceneDesc.cpuDispatcher = m_pDispatcher;
		//sceneDesc.simulationEventCallback = ; // fetchResults 때 불림. 아래 필터랑, pair랑 같이 짝꿍임. 좀 더 공부해야 쓸 수 있을듯;;
		sceneDesc.filterShader = &SimulationFilterShader;// PSX::PxDefaultSimulationFilterShader; // 레이어 충돌, 오브젝트에 속성을 주고 복잡한 필터링 충돌 식
		sceneDesc.flags =
			PSX::PxSceneFlag::eENABLE_PCM // 기본 true, 현대적인 충돌시스템 적용
			| PSX::PxSceneFlag::eENABLE_ACTIVE_ACTORS // 현재 움직임이 있는 액터만 가져옴
			| PSX::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS // 그중에 키네틱은 뺌
			| PSX::PxSceneFlag::eENABLE_CCD // CCD 기본플래그, 키면 비쌈. 단일오브젝트만 켤 수 있으니 선택적으로 고려
			| PSX::PxSceneFlag::eDISABLE_CCD_RESWEEP // CCD 옵션플래그, 정확도 낮추고 비용 아낌
			//| PSX::PxSceneFlag::eDISABLE_CONTACT_REPORT_BUFFER_RESIZE // 접촉 리포트버퍼 리사이즈 금지 -> 보통 끔
			//| PSX::PxSceneFlag::eDISABLE_CONTACT_CACHE // 접촉 캐시를 금지 -> 그냥 켜두자, 캐시 있는게 보통 더 낫다
			//| PSX::PxSceneFlag::eREQUIRE_RW_LOCK // 멀티스레딩 환경에서 혹시라도 락 걸고 접근하지 않아 발생할 일을 제거, 
			| PSX::PxSceneFlag::eENABLE_STABILIZATION // 물체가 얽힌 상황에서 불안정한걸 방지. 추가비용인듯
			//| PSX::PxSceneFlag::eENABLE_AVERAGE_POINT // 이해 못하겠음
			//| PSX::PxSceneFlag::eEnable_GPU_Dynamics // 피직스 그래픽가속 설정. 여기선 세팅 안돼서 비활성화ㅏ
			//| PSX::PxSceneFlag::eEnable_Direct_GPU_API // 직접 그래픽버퍼를 다루게끔 설정
			//| PSX::PxSceneFlag::eEnable_Enhanced_Determinism // 중간에 액터 추가해도 결과가 바뀌지 않는 결정론 수준 향상, 리플레이기능 ㅇㅇ
			//| PSX::PxSceneFlag::eENABLE_FRICTION_EVERY_ITERATION // 마찰 꼐산 추가
			//| PSX::PxSceneFlag::eENABLE_EXTERNAL_FORCES_EVERY_ITERATION_TGS // 중력 나눠서 여러번 적용
			//| PSX::PxSceneFlag::eENABLE_PCM // 기본 true
			//| PSX::PxSceneFlag::eENABLE_BODY_ACCELERATIONS // 선형 각가속도 값을 쿼리가능하도록 설정
			//| PSX::PxSceneFlag::eEnable_solver_residual_reporting // 고급 디버깅, 연구 용
			//| PSX::PxSceneFlag::eSOLVE_ARTICULATION_CONTACT_LAST 물리 관절 처리 시나리오 순서 변경
			;

		if (false == sceneDesc.isValid()) {
			assert(false);
			return E_FAIL;
		}

		// SCENE
		m_pScene = m_pPhysics->createScene(sceneDesc);
#ifdef _DEBUG
		m_pScene->setVisualizationParameter(PSX::PxVisualizationParameter::Enum::eJOINT_LOCAL_FRAMES, 1.f);
		m_pScene->setVisualizationParameter(PSX::PxVisualizationParameter::Enum::eJOINT_LIMITS, 1.f);
#endif // _DEBUG



		// CCT
		m_pCCTManager = PxCreateControllerManager(*m_pScene);
		m_pCCTManager->setOverlapRecoveryModule(true); // 복구모듈은 하이트필드에는 적용되지 않음, 오직 스태틱만 대상
		m_pCCTManager->setPreciseSweeps(true); // 정밀 스윕
		m_pCCTManager->setPreventVerticalSlidingAgainstCeiling(false); // 천장타고 슬라이딩 허용 비허용
	}

#ifdef _DEBUG
	// 디버그서버의 클라 세팅
	PSX::PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
	if (nullptr != pPvdClient) {
		pPvdClient->setScenePvdFlag(PSX::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pPvdClient->setScenePvdFlag(PSX::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pPvdClient->setScenePvdFlag(PSX::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
#endif DEBUG

	m_pMaterials.reserve(ENUM_CLASS(PXMATERIAL::END));
	m_pMaterials.push_back(m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f));

	PlaneData.eKind = PHYSX_KIND::BODY_STATIC;
	PlaneData.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);
	PlaneData.pOwner = nullptr;
	PlaneData.pBody = nullptr;

	PSX::PxRigidStatic* pGroundPlane = PxCreatePlane(*m_pPhysics, physx::PxPlane(0, 1, 0, 80), *m_pMaterials[ENUM_CLASS(PXMATERIAL::DEFAULT)]);
	pGroundPlane->userData = &PlaneData;
	pGroundPlane->setName("PHYSX_MANAGER_PLANE");
	m_pScene->addActor(*pGroundPlane);

	m_pActiveBodys = new unordered_set<PSX::PxActor*>[m_iMaxLevel]();
	m_pRestBodies = new unordered_set<PSX::PxActor*>[m_iMaxLevel]();
	m_TriangleMeshes = new map<_wstring, PSX::PxTriangleMesh*>[m_iMaxLevel]();
	m_TriangleMeshGeometry = new map<_wstring, PSX::PxTriangleMeshGeometry*>[m_iMaxLevel]();
	m_HeightFields = new map<_wstring, PSX::PxHeightField*>[m_iMaxLevel]();
	m_HeightFieldGeometry = new map<_wstring, PSX::PxHeightFieldGeometry*>[m_iMaxLevel]();

	return S_OK;
}
#ifdef _DEBUG

HRESULT CPhysX_Manager::Connect_DebugServer()
{
	// 네트워크 디버거 전송객체 생성
	m_pTransport = PSX::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (nullptr == m_pTransport) {
		assert(false);
		return E_FAIL;
	}

	m_pPvd = PSX::PxCreatePvd(*m_pFoundation);
	if (nullptr == m_pPvd) {
		m_pTransport->release();
		assert(false);
		return E_FAIL;
	}
	if (false == (m_pPvd->connect(*m_pTransport, PSX::PxPvdInstrumentationFlag::eALL))) {
		return E_FAIL;
	}
	return S_OK;
}

#endif // _DEBUG

CPhysX_Manager* CPhysX_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CPhysX_Manager* pInstance = new CPhysX_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize(iLevel))) {
		SAFE_RELEASE(pInstance);
		assert(false);
	}
	return pInstance;
}

void CPhysX_Manager::Free()
{
	for (_uint iLevel = 0; iLevel < m_iMaxLevel; ++iLevel) {
		ClearScene(iLevel);
		m_pActiveBodys[iLevel].clear();
		m_pRestBodies[iLevel].clear();
		m_TriangleMeshes[iLevel].clear();
		m_TriangleMeshGeometry[iLevel].clear();
		m_HeightFields[iLevel].clear();
		m_HeightFieldGeometry[iLevel].clear();
	}
	Safe_Delete_Array(m_pActiveBodys);
	Safe_Delete_Array(m_pRestBodies);
	Safe_Delete_Array(m_TriangleMeshes);
	Safe_Delete_Array(m_TriangleMeshGeometry);
	Safe_Delete_Array(m_HeightFields);
	Safe_Delete_Array(m_HeightFieldGeometry);
	
	if (nullptr != m_pCookingParam) {
		Safe_Delete(m_pCookingParam);
	}
	if (nullptr != m_pCCTManager) {
		m_pCCTManager->release();
		m_pCCTManager = nullptr;
	}
	for (PSX::PxMaterial* pMat : m_pMaterials) {
		if (pMat) {
			pMat->release();
		}
	} m_pMaterials.clear();
	if (nullptr != m_pScene) {
		m_pScene->release(); m_pScene = nullptr;
	}

	if (nullptr != m_pDispatcher) {
		m_pDispatcher->release(); m_pDispatcher = nullptr;
	}
	PxCloseExtensions();
	if (nullptr != m_pPhysics) {
		m_pPhysics->release(); m_pPhysics = nullptr;
	}
#ifdef _DEBUG

	if (nullptr != m_pPvd) {
		m_pPvd->disconnect();
		m_pPvd->release(); m_pPvd = nullptr;
	}

#endif // _DEBUG

	if (nullptr != m_pTransport) {
		m_pTransport->release(); m_pTransport = nullptr;
	}

	if (nullptr != m_pFoundation) {
		m_pFoundation->release(); m_pFoundation = nullptr;
	}

	__super::Free();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
