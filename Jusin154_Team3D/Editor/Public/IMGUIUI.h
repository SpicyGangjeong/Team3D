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
	void wstringTostring(vector<wstring>& panelNames);

private:
	CGameObject*			m_pGamePlay_Canvas	= { nullptr };
	CGameObject*			m_pPanelObject		= { nullptr };
	CGameObject*			m_pElementObject	= { nullptr };
	_float2					m_fPos{};
	_float2					m_fSize{};
	_float3					m_fSizeXY{};
	_float					m_fTimeMult{};

	_int					m_iPanelCount{};
	_int					m_iElementCount{};
	vector<wstring>			m_pNamewstring;
	vector<string>			m_pNamestring;
	vector<const _char*>	m_pName;

public:
	static CIMGUIUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END