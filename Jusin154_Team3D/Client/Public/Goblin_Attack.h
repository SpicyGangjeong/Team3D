#pragma once

#include "Client_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CGoblin_Attack final : public CEffect_Container
{
private:
	CGoblin_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_Attack(const CGoblin_Attack& rhs);
	virtual ~CGoblin_Attack() = default;

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
	class CEffectParts*					  m_pPT1 = { nullptr };
	class CEffectParts*					  m_pPT2 = { nullptr };
public:
	static CGoblin_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
