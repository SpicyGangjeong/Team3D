#include "pch.h"
#include "PhysX_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "Mesh.h"

static PSX::PxTransform XMWorldToPx(const _matrix& WorldMatrix)
{
	_vector vPos, vRotq, vScale;
	XMMatrixDecompose(&vScale, &vRotq, &vPos, WorldMatrix);

	vRotq = XMQuaternionNormalize(vRotq);

	PSX::PxTransform out;
	XMStoreFloat3((_float3*)&out.p, vPos);
	XMStoreFloat4((_float4*)&out.q, vRotq);

	return out;
}

CPhysX_Manager::CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

const PSX::PxRigidDynamic* CPhysX_Manager::Add_DynamicActor(CRigidBody& RigidBody)
{
	_matrix WorldMatrix = RigidBody.Get_PxTransformPtr()->Get_XMWorldMatrix();

	PSX::PxTransform pxWorldMatrix = XMWorldToPx(WorldMatrix);

	// PxRigidDynamic		씬의 동적 바디 인터페이스
	PSX::PxRigidDynamic* pActorDynamic = m_pPhysics->createRigidDynamic(pxWorldMatrix);
	pActorDynamic->userData = &RigidBody;
	pActorDynamic->attachShape(*RigidBody.Get_ShapePtr());
	if (RigidBody.Is_Kinematic()) {
		pActorDynamic->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, true);
	}
	else {
		PSX::PxRigidBodyExt::updateMassAndInertia(*pActorDynamic, (PSX::PxReal)RigidBody.Get_Density());
	}


	m_RigidBodys.emplace_back(&RigidBody, pActorDynamic);
	m_pScene->addActor(*pActorDynamic);

	return pActorDynamic;
}

const PSX::PxRigidStatic* CPhysX_Manager::Add_StaticActor(CRigidBody& RigidBody)
{
	_matrix WorldMatrix = RigidBody.Get_PxTransformPtr()->Get_XMWorldMatrix();

	PSX::PxTransform pxWorldMatrix = XMWorldToPx(WorldMatrix);
    
	// PxRigidStatic		씬의 정적 바디 인터페이스
	const PSX::PxTriangleMesh* pPxMesh = { nullptr };
	{
		auto iter = m_TriangleMeshes.find(RigidBody.Get_PxMeshKey());
		if (m_TriangleMeshes.end() == iter) {
			assert(false);
			return nullptr;
		}
		pPxMesh = iter->second;
	}
	const PSX::PxTriangleMeshGeometry* pPxMeshGeometry = { nullptr };
	{
		auto iter = m_TriangleMeshGeometry.find(RigidBody.Get_PxMeshKey());
		if (m_TriangleMeshGeometry.end() == iter) {
			assert(false);
			return nullptr;
		}
		pPxMeshGeometry = iter->second;
	}

	PSX::PxRigidStatic* pActor = m_pPhysics->createRigidStatic(pxWorldMatrix);
	PSX::PxShape* pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActor, *pPxMeshGeometry, *RigidBody.Get_PxMaterial());

	pShape->setFlag(PSX::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pShape->setFlag(PSX::PxShapeFlag::eSIMULATION_SHAPE, true);
	
	pActor->userData = &RigidBody;
	m_RigidBodys.emplace_back(&RigidBody, pActor);
	m_pScene->addActor(*pActor);

	return pActor;
}

PSX::PxMaterial* CPhysX_Manager::Get_Material(_float3& vMatInfo)
{
	PSX::PxMaterial* pPxMaterial = m_pPhysics->createMaterial(vMatInfo.x, vMatInfo.y, vMatInfo.z);
	return pPxMaterial;
}

PSX::PxShape* CPhysX_Manager::Create_Shape(ACTOR eType, _float3& vhalfGeometryInfo, PSX::PxMaterial& pxMaterial, _bool bExclusive, PSX::PxShapeFlags ePxShapeFlag)
{
	PSX::PxShape* pShape = { nullptr };
	switch (eType)
	{
	case Engine::ACTOR::BOX:
		pShape = m_pPhysics->createShape(PSX::PxBoxGeometry(vhalfGeometryInfo.x, vhalfGeometryInfo.y, vhalfGeometryInfo.z), pxMaterial,
			bExclusive, ePxShapeFlag);
		break;
	case Engine::ACTOR::CAPSULE:
		pShape = m_pPhysics->createShape(PSX::PxCapsuleGeometry(vhalfGeometryInfo.x, vhalfGeometryInfo.y), pxMaterial,
			bExclusive, ePxShapeFlag);
		break;
	case Engine::ACTOR::SPHERE:
		pShape = m_pPhysics->createShape(PSX::PxSphereGeometry(vhalfGeometryInfo.x), pxMaterial,
			bExclusive, ePxShapeFlag);
		break;
	default:
		break;
	}
	return pShape;
}

void CPhysX_Manager::RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh) {
	m_TriangleMeshes.emplace(CMyTools::ToWstring(pName), pPxTriMesh);

	PSX::PxTriangleMeshGeometry* pGeometry = new PSX::PxTriangleMeshGeometry(pPxTriMesh);
	pGeometry->meshFlags |= PSX::PxMeshGeometryFlag::eDOUBLE_SIDED;
	m_TriangleMeshGeometry.emplace(CMyTools::ToWstring(pName), pGeometry);
}

