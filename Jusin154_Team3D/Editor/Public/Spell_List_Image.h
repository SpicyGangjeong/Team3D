#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CSpell_List_Image final : public CElementObject
{
private:
	CSpell_List_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_List_Image(const CSpell_List_Image& rhs);
	virtual ~CSpell_List_Image() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

public:
	virtual void SizeUpX(_float fSizeX) override;
	virtual void SizeUpY(_float fSizeY) override;
	virtual void SizeUpdate(_float fSizeX, _float fSizeY) override;

	void	UV();
	_float4 Meter_Index(_uint Number);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };
	CVIBuffer_UI_Instance::UI_ATLAS_DESC	pUVDesc[26];

	_float m_fOffSetX{};
	_float m_fOffSetY{};
	_uint  m_iCols{};

public:
	static CSpell_List_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
