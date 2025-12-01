#include "pch.h"
#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "QuadTree.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
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
#ifdef EDITOR_PROJECT
	, m_HeigthValues{ rhs.m_HeigthValues }
#endif
{
	Safe_AddRef(m_pQuadTree);
}

#ifdef EDITOR_PROJECT
HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _char* pFilePath)
{
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

	m_iNumVerticesX = iWidth + 1;
	m_iNumVerticesZ = iHeight + 1;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_float* pPixels = new _float[m_iNumVertices];
	ZeroMemory(pPixels, sizeof(_float) * m_iNumVertices);

	_uint iPixelIndex = {};
	for (int i = 0; i < iHeight; ++i) {
		for (int j = 0; j < iWidth; ++j) {
			int iTexureindex = iComp * ((iHeight - 1 - i) * iWidth + j);
			iPixelIndex = i * m_iNumVerticesX + j;
			if (iComp < 3) {
			//  흑백
			}
			else {
				// RGB 컬러
				_uint height16 = ((int)pTexture[iTexureindex + 0]<< 8) + (int)pTexture[iTexureindex + 1];        // 0 ~ 65535
				pPixels[iPixelIndex] = height16 / 65535.0f;

				int color_r = (int)pTexture[iTexureindex + 0];
				int color_g = (int)pTexture[iTexureindex + 1];
				int color_b = (int)pTexture[iTexureindex + 2];

				// 알파 채널
				int alpha;
				if (iComp > 3) {
					/*pPixels[iIndex++] =
						(int)pTexture[index + 3];*/
				}
				else {
					alpha = 255;
				}
			}
		}
		iPixelIndex = i * m_iNumVerticesX + iWidth;
		pPixels[iPixelIndex] = 0.0f;
	}

	_uint iLastRowStart = iHeight * m_iNumVerticesX;
	for (_uint j = 0; j < m_iNumVerticesX; ++j) // m_iNumVerticesX == iWidth + 1
	{
		pPixels[iLastRowStart + j] = 0.0f;
	}

	stbi_image_free(pTexture);
	
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

	m_pVertexPositions = new     _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition =	_float3(
				(_float)j * m_iNumVerticesX / (m_iNumVerticesX - 1.f), 
				(_float)(pPixels[iIndex]) * 10.f * 256.f - 256.f,
				(_float)i * m_iNumVerticesZ / (m_iNumVerticesZ -1.f)
			);
			m_HeigthValues.push_back((_float)(pPixels[iIndex]) * 10.f * 256.f - 256.f);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX), (_float)i / m_iNumVerticesZ );
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

	for (_uint i = 0; i < m_iNumVerticesZ - 1 ; i++)
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
#endif
HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _char* pFilePath, _uint iSizeX, _uint iSizeZ)
{
	m_iNumVerticesX = iSizeX + 1;
	m_iNumVerticesZ = iSizeZ + 1;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_float* pPixels = new _float[m_iNumVertices];
	ZeroMemory(pPixels, sizeof(_float) * m_iNumVertices);


#pragma region READ_DATA
	string strFilePath = "../Bin/Resources/Data/Map/" + string(pFilePath);

	ifstream in(strFilePath, ios::binary);

	if (false == in.is_open())
	{
		MSG_BOX("Failed to Load Model Binanry File");
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			in.read(reinterpret_cast<_char*>(&pPixels[iIndex]), sizeof(_float));
		}
	}

	in.close();
