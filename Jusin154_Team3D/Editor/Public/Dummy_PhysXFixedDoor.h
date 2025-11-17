#pragma once

#include "Editor_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CDummy_PhysXFixedDoor final : public CPartObject
{
public:
	typedef struct tagPhysXDummyDesc : public CPartObject::PARTOBJECT_DESC
	{
		_float3 vPos = { };
		_float3 vRotRPY = { };
		_uint iSubKind = { };
	}PHYSXDUMMY_DESC;
private:
	CDummy_PhysXFixedDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXFixedDoor(const CDummy_PhysXFixedDoor& rhs);
	virtual ~CDummy_PhysXFixedDoor() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	PSX::PxRigidDynamic* Get_Actor();

private:
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	const PSX::PxRigidDynamic* m_pActor = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;


public:
	static CDummy_PhysXFixedDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
