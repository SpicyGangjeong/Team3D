#pragma once

#include "Client_Define.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Field final : public CLevel
{
private:
	CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Field() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool m_isDay = { true };
private:
	class CInfoInstance* m_pInfoInstance = { nullptr };

	_bool	m_bLevel = { false };
	_bool	m_bCurrentLevel = { false };

private:
	virtual HRESULT Initialize() override;
	HRESULT Initialize(void* pArg);
	HRESULT Ready_Lights();
	HRESULT Ready_Volumetric();
	HRESULT Ready_Camera();
	HRESULT Ready_Layer_SkyBox();
	HRESULT Ready_Background();
	HRESULT Ready_Markers();
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_Sound();
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster();
	HRESULT Reday_Layer_EffectPool();

public:
	static pair<CLevel*, function<void()>> Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg);
	virtual void Free() override;
};

NS_END
