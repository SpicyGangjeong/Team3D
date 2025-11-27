#pragma once

#include "Editor_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Editor)

class CDecendo final : public CEffect_Container
{
private:
	CDecendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecendo(const CDecendo& rhs);
	virtual ~CDecendo() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
private:
	_wstring						  m_wstrEffectName = {};
	class CDummy_PhysXEffectHitBox*   m_pPhysHitBox = {};

	class CPartObject*				  m_pProjectile = {};
	class CPartObject*				  m_pProjectile_Blur = {};

	_vector							  m_vRotateUp = {};
	_float							  m_fAccTime = {};
	_float							  m_fLerpAmount = {};
	_float							  m_fTurnValue = {};
	_vector							  m_vOwnerLook = {};
public:
	static CDecendo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	void Describe_Entity() override;
};

NS_END
