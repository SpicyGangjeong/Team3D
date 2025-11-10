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
	virtual void MoveX(_float fX);
	virtual void MoveY(_float fY);
	virtual void SizeUpdate(_float fSizeX, _float fSizeY);
	virtual void SizeUpdate_float(_float3 fSizeXY);
	virtual void Set_TimeMult(_float Mult);
	virtual _float Get_TimeMult();

	virtual void Visible(_bool bVisible);
	virtual _bool Get_Visible();
public:
	virtual _float2 Get_Origin_Position();			// Start Position
	virtual _vector Get_Current_Position();			// Current Position
	virtual _float3 Get_Origin_Size();				// Start Size
	virtual _float3 Get_Current_Size();				// Current Size

protected:
	_float2					m_fOrigin_Posigion{};
	_vector					m_fCurrent_Posigion{};
	_float3					m_fOrigin_Size{};

protected:
	_float4x4				m_ViewMatrix{};							// 직교를 하기 위해서 필요
	_float4x4				m_ProjMatrix{};
	_float					m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};  
	_float					m_fWinSizeX{}, m_fWinSizeY{};

	_bool					m_bHover = { false };					// UI들 위에 마우스가 올라가 있는지 확인

	_float					m_fTime{};								// 캔버스는 아니더라도 패널전체랑 Element들은 필요함
	_float					m_fTimeMult{1.f};						// 애니메이션 속도를 조절하기 위해서 필요
	_float3					m_vScale{};								// UI의 스케일을 조절하기 위해서 필요함

	RECT					m_pRect{};

protected:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Ready_Components(void* pArg) override;
public:
	virtual void Free() override;
};

NS_END