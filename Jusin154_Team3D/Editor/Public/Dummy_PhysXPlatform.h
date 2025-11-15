#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CDummy_PhysXPlatform final : public CGameObject
{
public:
	typedef struct tagBoxStartPos
	{
		_float3 vPos = {};
		_float3 vRotRPY = {};
	}BOXSTARTPOS_DESC;
private:
	CDummy_PhysXPlatform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXPlatform(const CDummy_PhysXPlatform& rhs);
	virtual ~CDummy_PhysXPlatform() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

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
	static CDummy_PhysXPlatform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
