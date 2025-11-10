#pragma once

#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPanelObject abstract : public CUIObject
{
protected:
	CPanelObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	virtual HRESULT	Ready_Panel(void* pArg);

public:
	virtual void MoveX(_float fX) override;
	virtual void MoveY(_float fY) override;
	virtual void Visible(_bool bVisible) override;
	virtual class CGameObject* Get_Element(const wstring& Name);
	virtual void Move_Panel(_float fX, _float fY);

protected:
	virtual void Add_Element(wstring Name, class CGameObject* pPanel);

protected:
	vector<class CGameObject*> m_Elements;
	map<wstring, class CGameObject*> m_Elements_map;		// 캔버스에서 내가 원하는 패널을 찾기 위해서

private:
	virtual class CGameObject* Find_Element(const wstring& Name);

public:
	virtual void Free() override;
};

NS_END