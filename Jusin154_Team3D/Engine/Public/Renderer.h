#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	void	Refresh_Renderer();
	void	Render();
	HRESULT Add_RenderGroup(RENDER eRenderGroup, class CGameObject* pRenderObject, _float4& vPos, _float fCullRadius);

#ifdef _DEBUG
private:
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
#endif
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDER::END)];

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;
#endif
private:
	class CShader* m_pShader = { nullptr };
	class CShader* m_pLastColorShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

private:
	_float4x4					m_WorldMatrix = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};
	_float4						m_vPlanes[6] = {};
	_float3						m_CubeViewFrustum[8] = {};
	_float3						m_CubeNDC[8] = {
		{ -1.f, -1.f, 0.f }, { 1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f }, { -1.f, 1.f, 0.f },
		{ -1.f, -1.f, 1.f }, { 1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f }, { -1.f, 1.f, 1.f }
	};

	ID3D11DepthStencilView* m_pShadowDSV = { nullptr };

	SHADOW_LIGHT_DESC m_PreShadowDesc = {};
	_float4x4 m_PreShadowMatrices[ENUM_CLASS(D3DTS::END)] = {};


private:
	void Render_Priority();
	void Render_Shadow();
	void Render_NonBlend();
	void Render_LightAcc();
	void Render_Blur(); 
	void Render_Combined();
	void Render_Effect();
	void Render_NonLight();
	void Render_Blend();
	void Render_LastColor();
	void Render_UI();


#ifdef _DEBUG
private:
	void Render_Debug();
#endif

private:
	HRESULT Ready_DepthStencilView(_uint iSizeX, _uint iSizeY);



private:
	HRESULT Initialize();
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
};

NS_END