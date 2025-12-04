#include "pch.h"
#include "Resource_Manager.h"

CResource_Manager::CResource_Manager(ID3D11Device* pDevice)
	:m_pDevice{ pDevice }
{ 
	SAFE_ADDREF(pDevice);
}

ID3D11ShaderResourceView* CResource_Manager::Add_Texture(const _char* pFilePath)
{
	lock_guard<mutex> lock(m_mtxTexture);

    ID3D11ShaderResourceView* pSRV = Find_SRV(pFilePath);

	if (nullptr == pSRV)
	{
		pSRV = Load_SRV(pFilePath);

		if (nullptr == pSRV)
		{
			return nullptr;
		}
	}

	SAFE_ADDREF(pSRV);

    return pSRV;
}

HRESULT CResource_Manager::Initialize(_uint iResourceCount)
{
	m_Resources.reserve(iResourceCount);

    return S_OK;
}

ID3D11ShaderResourceView* CResource_Manager::Find_SRV(const _char* pFilePath)
{
	_char		szFileName[MAX_PATH] = {};
	_splitpath_s(pFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

    auto iter = m_Resources.find(CMyTools::ToWstring(szFileName));

    if (iter == m_Resources.end())
        return nullptr;

    return iter->second;
}

ID3D11ShaderResourceView* CResource_Manager::Load_SRV(const _char* pFilePath)
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	_char		szTextureFileName[MAX_PATH] = {};
	_char		szTextureFilePath[MAX_PATH] = {};
	_char		szDrive[MAX_PATH] = {};
	_char		szDir[MAX_PATH] = {};
	_char		szFileName[MAX_PATH] = {};
	_char		szEXT[MAX_PATH] = {};
	_splitpath_s(pFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);
	strcpy_s(szTextureFileName, szDrive);
	strcat_s(szTextureFileName, szDir);
	strcat_s(szTextureFileName, szFileName);

	strcat_s(szTextureFilePath, szTextureFileName);
	strcat_s(szTextureFilePath, ".dds");

	HRESULT			hr = {};

	hr = CreateDDSTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV, 1024); // 일단 dds로 시도하고
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

	if (E_FAIL == hr)
		return nullptr;

	m_Resources.emplace(CMyTools::ToWstring(szFileName), pSRV);
	++m_iCount;

	return pSRV;
}

CResource_Manager* CResource_Manager::Create(ID3D11Device* pDevice, _uint iResourceCount)
{
    CResource_Manager* pInstance = new CResource_Manager(pDevice);

    if (FAILED(pInstance->Initialize(iResourceCount)))
    {
        MSG_BOX("Failed to Create CResource_Manager");
        return nullptr;
    }

    return pInstance;
}

void CResource_Manager::Free()
{
    __super::Free();

	SAFE_RELEASE(m_pDevice);

	unordered_map<_wstring, ID3D11ShaderResourceView*>::iterator iter = m_Resources.begin();

	for (; iter != m_Resources.end();)
	{
		SAFE_RELEASE(iter->second);
		iter = m_Resources.erase(iter);
	}
	m_Resources.clear();
}
