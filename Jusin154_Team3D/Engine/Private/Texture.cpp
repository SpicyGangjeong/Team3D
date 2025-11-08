#include "pch.h"
#include "Texture.h"
#include "Shader.h"

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

HRESULT CTexture::Initialize_Prototype(TEXTURE_LOAD_TYPE eType, const _tchar* pTextureFilePath, const _wstring& wstrTextureKey, _uint iNumTextures)
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

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Load_SRV(const _char* szPath, ID3D11ShaderResourceView** ppSRV)
{
	*ppSRV = nullptr;
	_char		szTextureFileName[MAX_PATH] = {};
	_char		szTextureFilePath[MAX_PATH] = {};
	_char		szDrive[MAX_PATH] = {};
	_char		szDir[MAX_PATH] = {};
	_char		szFileName[MAX_PATH] = {};
	_char		szEXT[MAX_PATH] = {};
	_splitpath_s(szPath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);
	strcpy_s(szTextureFileName, szDrive);
	strcat_s(szTextureFileName, szDir);
	strcat_s(szTextureFileName, szFileName);

	strcat_s(szTextureFilePath, szTextureFileName);
	strcat_s(szTextureFilePath, ".dds");

	HRESULT			hr = {};

	hr = CreateDDSTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, ppSRV); // 일단 dds로 시도하고
	if (FAILED(hr)) {
		memset(szTextureFilePath, 0, sizeof(_char) * MAX_PATH); // 실패하면 원래 확장자로 다시 시도
		strcat_s(szTextureFilePath, szTextureFileName);
		strcat_s(szTextureFilePath, szEXT);

		if (false == strcmp(".tga", szEXT)) {
			assert(false); // trying to Load TGA
			hr = S_OK;
		}
		else {
			hr = CreateWICTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, ppSRV);
		}
	}
	return hr;
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
		hr = CreateDDSTextureFromFile(m_pDevice, filePath.c_str(), nullptr, &pSRV);
		if (FAILED(hr)) {
			hr = CreateWICTextureFromFile(m_pDevice, filePath.c_str(), nullptr, &pSRV);
		}

		if (FAILED(hr)) {
			return hr;
		}

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}
CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TEXTURE_LOAD_TYPE eType, const _tchar* pTextureFilePath, const _wstring& wstrTextureKey, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pTextureFilePath, wstrTextureKey, iNumTextures)))
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

void CTexture::Describe_Entity()
{
}
