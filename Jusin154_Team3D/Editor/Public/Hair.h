#pragma once

#include "Editor_Define.h"
#include "ModelParts.h"
NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Editor)

class CHair final : public CModelParts
{
private:
	CHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHair(const CHair& Prototype);
	virtual ~CHair() = default;

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
private:
	CTexture* m_pTextureCom = { nullptr };
	_uint iTextureIndex = { 0 };

public:
	static CHair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
