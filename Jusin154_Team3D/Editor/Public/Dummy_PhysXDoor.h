#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CDummy_PhysXDoor final : public CGameObject
{
public:
	typedef struct tagPhysXDummyDesc
	{
		_float3 vPos = { };
		_float3 vRotRPY = { };
		_uint iSubKind = { };
		_float fAngleLimit = {};
	}PHYSXDUMMY_DESC;
private:
	CDummy_PhysXDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXDoor(const CDummy_PhysXDoor& rhs);
	virtual ~CDummy_PhysXDoor() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	const PSX::PxRigidDynamic* m_pActor = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	_float3		m_vRadianYAngle = {};


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;
	_float ClampRadian(_float fNewRadian);


public:
	static CDummy_PhysXDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
