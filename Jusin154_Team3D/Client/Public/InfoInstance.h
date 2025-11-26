#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CInfoInstance final : public CBase
{
	DECLARE_SINGLETON(CInfoInstance)
private:
	CInfoInstance();
	virtual ~CInfoInstance() = default;
public:
	HRESULT Initialize_Information(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	void Release_Information();
	LEVEL Get_RestartLevel();
public:
	void Update(_float fTimeDelta);


#pragma region MAP_INFO
	HRESULT Load_MapObjects(const _char* pFilePath);
#pragma endregion


private:
	CGameInstance*				m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CPlayerInfo*			m_pPlayerInfo = { nullptr };
	class CMonsterInfo*			m_pMonsterInfo = { nullptr };
	class CMapInfo*				m_pMapInfo = { nullptr };

#ifdef _DEBUG
	_string m_strLog = {};
#endif // _DEBUG

public:
	virtual void Free() override;
};

NS_END
