#pragma once

#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPanelObject abstract : public CUIObject
{
protected:
	CPanelObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPanelObject(const CPanelObject& rhs);
	virtual ~CPanelObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual _vector Get_WorldPostion() override;

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT	Ready_Element(void* pArg);

public:
	virtual void Visible(_bool bVisible) override;
	virtual class CGameObject* Get_Element(const wstring& Name);
	virtual _int Element_Count();
	virtual const vector<wstring> Element_Name();
	virtual void ElementAllVisible(_bool bVisible);

	virtual void Add_Function(wstring Name, function<void(void*)> Evnet) override;
	virtual void Function_Callback(wstring Name, void* pArg = nullptr) override;
protected:
	virtual void Add_Element(wstring Name, class CGameObject* pElement);
	virtual _bool Chack_Visible() override;

protected:
	vector<class CGameObject*>			m_Elements;				// 패널이 엘리먼츠를 관리하기 위해 만듬
	vector<wstring>						m_ElementName;			// 엘리멘트들의 이름을 전해주기 위해서
	map<wstring, class CGameObject*>	m_Elements_map;			// 캔버스에서 내가 원하는 패널을 찾기 위해서

	multimap<wstring, function<void(void*)>>		m_ElementFunction_map;

	_int								m_iElements_Count{};	// 전체 엘리멘트의 갯수

private:
	virtual class CGameObject* Find_Element(const wstring& Name);

public:
	virtual void Free() override;
};

NS_END
