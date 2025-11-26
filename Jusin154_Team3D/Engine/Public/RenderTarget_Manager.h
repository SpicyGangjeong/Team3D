#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
private:
	CRenderTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(const _wstring& strRenderTargetKey, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMultiRenderTargetKey, const _wstring& strRenderTargetKey);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV);
	HRESULT Begin_MRT_Include_BackBuffer(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV);
	HRESULT Begin_MRT_NO_DepthStencil(const _wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RenderTarget(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);
	HRESULT Paste_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture2D);

	HRESULT Accumulate_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTarget_SrcA, const _wstring& wstrRenderTarget_SrcB, const _wstring& wstrRenderTarget_Target, SHADER_PASS_DEFERRED ePass);
	HRESULT Refit_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTargetInput, const _wstring& wstrRenderTargetOutput, SHADER_PASS_DEFERRED ePass);
	HRESULT Finish_RenderTarget(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader, const _wstring& wstrRenderTargetOriginal, const _wstring& wstrRenderTargetBloomed, SHADER_PASS_DEFERRED ePass);

#ifdef _DEBUG
public:
	HRESULT Render_RenderTarget_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void    RenderTarget_Debuger();
#endif // _DEBUG

#ifdef _DEBUG
private:
	_int m_iSizeX = { 200 };
	_int m_iSizeY = { 200 };
#endif // _DEBUG

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	unordered_map<_wstring, class CRenderTarget*>			m_RenderTargets;
	unordered_map<_wstring, list<class CRenderTarget*>>		m_MRTs;

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pOriginalDSV = { nullptr };
private:
	class CRenderTarget* Find_RenderTarget(const _wstring& strRenderTargetKey);
	list<class CRenderTarget*>* Find_MRT(const _wstring& strMultiRenderTargetKey);

public:
	static CRenderTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free()override;
};

NS_END
