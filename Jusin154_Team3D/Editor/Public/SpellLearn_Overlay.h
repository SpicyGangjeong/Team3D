#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpellLearn_Overlay final : public CElementObject
{
private:
	CSpellLearn_Overlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_Overlay(const CSpellLearn_Overlay& rhs);
	virtual ~CSpellLearn_Overlay() = default;

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

public:
	void Set_SpellLearn(_int Index);
	void Set_Count(_int Index);

private:
	void Clear();
	void Compute_UI(_uint SpellID);
	void Update_Trail(_float fTimeaDalta);
	void SizeUp(_float fSizeX);
private:
	CTexture*		m_pDiffuse_TextureCom = { nullptr };
	CTexture*		m_pDiffuse_TextureCom1 = { nullptr };
	CTexture*		m_pDiffuse_TextureCom2 = { nullptr };
	CTexture*		m_pDiffuse_TextureCom3 = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_int			m_iImageFrameX{};
	_int			m_iImageFrameY{};
	_float			m_fFrame{};
	_float4			m_vUV{};

	_float			m_fSpellLearn{};
	_float			m_fTrail{};
	_float			m_fTargetTrail{};
	_bool			m_bClear = { false };
	_bool			m_fSizeUp = { false };
public:
	static CSpellLearn_Overlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
