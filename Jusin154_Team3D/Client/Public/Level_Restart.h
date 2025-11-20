#pragma once

#include "Client_Define.h"
#include "Level.h"
NS_BEGIN(Engine)
NS_END
NS_BEGIN(Client)


class CLevel_Restart final : public CLevel
{
private:
	CLevel_Restart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Restart() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Initialize() override;
private:
	LEVEL m_eNextLevel = {};
public:
	static pair<CLevel*, function<void()>> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END
