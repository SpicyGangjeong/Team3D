#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)

class CInformation final : public CBase
{
	DECLARE_SINGLETON(CInformation)
private:
	CInformation();
	virtual ~CInformation() = default;
public:
	HRESULT Initialize_Information(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	void Release_Information();


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	class CPlayerInfo* m_pPlayerInfo = { nullptr };
	class CMonsterInfo* m_pMonsterInfo = { nullptr };
	class CNeutralInfo* m_pNeutralInfo = { nullptr };

private:
#ifdef _DEBUG
	_string m_strLog = {};
#endif // _DEBUG

public:
	virtual void Free() override;
};

NS_END
