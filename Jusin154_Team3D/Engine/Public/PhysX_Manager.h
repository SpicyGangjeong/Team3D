#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CRigidBody;
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

	PSX::PxRigidDynamic* Add_DynamicActor(CRigidBody& RigidBody);
	PSX::PxRigidStatic* Add_StaticActor(CRigidBody& RigidBody);
	
	void RegistTriMesh(const _char* pName, PSX::PxTriangleMesh* pPxTriMesh);
	PSX::PxMaterial* Create_Material(const _float3* vMatInfo);

	HRESULT ConvertToTriMeshes(vector<class CMesh*>& Meshes, vector<class PSX::PxTriangleMesh*>& pxTriMeshes, _fmatrix WorldMatrix);
	HRESULT SaveTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
	HRESULT LoadTriMeshes(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
	//HRESULT LoadTriMeshes_Binary(const _char* pPath, vector<PSX::PxTriangleMesh*>& TriMeshes);
	PSX::PxTriangleMesh* Find_TriangleMesh(const _wstring& wstrMeshKey);

	void Update(_float fTimeDelta);
	void ClearScene();
	void Attach_Actor(CRigidBody& RigidBody, PSX::PxActor& Actor);
	void Detach_Actor(CRigidBody& RigidBody, PSX::PxActor*& pActor);
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
	PSX::PxPvd*							m_pPvd = { nullptr };							// 피직스 시뮬레이션 디버깅을 위한 도구
	PSX::PxCookingParams*				m_pCookingParam = { nullptr };
	PSX::PxPvdTransport*				m_pTransport = { nullptr };



	list<pair<CRigidBody*, PSX::PxActor*>>		m_RigidBodys = { };
	map<_wstring, PSX::PxTriangleMesh*>			m_TriangleMeshes = {};
	map<_wstring, PSX::PxTriangleMeshGeometry*>	m_TriangleMeshGeometry = {};

	vector<PSX::PxMaterial*> m_pMaterials = { };
	_uint m_iNumLevel = {};
private:
	HRESULT Initialize();
	HRESULT Connect_DebugServer();
	void Update_Kinematic();
	void Update_Dynamic_ActiveActors();
	//void Update_Dynamic_AllActors();

public:
	static CPhysX_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
