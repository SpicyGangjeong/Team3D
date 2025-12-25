#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CTransform;
class CMesh;

class ENGINE_DLL CDynamic_Joint abstract : public CComponent
{
	typedef struct tagDynamicJointDesc {
		PHYSX_JOINT			eType;
		PSX::PxActor*		pActor0;
		PSX::PxTransform*	pLocalFrame0;
		PSX::PxActor*		pActor1;
		PSX::PxTransform*	pLocalFrame1;
	}DYNAMICJOINT_DESC;
private:
	CDynamic_Joint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_Joint(const CDynamic_Joint& rhs);
	~CDynamic_Joint() = default;

public:
#ifdef _DEBUG
	virtual HRESULT Render()override;
#endif // _DEBUG

private:

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
	static CDynamic_Joint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
