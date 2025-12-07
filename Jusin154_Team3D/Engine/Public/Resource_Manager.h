#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CResource_Manager final : public CBase
{
private:
	CResource_Manager(ID3D11Device* pDevice);
	virtual ~CResource_Manager() = default;
	shared_mutex m_smtxTexture = {};
public:
	ID3D11ShaderResourceView* Add_Texture(const _char* pFilePath);


private:
	HRESULT Initialize(_uint iResourceCount);
	ID3D11ShaderResourceView* Load_SRV(const filesystem::path& pathFile);

private:
#ifdef _DEBUG
	_uint							m_iCount = {};
#endif // DEBUG

	ID3D11Device*					m_pDevice = { nullptr };

	unordered_map<_wstring, ID3D11ShaderResourceView*>	m_Resources;

public:
	static CResource_Manager* Create(ID3D11Device* pDevice, _uint iResourceCount);
	virtual void Free() override;
};

NS_END
