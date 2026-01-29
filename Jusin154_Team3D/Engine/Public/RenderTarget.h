#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;
public:
	ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }
	ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	void Clear();
	void Copy_ResourceTo(ID3D11Texture2D* pTexture2D);
	void Copy_ResourceTo(const CRenderTarget& Target);
	void Copy_ResourceFrom(ID3D11Texture2D* pTexture2D);
	void Copy_ResourceFrom(const CRenderTarget& Target);
	void Get_TextureDesc(D3D11_TEXTURE2D_DESC& Desc);

#ifdef _DEBUG
public:
	_bool Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _float fX, _float fY, _float fSizeX, _float fSizeY);
	void Ready_Debug();
	void Describe_Entity(const _char* pName);
	void Toggle_RT_Debug();
	void Set_Name(const _char* pName, _uint iSize);
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
private:
	ID3D11Texture2D* m_pTexture2D = { nullptr };
	ID3D11RenderTargetView* m_pRTV = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	_float4		m_vClearColor = {};

#ifdef _DEBUG
private:
	_float4x4		m_WorldMatrix = {};
	_point			m_ptWindowSize = {};
	_bool			m_isDraw = { false };
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};

NS_END
