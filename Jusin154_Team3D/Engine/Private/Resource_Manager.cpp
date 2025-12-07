#include "pch.h"
#include "Resource_Manager.h"

CResource_Manager::CResource_Manager(ID3D11Device* pDevice)
	:m_pDevice{ pDevice }
{ 
	SAFE_ADDREF(m_pDevice);
}

ID3D11ShaderResourceView* CResource_Manager::Add_Texture(const _char* pFilePath)
{
	if (pFilePath == nullptr || pFilePath[0] == '\0'){
		return nullptr;
	}
	filesystem::path pathFile = pFilePath;
	const _wstring wstrKeyOriginal = pathFile.filename().wstring();
	
	{ // 브레이스 해제 금지
		shared_lock<shared_mutex> sharedLock(m_smtxTexture);

		auto iter = m_Resources.find(wstrKeyOriginal);
		if (iter != m_Resources.end())
		{
			SAFE_ADDREF(iter->second);
			return iter->second;
		}
		// sharedLock 소멸
	} // 브레이스 해제 금지

	ID3D11ShaderResourceView* pSRV = Load_SRV(pathFile);

	if (pSRV == nullptr){
		return nullptr;
	}

	{ // 브레이스 해제 금지
		unique_lock<shared_mutex> uniqueLock(m_smtxTexture);

		auto iter = m_Resources.find(wstrKeyOriginal);
		if (iter != m_Resources.end())
		{
			SAFE_RELEASE(pSRV); // 여기까지 왔는데 end가 아니다? -> 다른 스레드가 넣음 -> 이 srv는 이미 로드됨. -> 삭제해야 됨

			SAFE_ADDREF(iter->second);
			return iter->second; // sharedLock 소멸
		}

		m_Resources.emplace(wstrKeyOriginal, pSRV);
		// sharedLock 소멸
	} // 브레이스 해제 금지

#ifdef _DEBUG
	++m_iCount;
#endif // _DEBUG
	SAFE_ADDREF(pSRV);
    return pSRV;
}

HRESULT CResource_Manager::Initialize(_uint iResourceCount)
{
	m_Resources.reserve(iResourceCount);

    return S_OK;
}

ID3D11ShaderResourceView* CResource_Manager::Load_SRV(const filesystem::path& pathFile)
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	const filesystem::path pathExtensionOriginal = pathFile.extension();

	filesystem::path pathDDSFile = pathFile;
	pathDDSFile.replace_extension(L".dds");

	HRESULT hr = CreateDDSTextureFromFile(
		m_pDevice,
		pathDDSFile.wstring().c_str(),
		nullptr,
		&pSRV
	);

	if (FAILED(hr)) {
		filesystem::path pathFileOriginal = pathFile;
		pathFileOriginal.replace_extension(pathExtensionOriginal);

		hr = CreateWICTextureFromFile(
			m_pDevice,
			pathFileOriginal.wstring().c_str(),
			nullptr,
			&pSRV
		);

		if (FAILED(hr)){
			return nullptr;
		}
	}
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
