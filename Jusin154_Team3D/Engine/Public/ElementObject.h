#pragma once

#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CElementObject abstract : public CUIObject
{
protected:
	CElementObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CElementObject(const CElementObject& rhs);
	virtual ~CElementObject() = default;

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

public:
	virtual void Visible(_bool bVisible) override;

public:
	virtual void Free() override;
};

NS_END