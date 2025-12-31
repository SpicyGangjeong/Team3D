#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CBroom_Finish final : public CElementObject
{
private:
	CBroom_Finish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_Finish(const CBroom_Finish& rhs);
	virtual ~CBroom_Finish() = default;

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
	void Set_Font();

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_wstring m_fFont[7];
	_float2 m_fFontPos[7];

	_float4 m_fImagePosi{};

public:
	static CBroom_Finish* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
