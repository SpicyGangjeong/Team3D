#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CCurrent_Spell_Slot final : public CElementObject
{
private:
	CCurrent_Spell_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCurrent_Spell_Slot(const CCurrent_Spell_Slot& rhs);
	virtual ~CCurrent_Spell_Slot() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

	virtual void SizeUpX(_float fSizeX) override;
	virtual void SizeUpY(_float fSizeY) override;
	virtual void SizeUpdate(_float fSizeX, _float fSizeY) override;

private:
	void Hover();
	void Click_Slot(_bool bClick);
	void Get_Skill(_int Index);
	void Clear(_int iValue);

	_float4 UV(_int SpellID);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture*				m_pDiffuse_TextureCom = { nullptr };
	CTexture*				m_pDiffuse_TextureCom1 = { nullptr };
	CTexture*				m_pDiffuse_TextureCom2 = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance*	m_pVIBufferCom = { nullptr };

	_float	m_fOffSetX{};
	_float	m_fOffSetY{};
	_uint	m_iCols{};

	_bool	m_bGetSpell = { false };
	_int	m_iSlot_Index{};
	_int	m_iSpell[4][4]{};
	CVIBuffer_UI_Instance::UI_ATLAS_DESC  m_vUV{};

public:
	static CCurrent_Spell_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