HRESULT CPhysX_Manager::ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<PSX::PxTriangleMesh*>& pxTriMeshes)
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		PSX::PxTriangleMesh* pTriangleMesh = Meshes[i]->ConvertToPxMesh(m_pCookingParam, m_pPhysics);
		if (nullptr == pTriangleMesh) {
			return E_FAIL;
		}
		pxTriMeshes.push_back(pTriangleMesh);
	}
	pxTriMeshes.shrink_to_fit();

	return S_OK;
}

_bool CPhysX_Manager::SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	filesystem::path pathPhysX = pPath;
	pathPhysX.replace_extension(".xml");
	if (TriMeshes.empty()){
		return false;
	}

	filesystem::create_directories(pathPhysX.parent_path());


	PSX::PxSerializationRegistry* pSerializationRegistry = PSX::PxSerialization::createSerializationRegistry(*m_pPhysics);
	PSX::PxCollection* pCollections = PxCreateCollection();
	if (nullptr == pCollections) { 
		pSerializationRegistry->release();
		assert(pCollections);
		return false;
	}

	for (PSX::PxTriangleMesh* pTriMesh : TriMeshes) {
		pCollections->add(*pTriMesh);
	}

	PSX::PxSerialization::complete(*pCollections, *pSerializationRegistry);
	PSX::PxDefaultFileOutputStream out(pathPhysX.string().c_str());
	PSX::PxCollection* extRefs = PxCreateCollection();
	const _bool ok = PSX::PxSerialization::serializeCollectionToXml(out, *pCollections, *pSerializationRegistry, m_pCookingParam, extRefs);
	//const _bool ok = PSX::PxSerialization::serializeCollectionToBinary(out, *pCollections, *pSerializationRegistry, extRefs);

	extRefs->release();
	pCollections->release();
	pSerializationRegistry->release();
	assert(ok);
	return ok;
}

_bool CPhysX_Manager::LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes)
{
	filesystem::path pathPhysX = pPath;
	pathPhysX.replace_extension(".xml");

	PSX::PxSerializationRegistry* pSerializationRegistry = PSX::PxSerialization::createSerializationRegistry(*m_pPhysics);

	PSX::PxDefaultFileInputData InputData(pathPhysX.string().c_str());
	PSX::PxCollection* extRefs = PxCreateCollection();
	
	//PSX::PxCollection* pCollections = PSX::PxSerialization::createCollectionFromBinary(&InputData, *pSerializationRegistry, extRefs);
	PSX::PxCollection* pCollections = PSX::PxSerialization::createCollectionFromXml(InputData, *m_pCookingParam, *pSerializationRegistry, extRefs);
	if (nullptr == pCollections) { 
		extRefs->release();
		pSerializationRegistry->release(); 
		assert(pCollections);
		return false;
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
	return true;
}

PSX::PxTriangleMesh* CPhysX_Manager::Find_TriangleMesh(const _wstring& wstrMeshKey)
{
	map<_wstring, PSX::PxTriangleMesh*>::iterator iter = m_TriangleMeshes.find(wstrMeshKey);
	if (iter != m_TriangleMeshes.end()) {
		return iter->second;
	}
	return nullptr;
}

void CPhysX_Manager::Update_Kinematic()
{
	for (auto& pairBody : m_RigidBodys) {
		PSX::PxRigidDynamic* pActor = pairBody.second->is<PSX::PxRigidDynamic>();
		if (nullptr == pActor) {
			continue;
		}

		if (pActor->getRigidBodyFlags() & PSX::PxRigidBodyFlag::eKINEMATIC)
		{
			CRigidBody* pBody = pairBody.first;
			const CTransform* pTransform = pBody->Get_PxTransformPtr();

			pActor->setKinematicTarget(XMWorldToPx(pTransform->Get_XMWorldMatrix()));
		}
	}
}

void CPhysX_Manager::Update(_float fTimeDelta)
{
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
		Update_Dynamic();
	}
}

void CPhysX_Manager::Update_Dynamic()
{
	PSX::PxU32 iNumActiveActor = {};
	PSX::PxActor** ppActiveActors = m_pScene->getActiveActors(iNumActiveActor);

	for (PSX::PxU32 i = 0; i < iNumActiveActor; ++i) {
		PSX::PxRigidDynamic* pActorDynamic = ppActiveActors[i]->is<PSX::PxRigidDynamic>();

		if (nullptr != pActorDynamic) {
			if (nullptr == ppActiveActors[i]->userData) {
				continue;
			}
			CTransform* pTransform = ((CRigidBody*)ppActiveActors[i]->userData)->Get_PxTransformPtr();

			PSX::PxTransform pPrevPxTransform;
			PSX::PxTransform pPxTransform = pActorDynamic->getGlobalPose();
			_float3 vOriginalScale = pTransform->Get_Scale();
			_matrix WorldMatrix = {};

			WorldMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vOriginalScale), XMVectorZero(), XMLoadFloat4((_float4*)&pPxTransform.q), XMLoadFloat3((_float3*)&pPxTransform.p));
			pTransform->Set_WorldMatrix(WorldMatrix);

		}
	}
}

