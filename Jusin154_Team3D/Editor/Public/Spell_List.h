#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CSpell_List final : public CElementObject
{
private:
	CSpell_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_List(const CSpell_List& rhs);
	virtual ~CSpell_List() = default;

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
	void Color();
	void Hover(_float fTimeDelta);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };

	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };

	_float m_fOffSetX{};
	_float m_fOffSetY{};
	_uint  m_iCols{};

	_int m_iPerSpell_Slot{};
	_float m_fHoverTime{};
public:
	static CSpell_List* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
