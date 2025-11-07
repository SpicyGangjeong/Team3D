#pragma once

#include "Client_Define.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Logo() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Initialize() override;
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);


public:
	static pair<CLevel_Logo*, function<void()>> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END