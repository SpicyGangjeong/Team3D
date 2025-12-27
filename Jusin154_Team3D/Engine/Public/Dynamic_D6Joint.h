#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CTransform;
class CMesh;

class ENGINE_DLL CDynamic_D6Joint final : public CComponent
{
public:
	typedef struct tagDynamicJointDesc {
		PHYSX_JOINT				eType;
		PSX::PxRigidActor*		pActor0;
		PSX::PxRigidActor*		pActor1;
		const PSX::PxTransform* pxJointWorldPos;
	}DYNAMICJOINT_DESC;
private:
	CDynamic_D6Joint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_D6Joint(const CDynamic_D6Joint& rhs);
	~CDynamic_D6Joint() = default;

public:
#ifdef _DEBUG
	virtual HRESULT Render()override;
#endif // _DEBUG
	PSX::PxD6Joint* Get_JointPtr() { return m_pJoint; };
private:
	PSX::PxD6Joint* m_pJoint = { nullptr };

#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pMainShape = { nullptr };
#endif // _DEBUG

private:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	HRESULT Add_DebugShape();
#endif

public:
	static CDynamic_D6Joint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
