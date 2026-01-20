#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpellLearn_Slot final : public CElementObject
{
private:
	CSpellLearn_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_Slot(const CSpellLearn_Slot& rhs);
	virtual ~CSpellLearn_Slot() = default;

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
	void Clear();

private:
	void SizeUp(_float fSizeX);
private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_bool			m_bClear = { false };
	_bool			m_fSizeUp = { false };
public:
	static CSpellLearn_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
