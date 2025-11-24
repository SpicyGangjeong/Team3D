#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CDummy_PhysXWall final : public CGameObject
{
public:
	typedef struct tagPhysXDummyDesc
	{
		_float3 vPos = { };
		_float3 vRotRPY = { };
		_uint iSubKind = { };
	}PHYSXDUMMY_DESC;
private:
	CDummy_PhysXWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXWall(const CDummy_PhysXWall& rhs);
	virtual ~CDummy_PhysXWall() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	const PSX::PxRigidDynamic* m_pActor = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CDummy_PhysXWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
