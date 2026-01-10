#pragma once

#include "Client_Define.h"
#include "Level.h"

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
	class CBroomRaceManager* m_pBroomRaceManager = { nullptr };

	_bool m_isDay = { true };

	_bool m_bLevel = { false };
	_bool m_bCurrentLevel = { false };

private:
	virtual HRESULT Initialize() override;
	HRESULT Initialize(void* pArg);
	HRESULT Ready_Lights();
	HRESULT Ready_Volumetric();
	HRESULT Ready_Background();
	HRESULT Ready_Land();
	HRESULT Ready_Layer_Hogsmeade();
	HRESULT Ready_Layer_Hogwart();
	HRESULT Ready_IntstanceProp();
	HRESULT Ready_Markers();
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_Sound();
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_RacerAI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Item(const _wstring& strLayerTag);
	HRESULT Ready_Layer_ReparoObject(const _wstring& strLayerTag);
	HRESULT Ready_Layer_SkyBox(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster();
	HRESULT Ready_Layer_Manager(const _wstring& strLayerTag);
	HRESULT Reday_Layer_EffectPool();

public:
	static pair<CLevel*, function<void()>> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg);
	virtual void Free() override;
};

NS_END
