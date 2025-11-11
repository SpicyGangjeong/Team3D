#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Editor)

class CIMGUIUI final : public CGameObject
{
public:
	CIMGUIUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIMGUIUI(const CIMGUIUI& rhs);
	virtual ~CIMGUIUI() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	void PanelwstringTostring(vector<wstring>& panelNames);
	void ElementwstringTostring(vector<wstring>& panelNames);

private:
	CGameObject*			m_pGamePlay_Canvas	= { nullptr };
	CGameObject*			m_pPanelObject		= { nullptr };
	CGameObject*			m_pElementObject	= { nullptr };
	_float2					m_fPos{};
	_float2					m_fSize{};
	_float3					m_fSizeXY{};
	_float					m_fEndTime{};
	_float					m_fDelayTime{};
	_float					m_fTimeMult{};
	_float					m_fAlpha{};

	_float2					m_fPanelPos{};
	_float2					m_fPanelSize{};
	_float3					m_fPanelSizeXY{};
	_float					m_fPanelTimeMult{};
	_float					m_fPanelAlpha{};
	_float					m_fAlphaTime{};

	_float					m_fCanvasAlpha{};

	_int					m_iElementCount{};
	vector<wstring>			m_pElementNamewstring;
	vector<string>			m_pElementNamestring;
	vector<const _char*>	m_pElementName;
	_bool					m_bElementFadeIn = { false };
	_bool					m_bElementFadeOut = { false };

	_int					m_iPanelCount{};
	vector<wstring>			m_iPanelNamewstring;
	vector<string>			m_iPanelNamestring;
	vector<const _char*>	m_iPanelName;

	_bool					m_bCanvasVisible =	{ false };
	_bool					m_bPanelVisible =	{ false };
	_bool					m_bElementVisible = { false };

public:
	static CIMGUIUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END