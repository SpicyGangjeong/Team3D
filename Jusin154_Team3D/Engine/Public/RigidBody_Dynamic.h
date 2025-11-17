#pragma once
#include "RigidBody.h"

NS_BEGIN(Engine)
class CTransform;
class CMesh;

class ENGINE_DLL CRigidBody_Dynamic : public CRigidBody
{
public:
	typedef struct tagRigidBody_Prototype_DYNAMIC_Desc : public CRigidBody::RIGIDBODY_PROTOTYPEDESC
	{
		_float3					vhalfGeometryInfo;		// 박스는 vSize, 캡슐은 radius, height로 씀
		_float					fDensity = { 1000.f };	// 밀도, 실제 무게는 부피에 따라 달라짐
		PSX::PxRigidDynamicLockFlags eLockFlag = {};
		PSX::PxTransform		pxMassCenter = {};
		_float2					vAutoDamping = {}; // 0, 0 이면 off, 선형감쇠값, 각 감쇠값
	}RIGIDBODY_PROTOTYPE_DYNAMIC_DESC;

	typedef struct tagRigidBody_DYNAMIC_Desc : public CRigidBody::RIGIDBODY_DESC
	{

	}RIGIDBODY_DYNAMIC_DESC;

private:
	CRigidBody_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody_Dynamic(const CRigidBody_Dynamic& rhs);
	virtual ~CRigidBody_Dynamic() = default;

public:
#ifdef _DEBUG
	virtual HRESULT Render()override;
#endif // _DEBUG

	virtual PSX::PxRigidDynamic* Get_Actor()		override { return m_pRigidBody; };
	_float3					Get_HalfGeometryInfo()	const { return m_vhalfGeometryInfo; }
	_float					Get_Density()			const { return m_fDensity; }

	virtual void			Add_Force(_fvector vForce, PSX::PxForceMode::Enum eType = PSX::PxForceMode::Enum::eFORCE); // 속도 + 방향
	virtual void			Add_Torque(_fvector vDirection, PSX::PxForceMode::Enum eType = PSX::PxForceMode::Enum::eFORCE); // 방향 ( 회전축은 MassCenter )
	void					Set_Kinematic(_bool bKinematic);

	HRESULT					ConvertToCCT(class CCharacter_Controller& CCTOriginal);
	_bool					IsActive() const { return m_bActive; }
	void					SetActive(_bool bCondition) { m_bActive = bCondition; }

private:
	PSX::PxRigidDynamic*	m_pRigidBody = { nullptr };		// 실제 시뮬레이션을 도는 본체
	PSX::PxRigidDynamicLockFlags m_eLockFlag = { };
	PSX::PxTransform		m_PxMassCenter = {}; // 회전축
	_float3					m_vMatInfo = {};
	_float3					m_vhalfGeometryInfo = {};
	_float2					m_vDamping = { 0.f, 0.f };
	_float					m_fDensity = { 1000.f };
	_bool					m_bActive = { true };

#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pMainShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
#endif // _DEBUG

private:
	HRESULT Initialize_Prototype(RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	HRESULT Add_DebugShape();
#endif

public:
	static CRigidBody_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
