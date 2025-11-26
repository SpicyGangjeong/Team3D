#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMapInfo final : public CBase
{
private:
	CMapInfo();
	~CMapInfo() = default;
public:
	void Update(_float fTimeDelta);
	HRESULT Load_MapObjects(const _char* pFilePath);


private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CMapInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
