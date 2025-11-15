#pragma once

#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCanvasObject abstract : public CUIObject
{
protected:
	CCanvasObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvasObject(const CCanvasObject& rhs);
	virtual ~CCanvasObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Late_Update(_float fTimeDelta);
	virtual _vector Get_WorldPostion() override;

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT	Ready_Panel(void* pArg);

public:
	virtual void Visible(_bool bVisible) override;
	virtual class CGameObject* Get_Panel(const wstring& Name);
	virtual _int Panels_Count();
	virtual const vector<wstring> Panel_Name();

protected:
	virtual void Add_Panel(wstring Name, class CGameObject* pPanel);

protected:
	vector<class CGameObject*>			m_Panels;			// 캔버스가 패널들을 관리 하기 위해서 필요함
	vector<wstring>						m_PanelNames;		// 패널들의 이름을 전해주기 위해서 
	map<wstring ,class CGameObject*>	m_Panels_map;		// 캔버스에서 내가 원하는 패널을 찾기 위해서
	_int								m_iPanel_Count{};	// 전체 패널의 갯수

private:
	virtual class CGameObject* Find_Panel(const wstring& Name);

public:
	virtual void Free() override;
};

NS_END