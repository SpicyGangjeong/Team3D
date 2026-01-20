#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CNPCInteraction final : public CElementObject
{
private:
	CNPCInteraction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCInteraction(const CNPCInteraction& rhs);
	virtual ~CNPCInteraction() = default;

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
	_float4 ComputeUV(_tchar Alphabat);
	void Set_ImageSizePosition();
	void SizeUpX(_float fSizeX);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float4 m_vUV{};
	_float4 m_vImagePosi1{};
	_float4 m_vImagePosi2{};

	_float2 m_fFontSize{};
	
public:
	static CNPCInteraction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
