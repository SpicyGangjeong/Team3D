#pragma once

#include "Client_Define.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_Loading() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual _uint	Get_NextLevelID() override { return ENUM_CLASS(m_eNextLevelID); };
	virtual void Capture_TerrainPreShadow(SHADOW_LIGHT_DESC& ShadowDesc);

private:
	SHADOW_LIGHT_DESC	m_ShadowDesc = {};
	LEVEL				m_eNextLevelID = { LEVEL::END };
	class CLoader*		m_pLoader = { nullptr };

	_float m_fTimeDelta{};
	_bool m_bDelay = { false };
	_bool m_bNextLevel = { false };
	class CGameObject* m_pIntro_Image = { nullptr };
private:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Initialize() override;
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END
