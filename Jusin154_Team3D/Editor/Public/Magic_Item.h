#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CMagic_Item final : public CElementObject
{
private:
	CMagic_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagic_Item(const CMagic_Item& rhs);
	virtual ~CMagic_Item() = default;

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
	void Compute_UV(_uint iItemID);
	void Compute_Image();
private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CTexture* m_pDiffuse_TextureCom4 = { nullptr };
	CTexture* m_pDiffuse_TextureCom5 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float4 m_vUV{};
	_uint	m_iArratCount{};
	_float2 m_vImageSize1 = {};
	_float2 m_vImagePos1 = {};
	_float2 m_vImageSize2 = {};
	_float2 m_vImagePos2 = {};
public:
	static CMagic_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
