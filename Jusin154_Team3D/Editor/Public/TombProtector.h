#pragma once

#include "Editor_Define.h"
#include "Monster.h"

NS_BEGIN(Editor)

class CTombProtector final : public CMonster
{
private:
	CTombProtector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTombProtector(const CTombProtector& Prototype);
	virtual ~CTombProtector() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTombProtector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END