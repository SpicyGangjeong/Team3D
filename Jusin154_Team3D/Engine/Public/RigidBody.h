#pragma once
#include "Component.h"

NS_BEGIN(PSX)
class PxActor;
class PxMaterial;
NS_END

NS_BEGIN(Engine)
class CTransform;
class CMesh;

class ENGINE_DLL CRigidBody final : public CComponent
{
public: // MORE INFO ON ENGINE_ENUM
	typedef struct tagRigidBody_PrototypeDesc // 다 채우세요 NONE 빼고, 목적에 안맞는거 빼고
	{
		ACTOR					eType;
		PSX::PxRigidBodyFlags	ePxRigidBodyFlags = { /* NONE */};
		PSX::PxShapeFlags		ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		PXMATERIAL				ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		_float3					vMatInfo = { 0.5f, 0.5f, 0.6f }; // 피직스 객체의 속성 등
		_float					fContactOffset = { 0.05f }; // 접촉 유격 오프셋

#pragma region DYNAMIC
		_float3					vhalfGeometryInfo;		// 박스는 vSize, 캡슐은 radius, height로 씀
		_float					fDensity = { 1000.f };	// 밀도, 실제 무게는 부피에 따라 달라짐
		
#pragma endregion
#pragma region STATIC
#pragma endregion
	}RIGIDBODY_PROTOTYPEDESC;

	typedef struct tagRigidBody_Desc
	{
#pragma region DYNAMIC
#pragma endregion
#pragma region STATIC
		const _char* szMeshName = { }; 
#pragma endregion
	}RIGIDBODY_DESC;

private:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
#ifdef _DEBUG
	virtual HRESULT Render()override;
#endif // _DEBUG


	ACTOR					Get_Type()				const { return m_eActorType; }
	PSX::PxActor*			Get_Actor()				const { return m_pRigidBody; }
	CTransform*				Get_TransformPtr()		const { return m_pTransform; }
	const _tchar*			Get_PxMeshKey()			const { return m_wstrMeshKey.c_str(); }
	PXMATERIAL				Get_MaterialType()		const { return m_eMatType; }
	_float					Get_ContactOffset()		const { return m_fContactOffset; }
	_float					Get_Density()			const { return m_fDensity; }
	PSX::PxShapeFlags		Get_ShapeFlags()		const { return m_ePxShapeFlags; }
	PSX::PxRigidBodyFlags	Get_RigidBodyFlags()	const { return m_ePxRigidBodyFlags; }
	_float3					Get_HalfGeometryInfo()	const { return m_vhalfGeometryInfo; }

	
	// 키네마틱은 transform으로 이동 ( simulate 전에 이동함 )
	// 키네마틱이 아닌 dynamic은 토크와 포스로 이동 ( simulate 후에 이동함 )
	void					Set_Kinematic(_bool bKinematic);


	void					Add_Force(_fvector vForce, PSX::PxForceMode::Enum eType = PSX::PxForceMode::Enum::eFORCE); // 속도 + 방향
	void					Add_Torque(_fvector vDirection, PSX::PxForceMode::Enum eType = PSX::PxForceMode::Enum::eFORCE); // 방향

private:
	ACTOR					m_eActorType = ACTOR::END;
	PSX::PxRigidBodyFlags	m_ePxRigidBodyFlags = {};
	PXMATERIAL				m_eMatType = { PXMATERIAL::END };
	_float					m_fContactOffset = { 0.05f };
	PSX::PxActor*			m_pRigidBody = { nullptr };		// 실제 시뮬레이션을 도는 본체

	CTransform*				m_pTransform = { nullptr };
	PhsXUserData			m_tagData = {};

#pragma region DYNAMIC
	PSX::PxShapeFlags		m_ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
	_float3					m_vMatInfo = {};
	_float3					m_vhalfGeometryInfo = {};
	_float3					m_vCenterOfMess = {}; // 회전축
	_float					m_fDensity = { 1000.f };
#pragma endregion
#pragma region STATIC
	_wstring				m_wstrMeshKey = {  };			// 짝꿍 스태틱 메시의 triangleMesh 키
#pragma endregion
#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pMainShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
#endif // _DEBUG

private:
	HRESULT Initialize_Prototype(RIGIDBODY_PROTOTYPEDESC& Desc);
	HRESULT Initialize(void* pArg);
	HRESULT Initialize_UserData();
#ifdef _DEBUG
	HRESULT Add_DebugShape();
#endif

public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPEDESC& Desc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
