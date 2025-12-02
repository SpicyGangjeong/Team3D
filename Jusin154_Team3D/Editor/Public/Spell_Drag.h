#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpell_Drag final : public CElementObject
{
private:
	CSpell_Drag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_Drag(const CSpell_Drag& rhs);
	virtual ~CSpell_Drag() = default;

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
	void Compute_UI(_uint SpellID);
	virtual void Set_SpellType(_int SpellID) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float4 m_vUV{};
	_float2 m_fCenter{};
public:
	static CSpell_Drag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
