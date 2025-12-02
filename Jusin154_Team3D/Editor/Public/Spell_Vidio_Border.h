#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpell_Vidio_Border final : public CElementObject
{
private:
	CSpell_Vidio_Border(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_Vidio_Border(const CSpell_Vidio_Border& rhs);
	virtual ~CSpell_Vidio_Border() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

//public:
//	virtual const SPELLINFO Get_Info(_int Index) override;
//
//public:
//	virtual void Set_SkillType(_int eType);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDesendo_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float	m_fStertTimer{};
	_int	m_iPerIndex{};
	_bool	m_bStart = { false };

public:
	static CSpell_Vidio_Border* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
