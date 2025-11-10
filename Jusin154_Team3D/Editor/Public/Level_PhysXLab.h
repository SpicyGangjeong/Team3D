#pragma once

#include "Editor_Define.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_PhysXLab final : public CLevel
{
private:
	CLevel_PhysXLab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_PhysXLab() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Initialize() override;
	HRESULT Ready_Layer_Light();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_PhysXObjects(const _wstring& strLayerTag);



public:
	static CLevel_PhysXLab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END