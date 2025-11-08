#pragma once

#include "Editor_Define.h"
#include "ModelParts.h"

NS_BEGIN(Editor)

class CHead final : public CModelParts
{
private:
	CHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHead(const CHead& Prototype);
	virtual ~CHead() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();

public:
	static CHead* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END