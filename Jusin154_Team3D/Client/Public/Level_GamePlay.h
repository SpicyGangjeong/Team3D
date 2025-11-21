#pragma once

#include "Client_Define.h"
#include "Level.h"
NS_BEGIN(Engine)
NS_END
NS_BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	class CInfoInstance* m_pInfoInstance = { nullptr };

private:
	virtual HRESULT Initialize() override;
	HRESULT Initialize(void* pArg);
	HRESULT Ready_Lights();
	HRESULT Ready_Markers();
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_Sound();
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_SkyBox(const _wstring& strLayerTag);

public:
	static pair<CLevel*, function<void()>> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg);
	virtual void Free() override;
};

NS_END
