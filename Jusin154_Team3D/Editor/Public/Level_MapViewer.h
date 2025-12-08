#pragma once

#include "Editor_Define.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_MapViewer final : public CLevel
{
private:
	CLevel_MapViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_MapViewer() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Initialize() override;
	HRESULT Ready_Layer_Light();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Background(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_InstanceProp(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BuildingContainer(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Unified(const _wstring& strLayerTag);
	HRESULT Ready_Layer_MapObjectManager(const _wstring& strLayerTag);



public:
	static CLevel_MapViewer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END
