#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIComponent final : public CComponent
{
public:
	typedef struct tagUIComponentDESC
	{
		_float fX, fY = {};
		_float fSizeX, fSizeY = {};
		_uint  iRenderOrderASC = {};

	}UI_DESC;
private:
	CUIComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIComponent(const CUIComponent& rhs);
	virtual ~CUIComponent() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

protected:

	_float					m_fX = {}, m_fY = {};
	_float					m_fSizeX = { 100 }, m_fSizeY = { 100 };
	_float					m_fWinCX = {}, m_fWinCY = {}; 
	_uint					m_iRenderOrderASC = {};
	_bool					m_isFollowParents = { true };

protected:
	_float4x4				m_ViewMatrix{};
	_float4x4				m_ProjMatrix{};
public:
	static CUIComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END