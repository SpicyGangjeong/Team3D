#include "pch.h"
#include "AlphaMap.h"
#include "GameInstance.h"
#include "ScreenGrab.h"

CAlphaMap::CAlphaMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBase{}
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ Engine::CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

void CAlphaMap::Update(_float3& vPosition, _uint iColorIndex, _float fValue, _uint iRange)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return;

	_uint iPixelIndex = {};

	_uint iStartX = (_uint)(vPosition.x / 0.25f) - (iRange / 2);
	_uint iStartZ = (_uint)(vPosition.z / 0.25f) - (iRange / 2);

	for (_uint j = 0; j < iRange; ++j)
	{
		for (_uint i = 0; i < iRange; ++i)
		{
			_uint iCurrentX = iStartX + i;
			_uint iCurrentZ = iStartZ + j;

			iPixelIndex = (iCurrentZ * m_iNumPixelW) + iCurrentX;

			if (m_iNumPixels <= iPixelIndex)
				return;

			if (0 == iColorIndex)
			{
				m_pPixels[iPixelIndex].x += fValue;
			}
			else if (1 == iColorIndex)
			{
				m_pPixels[iPixelIndex].y += fValue;
			}
			else if (2 == iColorIndex)
			{
				m_pPixels[iPixelIndex].z += fValue;
			}
			else if (3 == iColorIndex)
			{
				m_pPixels[iPixelIndex].w += fValue;
			}
			else
				return;

			_float fWeight = m_pPixels[iPixelIndex].x + m_pPixels[iPixelIndex].y + m_pPixels[iPixelIndex].z + m_pPixels[iPixelIndex].w;

			m_pPixels[iPixelIndex] = _float4(m_pPixels[iPixelIndex].x / fWeight, m_pPixels[iPixelIndex].y / fWeight, m_pPixels[iPixelIndex].z / fWeight, m_pPixels[iPixelIndex].w / fWeight);
		}
	}

	memcpy(SubResource.pData, m_pPixels, sizeof(_float4) * m_iNumPixels);

	m_pContext->Unmap(m_pTexture2D, 0);

}

void CAlphaMap::Save_ToFile(const _char* pFilePath)
{
	string strFilePath = "../Bin/Resources/Data/Map/" + string(pFilePath);

	ofstream  out(strFilePath, ios::binary);

	if (false == out.is_open())
	{
		MSG_BOX("Failed to Save Binanry File");
		return;
	}
	for (_uint i = 0; i < m_iNumPixels; ++i)
	{
		out.write(reinterpret_cast<const _char*>(&m_pPixels[i]), sizeof(_float4));
	}
	
	out.close();



	MSG_BOX("Success to Save Binanry File");
}

void CAlphaMap::Load_ToFile(const _char* pFilePath)
{
	string strFilePath = "../Bin/Resources/Data/Map/" + string(pFilePath);

	ifstream in(strFilePath, ios::binary);

	if (false == in.is_open())
	{
		MSG_BOX("Failed to Load Model Binanry File");
		return;
	}

	for (_uint i = 0; i < m_iNumPixels; ++i)
	{
		in.read(reinterpret_cast<_char*>(&m_pPixels[i]), sizeof(_float4));
	}


	in.close();

	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return;

	//MSG_BOX("Success to Load Binanry File");

	memcpy(SubResource.pData, m_pPixels, sizeof(_float4) * m_iNumPixels);

	m_pContext->Unmap(m_pTexture2D, 0);

	if (FAILED(SaveDDSTextureToFile(m_pContext, m_pTexture2D, L"../Bin/Resources/Data/Map/Hogwart_AlphaMap.dds")))
		return;
}

HRESULT CAlphaMap::Initialize(_uint iSizeX, _uint iSizeY)
{
	D3D11_TEXTURE2D_DESC	TextureDesc{};

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;


	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_iNumPixels = iSizeX * iSizeY;
	m_pPixels = new _float4[m_iNumPixels];
	ZeroMemory(m_pPixels, sizeof(_float4) * m_iNumPixels);

	for (_uint i = 0; i < m_iNumPixels; ++i)
	{
		m_pPixels[i].x = 1.f;
	}
	
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	memcpy(SubResource.pData, m_pPixels, sizeof(_float4) * m_iNumPixels);

	m_pContext->Unmap(m_pTexture2D, 0);

	m_iNumPixelW = iSizeX;
	m_iNumPixelH = iSizeY;

	

	return S_OK;
}

CAlphaMap* CAlphaMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY)
{
	CAlphaMap* pInstance = new CAlphaMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY)))
	{
		MSG_BOX("Failed to Created : CAlphaMap");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CAlphaMap::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pPixels);

	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pTexture2D);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pGameInstance);
}
