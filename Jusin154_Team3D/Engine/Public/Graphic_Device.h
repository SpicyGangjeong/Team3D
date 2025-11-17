#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CGraphic_Device final
	: public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY,
		_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	HRESULT Clear_BackBuffer_View(const _float4* pClearColor);

	HRESULT Clear_DepthStencil_View();

	HRESULT Present();

	HRESULT Bind_DepthStencil(class CShader* pShader, const _char* pContantName);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	IDXGISwapChain* m_pSwapChain = { nullptr };
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };
	ID3D11ShaderResourceView* m_pDepthStencilSRV = { nullptr };
private:
	HRESULT Ready_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

NS_END
