#include "pch.h"
#include "Texture.h"
#include "Shader.h"

#include "GameInstance.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	:CComponent(rhs)
	, m_SRVs{ rhs.m_SRVs }
{
	for (auto& iter : m_SRVs) {
		SAFE_ADDREF(iter);
	}
}


HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName, _uint iOffset, _uint iCount)
{

	_uint iCnt = (iCount == UINT_MAX) ? (_uint)m_SRVs.size() : iCount;
	return pShader->Bind_SRVs(pConstantName, m_SRVs.data(), iCnt);
}

ID3D11ShaderResourceView* CTexture::Get_SRV(_uint iTextureIndex)
{
	return m_SRVs[iTextureIndex];
}

_uint CTexture::Get_Size()
{
	return (_uint)m_SRVs.size();
}

_bool CTexture::Compare_GroupName(_wstring wstrGroupName)
{
	return m_wstrPrototypeName == wstrGroupName;
}

#ifdef _DEBUG
void CTexture::HoverName()
{

}
#endif

HRESULT CTexture::Initialize_Prototype(TEXTURE_LOAD_TYPE eType, const _tchar* pTextureFilePath, _uint iNumTextures, _wstring wstrPrototypeName)
{
	switch (eType)
	{
	case Engine::TEXTURE_LOAD_TYPE::SINGLE:
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };
		if (FAILED(Load_SRV(CMyTools::ToString(pTextureFilePath).c_str(), &pSRV))) {
			return E_FAIL;
		}
		m_SRVs.push_back(pSRV);
	} break;
	case Engine::TEXTURE_LOAD_TYPE::INCREMENTAL:
		if (FAILED(ParseTextureIncrementalToSRVs(iNumTextures, pTextureFilePath))) {
			return E_FAIL;
		}
		break;
	case Engine::TEXTURE_LOAD_TYPE::PATH:
		if (FAILED(ParseTexturePathToSRVs(pTextureFilePath))) {
			return E_FAIL;
		}
		break;
	default:
		break;
	}

	m_wstrPrototypeName = wstrPrototypeName;

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Load_SRV(const _char* szPath, ID3D11ShaderResourceView** ppSRV)
{
	ID3D11ShaderResourceView* pSRV = m_pGameInstance->Add_Resource(szPath);

	if (nullptr == pSRV)
		return E_FAIL;

	*ppSRV = pSRV;

	return S_OK;
}

HRESULT CTexture::ParseTextureIncrementalToSRVs(_uint iNumTextures, const _tchar* pTextureFilePath)
{
	m_SRVs.reserve(iNumTextures);

	_tchar			szFullPath[MAX_PATH] = {};

	ID3D11ShaderResourceView* pSRV = { nullptr };

	HRESULT			hr = {};

	for (size_t i = 0; i < iNumTextures; i++)
	{
		wsprintf(szFullPath, pTextureFilePath, i);

		hr = Load_SRV(CMyTools::ToString(szFullPath).c_str(), &pSRV);

		if (FAILED(hr)) {
			return E_FAIL;
		}
		m_SRVs.push_back(pSRV);
	}
	return S_OK;
}

HRESULT CTexture::ParseTexturePathToSRVs(const _tchar* pTextureFolderPath)
{
	_wstring wstrFolder = pTextureFolderPath;
	_wstring wstrSearch = wstrFolder + L"\\*";
	vector<_wstring> files;
	{
		WIN32_FIND_DATAW fileData{};
		HANDLE hFindResult = FindFirstFileW(wstrSearch.c_str(), &fileData);
		if (hFindResult == INVALID_HANDLE_VALUE) {
			return E_FAIL;
		}
		files.reserve(128);
		do {
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue; // ..폴더 무시
			}
			_wstring wstrName = fileData.cFileName;
			_wstring wstrFull = wstrFolder + L"\\" + wstrName;

			files.push_back(wstrFull);
		} while (FindNextFileW(hFindResult, &fileData));

		FindClose(hFindResult);
		sort(files.begin(), files.end());
		if (files.empty()) {
			return E_FAIL;
		}
	}

	m_SRVs.reserve(files.size());
	ID3D11ShaderResourceView* pSRV = { nullptr };
	HRESULT hr;
	for (const auto& filePath : files)
	{
		hr = Load_SRV(CMyTools::ToString(filePath).c_str(), &pSRV);

		if (FAILED(hr)) {
			return E_FAIL;
		}
		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}
CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TEXTURE_LOAD_TYPE eType, const _tchar* pTextureFilePath, _uint iNumTextures, _wstring wstrGroupName)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pTextureFilePath, iNumTextures, wstrGroupName)))
	{
		MSG_BOX("Failed to Created : CTexture");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg, CGameObject* pOwner)
{
	CTexture* pInstance = new CTexture(*this);
	pInstance->m_pOwner = pOwner;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();
	for (auto& pSRV : m_SRVs) {
		SAFE_RELEASE(pSRV);
	}
}
#ifdef _DEBUG

void CTexture::Describe_Entity()
{
}

#endif // _DEBUG
