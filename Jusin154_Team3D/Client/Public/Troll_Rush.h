#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"


NS_BEGIN(Client)

class CTroll_Rush final : public CEffect_Container
{
private:
	CTroll_Rush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTroll_Rush(const CTroll_Rush& rhs);
	virtual ~CTroll_Rush() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual	HRESULT	Pre_Setting(CGameObject* pObject, void* pArg = nullptr) override;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
	virtual void	OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr) override;
private:
	_wstring						  m_wstrEffectName = {};
	
	class CEffectParts*				  m_pRush_Blur = { nullptr };

public:
	static CTroll_Rush* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	void Describe_Entity() override;
};

NS_END
