#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CMagic_Meter final : public CElementObject
{
private:
	CMagic_Meter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagic_Meter(const CMagic_Meter& rhs);
	virtual ~CMagic_Meter() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	void	UV();
	_float2 Compute_UVX(_uint Number);
	_float2 Compute_UVY(_uint Numver);


private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_UI_Instance* m_pVIBufferCom = { nullptr };
	CVIBuffer_UI_Instance::UI_ATLAS_DESC	pUVDesc[5];

	_uint m_iCols{};
	_float2 m_fIamge_Size{};
	//_float2 Position[5];
public:
	static CMagic_Meter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
