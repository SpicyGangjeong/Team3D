#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CMouse_Manager final : public CBase
{
private:
	CMouse_Manager(HWND g_hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMouse_Manager() = default;

public:
	void	Update();
	void    Render();
	_float3 Get_MouseMove() { return m_vMove; }
	POINT	Get_MouseViewPortPos();
	POINT	Get_MouseScreenPos();
	void    Picking();
	HRESULT Ray_WorldToLocal(_fmatrix* InvWorldMatrix);
	_bool   MousePicking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut);
	_bool   MousePicking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut);
	//virtual void Mouse_Render();

	_bool	Toggle_MouseCenter() { return m_bLockMouseToCenter = !m_bLockMouseToCenter; }

private:
	HWND					m_ghWnd = {};
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	POINT					m_ptCurrentMouseCur = {};
	POINT					m_ptOldMouseCur = {};

	_float3					m_vMove = {};

	_uint					m_iWinCX = {};
	_uint					m_iWinCY = {};

	_bool					m_bLockMouseToCenter = { false };

	_float3					m_vRayPos[ENUM_CLASS(RAY::END)] = {};
	_float3					m_vRayDir[ENUM_CLASS(RAY::END)] = {};

	//_float4x4				m_ViewMatrix = {};
	//_float4x4				m_ProjMatrix = {};

	//class CShader*				m_pShader = { nullptr };
	//class CVIBuffer_Rect*		m_pRect = { nullptr };
	//ID3D11ShaderResourceView* m_pMouseSRV = { nullptr };

private:
	HRESULT Initialize();
	POINT	Get_ScreenCenterPos();
	void	Lock_MouseToCenter();
public:
	static CMouse_Manager* Create(HWND _g_hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
NS_END

