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
		_bool bAutoOwnerTranslation = true;
	}RIGIDBODY_DYNAMIC_DESC;

private:
	CRigidBody_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody_Dynamic(const CRigidBody_Dynamic& rhs);
	virtual ~CRigidBody_Dynamic() = default;

public:
#ifdef RELEASE_DEBUGGER
	_float3 Get_LocalTranslation() {return m_vLocalTranslation;	}
	virtual HRESULT Render()override;
	HRESULT Render(function<void()> custumState);
	HRESULT Render(function<void()> custumState, _fvector vColor);
#endif // RELEASE_DEBUGGER

	virtual PSX::PxRigidDynamic* Get_Actor()		override { return m_pRigidBody; };
	void					Set_Name(const _char* szName);
	_float3					Get_HalfGeometryInfo()	const { return m_vhalfGeometryInfo; }
	void					Set_HalfGeometryInfo(_float3 vhalfGeometryInfo);

	
	void					Move_LocalPos(_matrix newLocalPos);// 캡슐은 이 함수 쓸 거면 말하고 써야함
	void					Move_LocalPos(_fvector vNewRotQ, _gvector vNewTranslation);// 캡슐은 이 함수 쓸 거면 말하고 써야함
	void					Move_LocalPos(_float4& vNewRotQ, _float3& vNewTranslation);// 캡슐은 이 함수 쓸 거면 말하고 써야함
	_float					Get_Density()			const { return m_fDensity; }
	_float					Get_Mass();

	virtual void			Add_Force(_fvector vForce, PSX::PxForceMode::Enum eType = PSX::PxForceMode::Enum::eFORCE); // 속도 + 방향
	virtual void			Add_Torque(_fvector vDirection, PSX::PxForceMode::Enum eType = PSX::PxForceMode::Enum::eFORCE); // 방향 ( 회전축은 MassCenter )
	void					Set_Kinematic(_bool bKinematic);
	void					Move_Kinematic(_fmatrix xmTransform, _bool bTeleport = true);
	void					Move_Kinematic(_fvector vPos, _gvector vRotq, _bool bTeleport = true);
	void					Move_Kinematic(PSX::PxTransform& pxDestination, _bool bTeleport = true);

	HRESULT					ConvertToCCT(class CCharacter_Controller& CCTOriginal);
	_bool					IsActive() const { return m_bActive; }
	void					SetActive(_bool bCondition) { m_bActive = bCondition; }
	void					Detach_Actor(_uint iLevel);

	_vector					Get_Position();
	void					Set_Position(_vector vPos, _bool bTeleport);
	void					Set_Rotation(_vector vRotQ, _bool bTeleport);
	void					Set_AutoOwnerTranlation(_bool bAutoOwnerTranslation) { m_tagData.bAutoOwnerTranslation = bAutoOwnerTranslation; };
	_bool					Get_AutoOwnerTranlation() { return m_tagData.bAutoOwnerTranslation; };

	void					Set_Transform(_matrix WorldMatrix, _bool bTeleport);
	void					Set_Transform(_vector vPos, _vector vRotQ, _bool bTeleport);
	void					Set_Transform(PSX::PxTransform pxTransform, _bool bTeleport);

	void					Set_CenterTransform(_vector vStart, _vector vEnd, _bool bTeleport);
	_float3					Get_FootPosition();
	_float3					Get_HeadPosition();
	PSX::PxTransform		Get_GlobalPosition();
	PSX::PxTransform		Get_HeadPositionPxTransform();
	PSX::PxTransform		Get_FootPositionPxTransform();
private:
	PSX::PxRigidDynamic*	m_pRigidBody = { nullptr };		// 실제 시뮬레이션을 도는 본체
	PSX::PxRigidDynamicLockFlags m_eLockFlag = { };
	PSX::PxTransform		m_PxMassCenter = {}; // 회전축
	_float3					m_vMatInfo = {};
	_float3					m_vhalfGeometryInfo = {};
	_float2					m_vDamping = { 0.f, 0.f };
	_float					m_fDensity = { 1000.f };
	_bool					m_bActive = { true };

#ifdef RELEASE_DEBUGGER
	unique_ptr<GeometricPrimitive> m_pMainShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
#endif // RELEASE_DEBUGGER

private:
	HRESULT Initialize_Prototype(RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;
//	void Move(PSX::PxTransform& pxTransform, _bool bTeleport);
	_bool IsKinematic();

#ifdef RELEASE_DEBUGGER
	HRESULT Add_DebugShape();
#endif

public:
	static CRigidBody_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
