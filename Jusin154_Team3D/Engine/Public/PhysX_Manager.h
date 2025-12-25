#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CRigidBody;
class CRigidBody_Dynamic;
class CRigidBody_Static;
class CTransform;
class CMesh;

class CPhysX_Manager final : public CBase
{
private:
	CPhysX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysX_Manager() = default;
#pragma region RIGID_BODY

public:
	/* dev-treadmill.tistory.com/155 */
// HRESULT Update_RegionOfInterest(); ( 충돌 컬링, 필터링도 검색해야함 )
// Aggregate ( 충돌 그룹, 이 그룹끼리는 서로 충돌검사를 하지 않음, 지형, 레그돌 등 )

	PSX::PxRigidDynamic* Add_DynamicActor(CRigidBody_Dynamic& RigidBody, _uint iLevel);
	PSX::PxRigidStatic* Add_StaticActor(CRigidBody_Static& RigidBody, _uint iLevel);
	
	// https://documentation.help/nvidia-physx-sdk-guide/Joints.html
	PSX::PxJoint* Create_PxJoint(PHYSX_JOINT eType, PSX::PxRigidActor* pActor0, PSX::PxTransform& pxLocalFrame0, PSX::PxRigidActor* pActor1, PSX::PxTransform& pxLocalFrame1);
	
	void RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh, _uint iLevel);
	void RegistHeight(const _tchar* pName, PSX::PxHeightFieldDesc& Desc, _uint iLevel);
	PSX::PxMaterial* Create_Material(const _float3* vMatInfo);

	_bool SphereCast(_float fRadius, _float3 vStartPos, _float3 vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer);
	_bool SphereCast(_float fRadius, _fvector vStartPos, _gvector vDir, _float fDistance, PSX::PxHitFlags flagHitsData, PSX::PxQueryFlags flagQuery, PSX::PxSweepBuffer& hitBuffer);
	_bool RayCast(_float3 _vStartPos, _float3 _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount);
	_bool RayCast(_fvector _vStartPos, _fvector _vDir, _float fDistance, PSX::PxRaycastHit* pRayHitArray, _uint iMaxHitCapacity, _uint& iOutHitCount);

	
	HRESULT ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<class PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix);

#ifdef EDITOR_PROJECT

	HRESULT SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
	void Add_Editor_Plane(PhsXUserData& PlaneData);
#endif // EDITOR_PROJECT
	HRESULT LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
	//HRESULT LoadTriMeshes_Binary(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
	PSX::PxTriangleMesh* Find_TriangleMesh(const _tchar* pMeshName, _uint iLevel);
	PSX::PxHeightField* Find_HeightField(const _tchar* pFieldName, _uint iLevel);

	void Update(_float fTimeDelta);
	void ClearScene(_uint iLevel);
	void Attach_Actor(PSX::PxActor& Actor, _uint iLevel);
	unordered_set<PSX::PxActor*>::iterator Detach_Actor(PSX::PxActor& Actor, _uint iLevel);
	void Release_Actor(PSX::PxActor& Actor);
#pragma endregion
#pragma region CHARACTER_CONTROLLER
	/* dev-treadmill.tistory.com/158  */

public:
	PSX::PxController*	Add_CapsuleController(PSX::PxCapsuleControllerDesc& Desc);
	PSX::PxController*	Add_BoxController(PSX::PxBoxControllerDesc& Desc);

	PSX::PxController*	Get_Controller(_uint iControllerIndex);
	void				ReleaseController(_uint iControllerIndex);
	_uint				Get_NumCurrentController();
	HRESULT				PurgeAllController();

#pragma endregion


private:
	class CGameInstance*				m_pGameInstance = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	ID3D11Device*						m_pDevice = { nullptr };

	PSX::PxDefaultAllocator				m_AllocatorCallBack = { };						// 피직스 동적할당자
	PSX::PxDefaultErrorCallback			m_ErrorCallBack = { };							// 피직스 오류처리자
	PSX::PxDefaultCpuDispatcher*		m_pDispatcher = { nullptr };					// 피직스 CPU디스패쳐 ( 현재 피직스는 CPU only )
	PSX::PxTolerancesScale				m_ToleranceScale = PSX::PxTolerancesScale();	// 피직스 공차의 집합 ( 기본 단위 집합 )
	PSX::PxFoundation*					m_pFoundation = { nullptr };					// 피직스 초기화 종료 담당자
	PSX::PxPhysics*						m_pPhysics = { nullptr };						// 피직스 객체의 생성, 초기화, 조작 담당 (싱글톤)

	PSX::PxScene*						m_pScene = { nullptr };							// 피직스 시뮬레이션의 공간
	PSX::PxControllerManager*			m_pCCTManager = { nullptr };					// 피직스 캐릭터 컨트롤러를 전부 추적하고 매니징함
#ifdef _DEBUG
	PSX::PxPvd* m_pPvd = { nullptr };							// 피직스 시뮬레이션 디버깅을 위한 도구
#endif // _DEBUG
	PSX::PxCookingParams*				m_pCookingParam = { nullptr };
	PSX::PxPvdTransport*				m_pTransport = { nullptr };


	_uint										m_iMaxLevel = {};

	unordered_set<PSX::PxActor*>*				m_pActiveBodys = { nullptr };
	unordered_set<PSX::PxActor*>*				m_pRestBodies = { nullptr }; // 피직스의 액터로 구별되기 때문에 피직스액터 포인터를 키로 사용함

	map<_wstring, PSX::PxTriangleMesh*>*		m_TriangleMeshes = { nullptr };
	map<_wstring, PSX::PxTriangleMeshGeometry*>*m_TriangleMeshGeometry = { nullptr };

	map<_wstring, PSX::PxHeightField*>*			m_HeightFields = { nullptr };
	map<_wstring, PSX::PxHeightFieldGeometry*>* m_HeightFieldGeometry = { nullptr };


	PhsXUserData PlaneData = {};

	vector<PSX::PxMaterial*> m_pMaterials = { };
	_uint m_iNumLevel = { UINT_MAX };
private:
	HRESULT Initialize(_uint iLevel);
#ifdef _DEBUG
	HRESULT Connect_DebugServer();
#endif // _DEBUG

	void Update_Kinematic();
	void Update_Dynamic_ActiveActors();
	//void Update_Dynamic_AllActors();

public:
	static CPhysX_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual void Free() override;
};

NS_END
