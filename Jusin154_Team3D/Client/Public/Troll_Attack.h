#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CTroll_Attack final : public CEffect_Container
{
private:
	CTroll_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTroll_Attack(const CTroll_Attack& rhs);
	virtual ~CTroll_Attack() = default;

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

public:
	static CTroll_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