void CPhysX_Manager::ClearScene()
{
	for (auto& pObject : m_RigidBodys) {
		pObject.second->release();
	} m_RigidBodys.clear();
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


HRESULT CPhysX_Manager::Initialize()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_AllocatorCallBack, m_ErrorCallBack);
	if (nullptr == m_pFoundation) {
		assert(false);
		return E_FAIL;
	}

	if (FAILED(Connect_DebugServer())) {
		assert(false);
	}


	{ // 씬 세팅
		m_ToleranceScale.length = 1.f; // 1 meter
		m_ToleranceScale.speed = GRAVITY; // 
		

		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, m_ToleranceScale, true, m_pPvd);

		m_pCookingParam = new PSX::PxCookingParams(m_pPhysics->getTolerancesScale());
		PSX::PxSceneDesc sceneDesc = { m_pPhysics->getTolerancesScale() };

		sceneDesc.gravity = PSX::PxVec3(0.f, -GRAVITY, 0.f);

		m_pDispatcher = PSX::PxDefaultCpuDispatcherCreate(2/*cpu 코어 갯수*/);
		if (nullptr == m_pDispatcher) {
			assert(false);
			return E_FAIL;
		}
		sceneDesc.cpuDispatcher = m_pDispatcher;

		sceneDesc.filterShader = PSX::PxDefaultSimulationFilterShader;

		sceneDesc.flags = PSX::PxSceneFlag::eENABLE_PCM | PSX::PxSceneFlag::eENABLE_ACTIVE_ACTORS | PSX::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS;
		if (false == sceneDesc.isValid()) {
			assert(false);
			return E_FAIL;
		}
		m_pScene = m_pPhysics->createScene(sceneDesc);
		m_pCCTManager = PxCreateControllerManager(*m_pScene);
		m_pCCTManager->setOverlapRecoveryModule(false);
	}

	// 디버그서버의 클라 세팅
	PSX::PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
	if (nullptr != pPvdClient) {
		pPvdClient->setScenePvdFlag(PSX::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pPvdClient->setScenePvdFlag(PSX::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pPvdClient->setScenePvdFlag(PSX::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	// m_pScene->overlap();??????

#ifdef 기무리
	physx::PxMaterial* pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	//physx::PxRigidStatic* pGroundPlane = PxCreatePlane(*m_pPhysics, physx::PxPlane(0, 1, 0, 0), *pMaterial);
	//m_pScene->addActor(*pGroundPlane);

	//{
	//	float halfExtent = .5f;
	//	physx::PxShape* shape = m_pPhysics->createShape(physx::PxSphereGeometry(halfExtent), *pMaterial);
	//	physx::PxU32 size = 30;
	//	physx::PxTransform pxTransform(physx::PxVec3(0));

	//	for (physx::PxU32 i = 0; i < size; i++) {
	//		for (physx::PxU32 j = 0; j < size - i; j++) {
	//			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i) + 100, physx::PxReal(i * 2 + 1), 0) * halfExtent);
	//			physx::PxRigidDynamic* body = m_pPhysics->createRigidDynamic(pxTransform.transform(localTm));
	//			body->attachShape(*shape);
	//			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	//			m_pScene->addActor(*body);
	//		}
	//	}
	//}
#endif // 기무리
	pMaterial->release();
	

	return S_OK;
}

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
	m_pPvd->connect(*m_pTransport, PSX::PxPvdInstrumentationFlag::eALL);
	return S_OK;
}

CPhysX_Manager* CPhysX_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysX_Manager* pInstance = new CPhysX_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize())) {
		SAFE_RELEASE(pInstance);
		assert(false);
	}
	return pInstance;
}

void CPhysX_Manager::Free()
{
	for (auto& pObjects : m_RigidBodys) {
		pObjects.second->release();
	} m_RigidBodys.clear();

	for (auto& pMeshes : m_TriangleMeshes) {
		pMeshes.second->release();
	} m_TriangleMeshes.clear();

	for (auto& pGeometry : m_TriangleMeshGeometry) {
		Safe_Delete(pGeometry.second);
	} m_TriangleMeshGeometry.clear();

	if (nullptr != m_pCookingParam) {
		Safe_Delete(m_pCookingParam);
	}
	if (nullptr != m_pCCTManager) {
		m_pCCTManager->release();
		m_pCCTManager = nullptr;
	}

	if (nullptr != m_pScene) {
		m_pScene->release(); m_pScene = nullptr;
	}

	if (nullptr != m_pDispatcher) {
		m_pDispatcher->release(); m_pDispatcher = nullptr;
	}

	if (nullptr != m_pPhysics) {
		m_pPhysics->release(); m_pPhysics = nullptr;
	}

	if (nullptr != m_pPvd) {
		m_pPvd->disconnect();
		m_pPvd->release(); m_pPvd = nullptr;
	}

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
