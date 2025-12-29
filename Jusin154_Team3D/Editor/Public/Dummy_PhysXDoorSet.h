#pragma once

#include "Editor_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CDummy_PhysXDoorSet final : public CContainerObject
{
public:
	typedef struct tagPhysXDummyDesc
	{
		_float3 vPos = { };
		_float3 vRotRPY = { };
		_float fAngleLimit = {};
	}PHYSXDUMMY_DESC;
private:
	CDummy_PhysXDoorSet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXDoorSet(const CDummy_PhysXDoorSet& rhs);
	virtual ~CDummy_PhysXDoorSet() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CDummy_PhysXFixedDoor* m_pDoorPart = { nullptr };
	PSX::PxRevoluteJoint* m_pPSXJoint = { nullptr };
	_float3		m_vRadianYAngle = {};


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Ready_PartObject(void* pArg);
	virtual HRESULT Bind_ShaderResources() override;


public:
	static CDummy_PhysXDoorSet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
