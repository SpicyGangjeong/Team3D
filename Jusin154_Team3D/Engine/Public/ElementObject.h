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

public:
	virtual void Visible(_bool bVisible) override;
	virtual _bool Chack_Visible() override;
	virtual void Nine_Slice_Left(_float X) override;
	virtual void Nine_Slice_Right(_float Y) override;
	virtual void Nine_Slice_Top(_float Z) override;
	virtual void Nine_Slice_Bottom(_float W) override;
	virtual _float Get_Nine_Slice_Left() override;
	virtual _float Get_Nine_Slice_Right() override;
	virtual _float Get_Nine_Slice_Top() override;
	virtual _float Get_Nine_Slice_Bottom() override;

	virtual _bool Start_Lerp(_float fTimeDelta) override;
	virtual _bool Start_Lerp_Speed(_float fTimeDelta, _float2 MousePoint) override;
	virtual void Reset_Pos(_float fTimeDelta)override;
	virtual void Start_Size_Lerp(_float fTimeDelta) override;
	virtual void Reset_Size_Lerp(_float fTimeDelta) override;
	virtual _vector Get_Lerp_Pos() override;

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;

	_float m_fBezierTime = 0;

public:
	virtual void Free() override;
};

NS_END
