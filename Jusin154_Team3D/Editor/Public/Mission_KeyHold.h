#pragma once

#include "Editor_Define.h"
#include "UIObject.h"

NS_BEGIN(Editor)

class CMission_KeyHold final : public CUIObject
{
private:
	CMission_KeyHold(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMission_KeyHold(const CMission_KeyHold& rhs);
	virtual ~CMission_KeyHold() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;


private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


public:
	static CMission_KeyHold* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END