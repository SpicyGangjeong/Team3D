#include "pch.h"
#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "QuadTree.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX{ rhs.m_iNumVerticesX }
	, m_iNumVerticesZ{ rhs.m_iNumVerticesZ }
	, m_pQuadTree{ rhs.m_pQuadTree }
{
	Safe_AddRef(m_pQuadTree);
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _char* pFilePath, _uint iSizeX, _uint iSizeZ)
{


	//_ulong			dwByte = {};
	//HANDLE			hFile = CreateFile(CMyTools::ToWstring(pFilePath).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	//BITMAPFILEHEADER			fh{};
	//BITMAPINFOHEADER			ih{};
	//
	//ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	//ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	


	//ReadFile(hFile, pPixels, sizeof(_uint) * ih.biHeight * ih.biWidth, &dwByte, nullptr);

	HANDLE	hFile = CreateFile(TEXT("../Bin/Bina.bin"),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"오브젝트 저장 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	_int iWidth = {};
	_int iHeight = {};
	_int iComp = {};
	unsigned char* pTexture = {};
	pTexture = stbi_load(pFilePath, &iWidth, &iHeight, &iComp, 0);

	if (!pTexture)
	{
		MSG_BOX("Failed to Load HeightMapFile");
		return E_FAIL;
	}

	m_iNumVerticesX = iWidth + 1;//iSizeX;
	m_iNumVerticesZ = iHeight + 1;//iSizeZ;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_float* pPixels = new _float[m_iNumVertices];
	ZeroMemory(pPixels, sizeof(_float) * m_iNumVertices);

	_uint iIndex = {};
	for (int i = 0; i < iHeight; ++i) {
		for (int j = 0; j < iWidth; ++j) {
			int index =
				iComp * (i * iWidth + j);
			if (iComp < 3) {
				// 흑백
				int grey =
					(int)pTexture[index + 0];

				// 알파 채널
				int alpha;
				if (iComp > 1) {
					int alpha =
						(int)pTexture[index + 1];
				}
				else {
					alpha = 255;
				}
			}
			else {
				// RGB 컬러
				_uint height16 = ((int)pTexture[index + 0]<< 8) | (int)pTexture[index + 1];        // 0 ~ 65535
				pPixels[iIndex++] = height16 / 65535.0f;

				int color_r = (int)pTexture[index + 0];
				int color_g = (int)pTexture[index + 1];
				int color_b = (int)pTexture[index + 2];

				// 알파 채널
				int alpha;
				if (iComp > 3) {
					/*pPixels[iIndex++] =
						(int)pTexture[index + 3];*/
				}
				else {
					alpha = 255;
				}

				if (!WriteFile(hFile, &pPixels[iIndex - 1], sizeof(size_t), nullptr, nullptr)) {
					return E_FAIL;
				}
			}
		}
	}

	stbi_image_free(pTexture);




	CloseHandle(hFile);



	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXNORTEX);

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_STAGING;
	VBDesc.BindFlags = 0;//D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	VBDesc.MiscFlags = 0;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, (_float)(pPixels[iIndex]) * 5 * 256.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f) /32.f, (_float)i / (m_iNumVerticesZ - 1.f) / 32.f);
			//pVertices[iIndex].vTexcoord = _float2(1.f - ((float)i / (m_iNumVerticesZ - 1.f)) / 32.f,
				//((float)j / (m_iNumVerticesX - 1.f)) / 32.f);
		}
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	IBDesc.MiscFlags = 0;


	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = {};

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = pIndices;
#pragma endregion

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	Safe_Delete_Array(pPixels);

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX, m_iNumVerticesX * m_iNumVerticesZ - 1, m_iNumVerticesX - 1, 0);
	if (nullptr == m_pQuadTree)
		return E_FAIL;

	m_pQuadTree->SetUp_Neighbors();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Terrain::Picking(_fmatrix WorldMatrix, _float3* pOut)
{
	_matrix WorldInv = XMMatrixInverse(nullptr, WorldMatrix);

	m_pGameInstance->Ray_WorldToLocal(&WorldInv);
	return m_pQuadTree->Picking(m_pGameInstance,m_pVertexPositions, *pOut, WorldMatrix);
}

void CVIBuffer_Terrain::Culling(_fmatrix WorldMatrix)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(XMMatrixInverse(nullptr, WorldMatrix));

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	_uint* pIndices = static_cast<_uint*>(SubResource.pData);

	_uint		iNumIndices = {};

	m_pQuadTree->Culling(m_pGameInstance, m_pVertexPositions, pIndices, &iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

void CVIBuffer_Terrain::FitY(_fmatrix WorldMatrix, _float fY)
{
	m_pGameInstance->Transform_Frustum_ToLocalSpace(XMMatrixInverse(nullptr, WorldMatrix));

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE, 0, &SubResource);

	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(SubResource.pData);

	_matrix WorldInv = XMMatrixInverse(nullptr, WorldMatrix);

	m_pGameInstance->Ray_WorldToLocal(&WorldInv);

	m_pQuadTree->Set_Y(m_pGameInstance, m_pVertexPositions, pVertices, WorldMatrix, fY);

	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath, _uint iSizeX, _uint iSizeZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath, iSizeX, iSizeZ)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg, class CGameObject* pOwner)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CVIBuffer_Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pQuadTree);
}

void CVIBuffer_Terrain::Describe_Entity()
{
}