#pragma endregion

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

	m_pVertexPositions = new     _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3(
				(_float)j * m_iNumVerticesX / (m_iNumVerticesX - 1.f),
				(_float)(pPixels[iIndex]),
				(_float)i * m_iNumVerticesZ / (m_iNumVerticesZ - 1.f)
			);
			m_HeigthValues.push_back((_float)(pPixels[iIndex]));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX), (_float)i / m_iNumVerticesZ);
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
	if (nullptr == m_pQuadTree){
		return E_FAIL;
	}

	m_pQuadTree->SetUp_Neighbors();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Terrain::ConvertToHeightField(const _tchar* pStaticKey)
{
	vector<PSX::PxHeightFieldSample> pxSamples = {};
	pxSamples.resize(m_iNumVerticesZ * m_iNumVerticesX);

	for (_uint iRow = 0; iRow < m_iNumVerticesZ; ++iRow) {
		for (_uint iCol = 0; iCol < m_iNumVerticesX; ++iCol) {
			_uint iIndex = iRow * m_iNumVerticesX + iCol;
			_uint iPhysXIndex = iCol * m_iNumVerticesZ + iRow;

			_float fRealHeight = m_HeigthValues[iIndex];
			PSX::PxI16 iClampHeight = (PSX::PxI16)(PSX::PxClamp(fRealHeight, -32767.f, 32767.f));

			PSX::PxHeightFieldSample& pxSample = pxSamples[iPhysXIndex];

			pxSample.height = iClampHeight;
			pxSample.materialIndex0 = pxSample.materialIndex1 = 0;
			pxSample.clearTessFlag();
		}
	}

	PSX::PxHeightFieldDesc Desc{};
	Desc.format = PSX::PxHeightFieldFormat::eS16_TM;
	Desc.nbRows = m_iNumVerticesZ;
	Desc.nbColumns = m_iNumVerticesX;

	Desc.samples.data = pxSamples.data();
	Desc.samples.stride = sizeof(PSX::PxHeightFieldSample);
	Desc.convexEdgeThreshold = 0;
	if (false == Desc.isValid()) {
		assert(false);
	}
	else {
		m_pGameInstance->RegistHeight(pStaticKey, Desc);
	}
	
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

	m_pContext->Map(m_pVB, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(SubResource.pData);

	_matrix WorldInv = XMMatrixInverse(nullptr, WorldMatrix);

	m_pGameInstance->Ray_WorldToLocal(&WorldInv);

	m_pQuadTree->Set_Y(m_pGameInstance, m_pVertexPositions, pVertices, WorldMatrix, fY);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&m_pVertexPositions[i], &pVertices[i].vPosition, sizeof(_float3));
	}


	m_pContext->Unmap(m_pVB, 0);
}


HRESULT CVIBuffer_Terrain::Load_HeightMap(const _char* pFilePath)
{
	string strFilePath = "../Bin/Resources/Data/Map/" + string(pFilePath);

	ifstream in(strFilePath, ios::binary);

	if (false == in.is_open())
	{
		MSG_BOX("Failed to Load Model Binanry File");
		return E_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE, 0, &SubResource);

	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			in.read(reinterpret_cast<_char*>(&pVertices[iIndex].vPosition.y), sizeof(_float));
		}
	}

	in.close();

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

#ifdef EDITOR_PROJECT
void CVIBuffer_Terrain::Change_HeigthRatio(_float fRatio)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE, 0, &SubResource);

	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition.y = m_HeigthValues[i] * fRatio;
	}

	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Terrain::Set_CullingRadius(_float fRaduis)
{
	m_pQuadTree->Set_CullingRadius(fRaduis);
}


HRESULT CVIBuffer_Terrain::Save_HeightMap(const _char* pFilePath)
{
	string strFilePath = "../Bin/Resources/Data/Map/" + string(pFilePath);

	ofstream  out(strFilePath, ios::binary);

	if (false == out.is_open())
	{
		MSG_BOX("Failed to Save Binanry File");
		return E_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_READ, 0, &SubResource);

	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			out.write(reinterpret_cast<const _char*>(&pVertices[iIndex].vPosition.y), sizeof(_float));
		}
	}
	m_pContext->Unmap(m_pVB, 0);

	out.close();

	MSG_BOX("Success to Load Binanry File");

	return S_OK;
}
#endif // EDITOR_PROJECT

#ifdef EDITOR_PROJECT
CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif // EDITOR_PROJECT

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
#ifdef _DEBUG

void CVIBuffer_Terrain::Describe_Entity()
{
}
#endif // _DEBUG
