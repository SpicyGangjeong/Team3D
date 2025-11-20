#pragma once

#include "Editor_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Editor)

class CNomalJap final : public CEffect_Container
{
private:
	CNomalJap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNomalJap(const CNomalJap& rhs);
	virtual ~CNomalJap() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;

private:
	_wstring		m_wstrEffectName = {};

public:
	static CNomalJap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	CGameObject* Clone(void* pArg, CGameObject* pOwner) override;
	void Describe_Entity() override;
};

NS_END
