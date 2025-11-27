#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
NS_END

NS_BEGIN(Client)

class CPlayerInfo final : public CBase
{
private:
	CPlayerInfo();
	~CPlayerInfo() = default;
public:
	void Update(_float fTimeDelta);
	void Change_Level();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	class CInfoInstance*	m_pInfoInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CTransform* m_pTransform = { nullptr };


private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CPlayerInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);
	virtual void Free() override;
};

NS_END
