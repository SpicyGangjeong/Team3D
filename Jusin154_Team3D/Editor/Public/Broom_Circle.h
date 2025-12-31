#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CBroom_Circle final : public CElementObject
{
private:
	CBroom_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_Circle(const CBroom_Circle& rhs);
	virtual ~CBroom_Circle() = default;

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
	virtual void Start_Size_Lerp(_float fTimeDelta) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_int	m_iCount{};
	_float	m_fFontAlpha{};
	_float	m_fDissolve{};


	_bool	m_bCountDown = { false };
	_bool	m_bTimer = { false };
	_bool	m_bSizeDown = { false };
	_bool	m_bSizeUp = { false };

public:
	static CBroom_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
