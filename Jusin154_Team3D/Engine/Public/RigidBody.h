#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CTransform;
class CMesh;

class ENGINE_DLL CRigidBody abstract : public CComponent
{
protected: // MORE INFO ON ENGINE_ENUM
	typedef struct tagRigidBody_PrototypeDesc // 다 채우세요 NONE 빼고, 목적에 안맞는거 빼고
	{
		ACTOR					eType;
		PSX::PxRigidBodyFlags	ePxRigidBodyFlags = { /* NONE */};
		PSX::PxShapeFlags		ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		PXMATERIAL				ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		_float3					vMatInfo = { 0.5f, 0.5f, 0.6f }; // 피직스 객체의 속성 등
		_float					fContactOffset = { 0.05f }; // 접촉 유격 오프셋

	}RIGIDBODY_PROTOTYPEDESC;

	typedef struct tagRigidBody_Desc
	{
		_uint iSubKind = { };
	}RIGIDBODY_DESC;

protected:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& rhs);
	virtual ~CRigidBody() = default;

public:
	ACTOR					Get_Type()				const { return m_eActorType; }
	virtual PSX::PxActor*	Get_Actor()PURE;
	CTransform*				Get_TransformPtr()		const { return m_pTransform; }
	PXMATERIAL				Get_MaterialType()		const { return m_eMatType; }
	_float					Get_ContactOffset()		const { return m_fContactOffset; }
	PSX::PxShapeFlags		Get_ShapeFlags()		const { return m_ePxShapeFlags; }
	PSX::PxRigidBodyFlags	Get_RigidBodyFlags()	const { return m_ePxRigidBodyFlags; }

protected:
	ACTOR					m_eActorType = ACTOR::END;
	PSX::PxRigidBodyFlags	m_ePxRigidBodyFlags = { };
	PSX::PxShapeFlags		m_ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
	PXMATERIAL				m_eMatType = { PXMATERIAL::END };
	_float					m_fContactOffset = { 0.05f };

	CTransform*				m_pTransform = { nullptr };
	PhsXUserData			m_tagData = {};

protected:
	virtual HRESULT Initialize(void* pArg);

public:
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
