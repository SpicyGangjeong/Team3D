#pragma once

#include "Editor_Define.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_UI final : public CLevel
{
private:
	CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual ~CLevel_UI() = default;

public:
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	/* 이 레벨에서 쓰기위한 객체들을 생성한다. */
	virtual HRESULT Initialize() override;
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Cursor(const _wstring& strLayerTag);
	void ResetLevel_Environment() {};


public:
	static CLevel_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID);
	virtual void Free() override;
};

NS_END
