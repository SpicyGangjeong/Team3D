#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc
	{
		_float fX, fY = {};
		_float fSizeX, fSizeY = {};
	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& rhs);
	virtual ~CUIObject() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual _vector Get_WorldPostion() override;

public:
	virtual void Move_vector(_fvector vPos);
	virtual void Move_float2(_float fX, _float fY);

protected:
	_float4x4				m_ViewMatrix{};
	_float4x4				m_ProjMatrix{};

	_bool					m_bActive = { false };
	_bool					m_bHover = { false };

	_float					m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float					m_fWinSizeX{}, m_fWinSizeY{};

	_float					m_fTime{};
	_float3					m_vScale{};

	RECT					m_pRect{};

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;
public:
	virtual void Free() override;
};

NS_END