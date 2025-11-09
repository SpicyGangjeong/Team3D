#include "pch.h"
#include "Material.h"
#include "GameInstance.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}
#ifdef EDITOR_PROJECT
HRESULT CMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint iNumSRVs = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		m_SRVs[i].reserve(iNumSRVs);

		ID3D11ShaderResourceView* pSRV = { nullptr };

		HRESULT			hr = {};

		for (size_t j = 0; j < iNumSRVs; j++)
		{
			aiString	strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), (_uint)j, &strTexturePath))) {
				continue;
			}
			_char szTextureDir[MAX_PATH] = {};
			_char szSaveDir[MAX_PATH] = {};
			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};
			_char szName[MAX_PATH] = {};
			_char szExt[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, szName, MAX_PATH, szExt, MAX_PATH);
			strcpy_s(szTextureDir, sizeof(_char) * MAX_PATH, szDrive);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szDir);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szName);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szExt);

			{
				_char		szTextureFileName[MAX_PATH] = {};
				_char		szTextureFilePath[MAX_PATH] = {};
				_char		szDrive[MAX_PATH] = {};
				_char		szDir[MAX_PATH] = {};
				_char		szFileName[MAX_PATH] = {};
				_char		szEXT[MAX_PATH] = {};
				_splitpath_s(szTextureDir, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);
				strcpy_s(szTextureFileName, szDrive);
				strcat_s(szTextureFileName, szDir);
				strcat_s(szTextureFileName, szFileName);

				strcat_s(szTextureFilePath, szTextureFileName);
				strcat_s(szTextureFilePath, ".dds");

				hr = CreateDDSTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV); // 일단 dds로 시도하고
				if (FAILED(hr)) {
					memset(szTextureFilePath, 0, sizeof(_char) * MAX_PATH); // 실패하면 원래 확장자로 다시 시도
					strcat_s(szTextureFilePath, szTextureFileName);
					strcat_s(szTextureFilePath, szEXT);

					if (false == strcmp(".tga", szEXT)) {
						assert(false); // trying to Load TGA
						hr = S_OK;
					}
					else {
						hr = CreateWICTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV);
					}
				}
				m_SaveMaterial.Path[i].push_back(szTextureFilePath);
			}

			strcpy_s(szSaveDir, sizeof(_char) * MAX_PATH, szName);
			strcat_s(szSaveDir, sizeof(_char) * MAX_PATH, szExt);

			if (FAILED(hr)) {
				return E_FAIL;
			}
			m_strPath[i].push_back(szSaveDir);
			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}
HRESULT CMaterial::SaveAsBinary(HANDLE hFile, DWORD& dwByte)
{
	// 실제 값을 저장하지 않고 파일이름.확장자명의 문자열을 저장해서 수정하기 편하게 만들기
	// 대신 리소스를 공유해야함
	// 텍스쳐를 저장하지 않고
	// 텍스쳐 경로만 ( "이름.확장자" ) 만 저장함
	for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
		_uint iNumSrv = (_uint)m_strPath[i].size();
		WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr); // 종류 저장
		WriteFile(hFile, &iNumSrv, sizeof(_uint), &dwByte, nullptr); // 종류의 갯수 저장
		for (_uint j = 0; j < iNumSrv; ++j) {
			m_strPath[i][j].shrink_to_fit();
			_uint strLength = (_uint)(sizeof(_char) * m_strPath[i][j].length());
			WriteFile(hFile, &strLength, sizeof(_uint), &dwByte, nullptr); // 사이즈 저장
			WriteFile(hFile, m_strPath[i][j].data(), strLength, &dwByte, nullptr); // 실제 값 저장
		}
	}
	return S_OK;
}
CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
#endif

HRESULT CMaterial::Bind_SRV(CShader* pShader, const _char* pConstantName, _uint iType, _uint iTextureIndex)
{
	if (m_SRVs[iType].empty()) {
		//assert(false);
		return S_OK;
	}


	return pShader->Bind_SRV(pConstantName, m_SRVs[iType][iTextureIndex]);
}


HRESULT CMaterial::Initialize(const _char* pModelFilePath, HANDLE hFile, DWORD& dwByte)
{
	for (int i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
		_uint iNumSrv = { 0 };
		if (!ReadFile(hFile, &i, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &iNumSrv, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (iNumSrv == 0) {
			continue;
		}
		m_SRVs[i].reserve(iNumSrv);

		ID3D11ShaderResourceView* pSRV = { nullptr };
		HRESULT hr = {};

		for (_uint j = 0; j < iNumSrv; ++j) {
			_uint strLength = { 0 };
			if (!ReadFile(hFile, &strLength, sizeof(_uint), &dwByte, nullptr)) {
				return E_FAIL;
			}
			_char szTextureName[MAX_PATH] = {};
			if (!ReadFile(hFile, szTextureName, strLength, &dwByte, nullptr)) {
				return E_FAIL;
			}

			_char szTextureDir[MAX_PATH] = {};
			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			strcpy_s(szTextureDir, sizeof(_char) * MAX_PATH, szDrive);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szDir);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szTextureName);


			{
				_char		szTextureFileName[MAX_PATH] = {};
				_char		szTextureFilePath[MAX_PATH] = {};
				_char		szDrive[MAX_PATH] = {};
				_char		szDir[MAX_PATH] = {};
				_char		szFileName[MAX_PATH] = {};
				_char		szEXT[MAX_PATH] = {};
				_splitpath_s(szTextureDir, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);
				strcpy_s(szTextureFileName, szDrive);
				strcat_s(szTextureFileName, szDir);
				strcat_s(szTextureFileName, szFileName);

				strcat_s(szTextureFilePath, szTextureFileName);
				strcat_s(szTextureFilePath, ".dds");

				hr = CreateDDSTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV); // 일단 dds로 시도하고
				if (FAILED(hr)) {
					memset(szTextureFilePath, 0, sizeof(_char) * MAX_PATH); // 실패하면 원래 확장자로 다시 시도
					strcat_s(szTextureFilePath, szTextureFileName);
					strcat_s(szTextureFilePath, szEXT);

					if (false == strcmp(".tga", szEXT)) {
						assert(false); // trying to Load TGA
						hr = S_OK;
					}
					else {
						hr = CreateWICTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV);
					}
				}

			}
			if (FAILED(hr)) {
				return E_FAIL;
			}

			m_SRVs[i].push_back(pSRV);
		}
	}
	return S_OK;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const SaveMaterial& _SaveMaterial)
{
	for (size_t type = 0; type < 27; type++)
	{
		for (const auto& path : _SaveMaterial.Path[type])
		{
			char szDrive[MAX_PATH] = {};
			char szDir[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			char szFullPath[MAX_PATH] = {};
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, path.c_str());

			_tchar szPerfectPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, szPerfectPath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = nullptr;
			HRESULT hr = S_OK;

			const char* ext = strrchr(szFullPath, '.');
			if (ext)
			{
				if (strcmp(ext, ".dds") == 0)
					hr = CreateDDSTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV);
				else if (strcmp(ext, ".tga") == 0)
					hr = S_OK;
				else
					hr = CreateWICTextureFromFile(m_pDevice, szPerfectPath, nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_SRVs[type].push_back(pSRV);
		}
	}
	return S_OK;
}


CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, HANDLE hFile, DWORD& dwByte) {
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, hFile, dwByte)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const SaveMaterial& _SaveMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, _SaveMaterial)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMaterial::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs) {
			SAFE_RELEASE(pSRV);
		}
		SRVs.clear();
	}
}
