#pragma once

#include "Editor_Define.h"
#include "UIObject.h"

NS_BEGIN(Editor)

class CLodingWidget1 final : public CUIObject
{
private:
	CLodingWidget1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLodingWidget1(const CLodingWidget1& rhs);
	virtual ~CLodingWidget1() = default;

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
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_int m_iImageFrameX{};
	_float m_fFrame{};
public:
	static CLodingWidget1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END