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
public:
	typedef struct tagRigidStatic_PrototypeDesc
	{
		_float3			vMatInfo;
		const _char*	szMeshName = { } ;
	}RIGID_STATIC_PROTOTYPEDESC;
	typedef struct tagRigidDynamic_PrototypeDesc
	{
		_float3						vMatInfo;
		_float3						vhalfGeometryInfo;
		_bool						bExclusive = { false };
		PSX::PxShapeFlags			ePxShapeFlag = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
	}RIGID_DYNAMIC_PROTOTYPEDESC;

	typedef struct tagRigidBody_PrototypeDesc
	{
		ACTOR						eType;
		union 
		{
			RIGID_STATIC_PROTOTYPEDESC		tRigidStaticDesc;
			RIGID_DYNAMIC_PROTOTYPEDESC		tRigidDynamicDesc;
		}; 
	}RIGIDBODY_PROTOTYPEDESC;
	typedef struct tagRigidStatic_Desc {
		const _char* szMeshName = { };
	}RIGID_STATIC_DESC;
	typedef struct tagRigidDynamic_Desc {
		_float			fDensity = { 1000.f };
		_bool			bIsKinematic = { false };
	}RIGID_DYNAMIC_DESC;
	typedef struct tagRigidBody_Desc
	{
		ACTOR eType;
		union
		{
			RIGID_STATIC_DESC		tRigidStaticDesc;
			RIGID_DYNAMIC_DESC		tRigidDynamicDesc;
		};
		// 키네마틱을 켜면, 
		// 프레임 별 직접 위치 지정이 가능하다.
		// 물리엔진의 힘, 중력 등의 영향을 받지 않는다.
		// 단, 다른 물리엔진 오브젝트와의 충돌은 발생한다.
		// 즉, 강제로 움직이지만, 충돌은 시뮬레이션 한다.
		// 그러므로 키네마틱 오브젝트는 다른 오브젝트에 영향을 주지만, 다른 오브젝트의 힘에 영향을 받지 않는다.
		// 주로 움직이는 플랫폼, 문 등에 사용된다.
		// 스태틱 리지드바디와 달리 움직일 수 있다 !!!
	}RIGIDBODY_DESC;

private:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;
public:
	ACTOR Get_Type() const { return m_eActorType; }
	PSX::PxShape* Get_ShapePtr() const { return m_pShape; }
	CTransform* Get_PxTransformPtr() const { return m_pTransform; }
	const _tchar* Get_PxMeshKey() const { return m_wstrMeshKey.c_str(); }
	const PSX::PxMaterial* Get_PxMaterial() const { return m_pMaterial; }
	_float Get_ContactOffset() { return m_fContactOffset; }
	_float Get_Density() const { return m_fDensity; }
	_bool Is_Kinematic() const { return m_bKinematic; }

private:
	ACTOR				m_eActorType = ACTOR::END;
	const PSX::PxActor*	m_pRigidBody = { nullptr };				// 실제 시뮬레이션을 도는 본체
	PSX::PxMaterial*	m_pMaterial = { nullptr };				// 피직스 객체의 속성 등
	PSX::PxShape*		m_pShape = { nullptr };					// 피직스 객체의 모양 ( 머테리얼로 만들어짐 )
	_float3				m_vhalfGeometryInfo = {};
	_float				m_fContactOffset = { 0.05f };
	CTransform*			m_pTransform = { nullptr };
	_bool				m_bKinematic = { false };
	_bool				m_bExclusive = { false };
	_float				m_fDensity = { 1000.f };
	_wstring			m_wstrMeshKey = {  };

private:
	HRESULT Initialize_Prototype(RIGIDBODY_PROTOTYPEDESC& Desc);
	HRESULT Initialize(void* pArg);

public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPEDESC& Desc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END