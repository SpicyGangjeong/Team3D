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

PSX::PxRigidDynamic* CPhysX_Manager::Add_DynamicActor(CRigidBody_Dynamic& RigidBody)
{
	_matrix WorldMatrix = RigidBody.Get_TransformPtr()->Get_XMWorldMatrix();
	PSX::PxTransform pxWorldMatrix = XMWorldToPx_NoScale(WorldMatrix);

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
		break;
	case ACTOR::SPHERE:
		geoHolder = PSX::PxCapsuleGeometry(vVolume.x);
		pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActorDynamic, geoHolder.sphere(), *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);
		break;
	default:
		assert(false); 
		break;
	}
	assert(nullptr != pShape);

	pShape->setFlags(RigidBody.Get_ShapeFlags());
	pShape->setContactOffset(RigidBody.Get_ContactOffset());
	pShape->setRestOffset(0.f);

	pActorDynamic->attachShape(*pShape);
	PSX::PxRigidBodyFlags pxRigidFlags = RigidBody.Get_RigidBodyFlags();
	pActorDynamic->setRigidBodyFlags(pxRigidFlags);

	if (pxRigidFlags.isSet(PSX::PxRigidBodyFlag::eKINEMATIC)) {

	}
	else {
		PSX::PxRigidBodyExt::updateMassAndInertia(*pActorDynamic, (PSX::PxReal)RigidBody.Get_Density());
	}

	m_pRestBodies.insert(pActorDynamic);

	return pActorDynamic;
}

PSX::PxRigidStatic* CPhysX_Manager::Add_StaticActor(CRigidBody_Static& RigidBody)
{
	_matrix WorldMatrix = RigidBody.Get_TransformPtr()->Get_XMWorldMatrix();
	PSX::PxTransform pxWorldMatrix = XMWorldToPx_NoScale(WorldMatrix);

	// PxRigidStatic		씬의 정적 바디 인터페이스
	PSX::PxRigidStatic* pActor = m_pPhysics->createRigidStatic(pxWorldMatrix);
	PSX::PxShape* pShape = { nullptr };
	PSX::PxTriangleMesh* pPxMesh = Find_TriangleMesh(RigidBody.Get_PxMeshKey());
	PSX::PxTriangleMeshGeometry* pPxMeshGeometry = { nullptr };
	
	switch (RigidBody.Get_Type())
	{
	case ACTOR::PLANE:
		break;
	case ACTOR::TRIANGLEMESH:
		{ // SetUp Geometry
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
			PX_ASSERT(pPxMeshGeometry.isValid());
			m_TriangleMeshGeometry.emplace(RigidBody.Get_PxMeshKey(), pPxMeshGeometry);
		}
		break;
	case ACTOR::HEIGHTFIELD:
		break;
	default:
		assert(false);
		break;
	}

	pShape = PSX::PxRigidActorExt::createExclusiveShape(*pActor, *pPxMeshGeometry, *m_pMaterials[ENUM_CLASS(RigidBody.Get_MaterialType())]);

	pShape->setFlags(RigidBody.Get_ShapeFlags());
	pShape->setContactOffset(RigidBody.Get_ContactOffset());
	pShape->setRestOffset(0.f);
	
	m_pRestBodies.insert(pActor);
	Attach_Actor(*pActor);

	return pActor;
}

PSX::PxMaterial* CPhysX_Manager::Create_Material(const _float3* vMatInfo)
{
	PSX::PxMaterial* pPxMaterial = m_pPhysics->createMaterial(vMatInfo->x, vMatInfo->y, vMatInfo->z);
	return pPxMaterial;
}

