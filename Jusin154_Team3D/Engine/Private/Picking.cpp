#include "pch.h"
#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iSizeX, _uint iSizeY)
{
	m_hWnd = hWnd;

	D3D11_TEXTURE2D_DESC	TextureDesc{};

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;


	TextureDesc.Usage = D3D11_USAGE_STAGING;
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D))){
		return E_FAIL;
	}

	m_iNumPixelW = iSizeX;
	m_iNumPixelH = iSizeY;

	return S_OK;
}

void CPicking::Update()
{
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Picking"));
	m_pGameInstance->Copy_RenderTargetTo(TEXT("Target_Depth"), m_pTexture2D);
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Picking"));
}
_bool CPicking::isPicking(_float3* pOut)
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	if (ptMouse.x < 0 || ptMouse.x >= (LONG)m_iNumPixelW || ptMouse.y < 0 || ptMouse.y >= (LONG)m_iNumPixelH){
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource))) {
		return false;
	}

	BYTE* pRowStart = static_cast<BYTE*>(SubResource.pData) + ptMouse.y * SubResource.RowPitch;
	_float4* pPixelRow = reinterpret_cast<_float4*>(pRowStart);
	_float4 Pixel = pPixelRow[ptMouse.x];

	m_pContext->Unmap(m_pTexture2D, 0);

	if (0.f == Pixel.w){
		return false;
	}

	_float3 vMousePos{};
	vMousePos.x = ptMouse.x / (m_iNumPixelW * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (m_iNumPixelH * -0.5f) + 1.f;
	vMousePos.z = Pixel.x;

	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos),
			m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ_INV)));

	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos),
			m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW_INV)));

	*pOut = vMousePos;
	return true;
}


CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iSizeX, _uint iSizeY)
{
	CPicking* pInstance = new CPicking(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iSizeX, iSizeY)))
	{
		MSG_BOX("Failed to Created : CPicking");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pTexture2D);
	SAFE_RELEASE(m_pGameInstance);
}
