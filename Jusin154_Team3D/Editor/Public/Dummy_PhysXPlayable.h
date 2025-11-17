#pragma once

#include "Editor_Define.h"
#include "GameObject.h"
#include "CallBack_Playable_Behavior.h"
#include "CallBack_Playable_HitReport.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CDummy_PhysXPlayable final : public CGameObject
{
public:
	typedef struct tagPhysXDummyDesc
	{
		_float3 vPos = { };
		_float3 vRotRPY = { };
		_uint iSubKind = { };
	}PHYSXDUMMY_DESC;
private:
	CDummy_PhysXPlayable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_PhysXPlayable(const CDummy_PhysXPlayable& rhs);
	virtual ~CDummy_PhysXPlayable() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCharacter_Controller*		m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic*			m_pRigidBody = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	
	CCallBack_Playable_Behavior* m_pCallBack_Behavior = { nullptr };
	CCallBack_Playable_HitReport* m_pCallBack_HitReport = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CDummy_PhysXPlayable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