void CPhysX_Manager::RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh) 
{
	m_TriangleMeshes.emplace(CMyTools::ToWstring(pName), pPxTriMesh);
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

PSX::PxTriangleMesh* CPhysX_Manager::Find_TriangleMesh(const _tchar* pMeshName)
{
	map<_wstring, PSX::PxTriangleMesh*>::iterator iter = m_TriangleMeshes.find(pMeshName);
	if (iter != m_TriangleMeshes.end()) {
		return iter->second;
	}
	return nullptr;
}

void CPhysX_Manager::Update_Kinematic()
{
	for (auto& pBody : m_pActiveBodys) {
		PSX::PxRigidDynamic* pActor = pBody->is<PSX::PxRigidDynamic>();
		if (nullptr == pActor) {
			continue;
		}

		if (pActor->getRigidBodyFlags() & PSX::PxRigidBodyFlag::eKINEMATIC)
		{
			const CTransform* pTransform = static_cast<PhsXUserData*>(pBody->userData)->pOwner->Get_Component<CTransform>();
			
			pActor->setKinematicTarget(XMWorldToPx_NoScale(pTransform->Get_XMWorldMatrix()));
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
		 Update_Dynamic_ActiveActors();
		//Update_Dynamic_AllActors();
	}
}

void CPhysX_Manager::Update_Dynamic_ActiveActors()
{
	PSX::PxU32 iNumActiveActor = {};
	PSX::PxActor** ppActiveActors = m_pScene->getActiveActors(iNumActiveActor);

	for (PSX::PxU32 i = 0; i < iNumActiveActor; ++i) {
		PhsXUserData* pUserData = (PhsXUserData*)ppActiveActors[i]->userData;
		PSX::PxRigidDynamic* pActorDynamic = ppActiveActors[i]->is<PSX::PxRigidDynamic>();
		if (nullptr != pActorDynamic) {
			if (nullptr == ppActiveActors[i]->userData) {
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
}

void CPhysX_Manager::ClearScene()
{
	for (auto& pBody : m_pActiveBodys) {
		pBody->release();
	} m_pActiveBodys.clear();
	for (auto& pBody : m_pRestBodies) {
		pBody->release();
	} m_pRestBodies.clear();
}
void CPhysX_Manager::Attach_Actor(PSX::PxActor& Actor)
{
	unordered_set<PSX::PxActor*>::iterator iter = m_pRestBodies.find(&Actor);
	if (m_pRestBodies.end() == iter) {
		return;
	}
	else {
		m_pRestBodies.erase(&Actor);
	}
	m_pActiveBodys.insert(&Actor);
	if (m_pScene != nullptr) {
		m_pScene->addActor(Actor);
	}
}

unordered_set<PSX::PxActor*>::iterator CPhysX_Manager::Detach_Actor(PSX::PxActor& Actor)
{
	unordered_set<PSX::PxActor*>::iterator iterOut = m_pRestBodies.end();
	unordered_set<PSX::PxActor*>::iterator iter = m_pActiveBodys.find(&Actor);
	if (m_pActiveBodys.end() != iter) {
		m_pActiveBodys.erase(iter); // 액터를 활성화 맵에서 분리 해주고
		iterOut = m_pRestBodies.insert(&Actor).first; // 액터를 다시 쓸 수 있기 때문에 따로 보관해줌
		if (m_pScene != nullptr) {
			m_pScene->removeActor(Actor);
		}
	}
	return iterOut;
}

void CPhysX_Manager::Release_Actor(PSX::PxActor& Actor)
{
	// 혹시 활성화 맵에 들어있을 수도 있으니 검사
	unordered_set<PSX::PxActor*>::iterator iterResult = Detach_Actor(Actor);
	if (m_pRestBodies.end() != iterResult) { // 방금 전까지만 해도 활성화 중이었던 액터 라면
		m_pRestBodies.erase(iterResult); // 바로 지워줌
	}
	else {
		m_pActiveBodys.erase(&Actor);
		m_pRestBodies.erase(&Actor);
	}
	Actor.release();
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
		ASSERT_NURI(false);
	}

	{ // 씬 세팅
		m_ToleranceScale.length = 1.f; // 1 meter
		m_ToleranceScale.speed = GRAVITY; // 
		

		m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, m_ToleranceScale, true, m_pPvd);

		m_pCookingParam = new PSX::PxCookingParams(m_pPhysics->getTolerancesScale());
		m_pCookingParam->meshPreprocessParams |= PSX::PxMeshPreprocessingFlag::eWELD_VERTICES;
		PSX::PxSceneDesc sceneDesc = { m_pPhysics->getTolerancesScale() };

		sceneDesc.gravity = PSX::PxVec3(0.f, -GRAVITY, 0.f);

		m_pDispatcher = PSX::PxDefaultCpuDispatcherCreate(2/*cpu 코어 갯수*/);
		if (nullptr == m_pDispatcher) {
			assert(false);
			return E_FAIL;
		}
		sceneDesc.cpuDispatcher = m_pDispatcher;
		//sceneDesc.simulationEventCallback = ; // fetchResults 때 불림. 아래 필터랑, pair랑 같이 짝꿍임. 좀 더 공부해야 쓸 수 있을듯;;
		sceneDesc.filterShader = PSX::PxDefaultSimulationFilterShader; // 레이어 충돌, 오브젝트에 속성을 주고 복잡한 필터링 충돌 식
		sceneDesc.flags =
			PSX::PxSceneFlag::eENABLE_PCM // 기본 true, 현대적인 충돌시스템 적용
			| PSX::PxSceneFlag::eENABLE_ACTIVE_ACTORS // 현재 움직임이 있는 액터만 가져옴
			| PSX::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS // 그중에 키네틱은 뺌
			| PSX::PxSceneFlag::eENABLE_CCD // CCD 기본플래그, 키면 비쌈. 단일오브젝트만 켤 수 있으니 선택적으로 고려
			| PSX::PxSceneFlag::eDISABLE_CCD_RESWEEP // CCD 옵션플래그, 정확도 낮추고 비용 아낌
			//| PSX::PxSceneFlag::eDISABLE_CONTACT_REPORT_BUFFER_RESIZE // 접촉 리포트버퍼 리사이즈 금지 -> 보통 끔
			//| PSX::PxSceneFlag::eDISABLE_CONTACT_CACHE // 접촉 캐시를 금지 -> 그냥 켜두자, 캐시 있는게 보통 더 낫다
			//| PSX::PxSceneFlag::eREQUIRE_RW_LOCK // 멀티스레딩 환경에서 혹시라도 락 걸고 접근하지 않아 발생할 일을 제거, 
			//| PSX::PxSceneFlag::eENABLE_STABILIZATION // 물체가 얽힌 상황에서 불안정한걸 방지. 추가비용인듯
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

		m_pScene = m_pPhysics->createScene(sceneDesc);

		// CCT
		m_pCCTManager = PxCreateControllerManager(*m_pScene);
		m_pCCTManager->setOverlapRecoveryModule(true); // 복구모듈은 하이트필드에는 적용되지 않음, 오직 스태틱만 대상
		m_pCCTManager->setPreciseSweeps(true); // 정밀 스윕
		m_pCCTManager->setPreventVerticalSlidingAgainstCeiling(false); // 천장타고 슬라이딩 허용 비허용
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
	PlaneData.eKind = PHYSX_KIND::BODY_STATIC;
	PlaneData.iSubKind = UINT_MAX;
	PlaneData.pOwner = nullptr;
	PlaneData.pBody = nullptr;

	m_pMaterials.reserve(ENUM_CLASS(PXMATERIAL::END));
	m_pMaterials.push_back(m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f));
	PSX::PxRigidStatic* pGroundPlane = PxCreatePlane(*m_pPhysics, physx::PxPlane(0, 1, 0, 0), *m_pMaterials[ENUM_CLASS(PXMATERIAL::DEFAULT)]);
	pGroundPlane->userData = &PlaneData;
	pGroundPlane->setName("PHYSX_MANAGER_PLANE");
	m_pScene->addActor(*pGroundPlane);;

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
	if (false == (m_pPvd->connect(*m_pTransport, PSX::PxPvdInstrumentationFlag::eALL))) {
		return E_FAIL;
	}
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
	ClearScene();

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
