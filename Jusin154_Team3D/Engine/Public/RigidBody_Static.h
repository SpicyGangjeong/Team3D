#pragma once
#include "RigidBody.h"

NS_BEGIN(Engine)
class CTransform;
class CMesh;


class ENGINE_DLL CRigidBody_Static : public CRigidBody
{
public:
	typedef struct tagRigidBody_Prototype_STATIC_Desc :  public CRigidBody::RIGIDBODY_PROTOTYPEDESC
	{

	}RIGIDBODY_STATIC_PROTOTYPEDESC;

	typedef struct tagRigidBody_STATIC_Desc : public CRigidBody::RIGIDBODY_DESC
	{
		const _tchar* pMeshName = { nullptr };
	}RIGIDBODY_STATIC_DESC;

private:
	CRigidBody_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody_Static(const CRigidBody_Static& rhs);
	virtual ~CRigidBody_Static() = default;

public:
	PSX::PxRigidStatic*	Get_Actor()		override { return m_pRigidBody; }
	const _tchar*		Get_PxMeshKey()	const { return m_wstrMeshName.c_str(); }

private:
	PSX::PxRigidStatic*	m_pRigidBody = { nullptr };		// 실제 시뮬레이션을 도는 본체
	_wstring		m_wstrMeshName = { };		// 짝꿍 스태틱 메시의 triangleMesh 키

private:
	HRESULT Initialize_Prototype(RIGIDBODY_STATIC_PROTOTYPEDESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CRigidBody_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_STATIC_PROTOTYPEDESC& Desc);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
