#include "pch.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"

void CRenderer::Render()
{
	_uint iNumViewPort = { 1 };
	D3D11_VIEWPORT vp = {};
	m_pContext->RSGetViewports(&iNumViewPort, &vp);
	_float2 vResolution = { vp.Width, vp.Height };

	if (FAILED(m_pShader->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2)))) {
		assert(false);
	}

	Render_Priority();
	Render_Shadow();
	Render_NonBlend();
	Render_SSAO();
	Render_SSAO_BLUR();
	Render_LightAcc();
	Render_Blur();
	Render_Combined();
	Render_Occlusion();
	Render_EnvironmentPostProcess();
	Render_Fog();
	Render_Effect();
	Render_NonLight();
	Render_Blend();
	Render_WeightBlend();
	Render_Bloom();
	Render_LastColor();
	Render_Tone_Mapping();
	Render_UI();
	Render_UI_Overley();

#ifdef _DEBUG
	static _bool m_bToggleDebug = false;
	Describe_Entitiy();
	m_pGameInstance->RenderTarget_Debuger();
	GUI::Begin("RenderTarget Debuger", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::Checkbox("DebugToggle", &m_bToggleDebug);

	if (m_pGameInstance->Key_Pressing(DIK_F10)) {
		Render_Debug();
	}
	else {
		if (m_bToggleDebug) {
			Render_Debug();
		}
	}
	GUI::End();
	
#endif
}

void CRenderer::Render_PreShadow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PreShadow"), m_pPreShadowDSV))) {
		return;
	}

	const _float4x4* pMatrices = m_pGameInstance->Get_ShadowMatricesPtr(0);
	m_PreShadowFar = m_pGameInstance->Get_ShadowBoxFar(0);
	for (int i = 0; i < ENUM_CLASS(D3DTS::END); ++i) {
		m_PreShadowMatrices[i] = pMatrices[i];
	}

	D3D11_VIEWPORT			ViewPortOldDesc;
	D3D11_VIEWPORT			ViewPortDesc;
	_uint					iNumViewOldPort = { 1 };
	ZeroMemory(&ViewPortOldDesc, sizeof(D3D11_VIEWPORT));
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	{
		ViewPortDesc.TopLeftX = 0;
		ViewPortDesc.TopLeftY = 0;
		ViewPortDesc.Width		= (_float)g_iMaxShadowWidth;
		ViewPortDesc.Height		= (_float)g_iMaxShadowHeight;
		ViewPortDesc.MinDepth = 0.f;
		ViewPortDesc.MaxDepth = 1.f;
	}
	m_pContext->RSGetViewports(&iNumViewOldPort, &ViewPortOldDesc);
	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRESHADOW)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render_Shadow())) {
				assert(false);
			}
		}
		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::PRESHADOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	m_pContext->RSSetViewports(iNumViewOldPort, &ViewPortOldDesc);
}

void CRenderer::Render_Occlusion()
{
	COMPUTE_TIMEDELTA("Timer_Render_Occlusion");
	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_Combined")))) {
		return;
	}

	m_RenderObjects[ENUM_CLASS(RENDER::OCCLUSION)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return pSour->Get_Depth() < pDest->Get_Depth();
		});

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::OCCLUSION)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render_BoundingBox())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::OCCLUSION)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_Occlusion");
}

void CRenderer::Render_Priority()
{
	COMPUTE_TIMEDELTA("Timer_Render_Priority");
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_Priority");
}

void CRenderer::Render_Shadow()
{
	COMPUTE_TIMEDELTA("Timer_Render_Shadow");
	D3D11_VIEWPORT			ViewPortOldDesc;
	_uint					iNumViewOldPort = { 1 };
	ZeroMemory(&ViewPortOldDesc, sizeof(D3D11_VIEWPORT));
	m_pContext->RSGetViewports(&iNumViewOldPort, &ViewPortOldDesc);

	if (FAILED(m_pGameInstance->Bind_CascadeSplitRatio(m_pShader, "g_fCascadeSplitRatioNear", true))) {
		assert(false);
	}
	if (FAILED(m_pGameInstance->Bind_CascadeSplitRatio(m_pShader, "g_fCascadeSplitRatioFar", false))) {
		assert(false);
	}


	{ // MRT_Shadow_Near
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Near"), m_pShadowDSV_NEAR))) {
			return;
		}

		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		{
			ViewPortDesc.TopLeftX = 0;
			ViewPortDesc.TopLeftY = 0;
			ViewPortDesc.Width = (_float)(g_iMaxShadowWidth);
			ViewPortDesc.Height = (_float)(g_iMaxShadowHeight);
			ViewPortDesc.MinDepth = 0.f;
			ViewPortDesc.MaxDepth = 1.f;
		}
		m_pContext->RSSetViewports(1, &ViewPortDesc);

		for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_NEAR)]) {
			if (nullptr != pRenderObject) {
				if (FAILED(pRenderObject->Render_Shadow())) {
					assert(false);
				}
			}

			SAFE_RELEASE(pRenderObject);
		}

		m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_NEAR)].clear();

		if (FAILED(m_pGameInstance->End_MRT())) {
			return;
		}
	}
	{ // MRT_Shadow_Middle
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Middle"), m_pShadowDSV_MIDDLE))) {
			return;
		}

		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		{
			ViewPortDesc.TopLeftX = 0;
			ViewPortDesc.TopLeftY = 0;
			ViewPortDesc.Width = (_float)(g_iMaxShadowWidth);
			ViewPortDesc.Height = (_float)(g_iMaxShadowHeight);
			ViewPortDesc.MinDepth = 0.f;
			ViewPortDesc.MaxDepth = 1.f;
		}
		m_pContext->RSSetViewports(1, &ViewPortDesc);

		for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_MIDDLE)]) {
			if (nullptr != pRenderObject) {
				if (FAILED(pRenderObject->Render_Shadow())) {
					assert(false);
				}
			}

			SAFE_RELEASE(pRenderObject);
		}

		m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_MIDDLE)].clear();

		if (FAILED(m_pGameInstance->End_MRT())) {
			return;
		}
	}
	{ // MRT_Shadow_Far
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Far"), m_pShadowDSV_FAR))) {
			return;
		}

		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		{
			ViewPortDesc.TopLeftX = 0;
			ViewPortDesc.TopLeftY = 0;
			ViewPortDesc.Width = (_float)(g_iMaxShadowWidth);
			ViewPortDesc.Height = (_float)(g_iMaxShadowHeight);
			ViewPortDesc.MinDepth = 0.f;
			ViewPortDesc.MaxDepth = 1.f;
		}
		m_pContext->RSSetViewports(1, &ViewPortDesc);

		for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_FAR)]) {
			if (nullptr != pRenderObject) {
				if (FAILED(pRenderObject->Render_Shadow())) {
					assert(false);
				}
			}

			SAFE_RELEASE(pRenderObject);
		}

		m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_FAR)].clear();

		if (FAILED(m_pGameInstance->End_MRT())) {
			return;
		}
	}
	m_pContext->RSSetViewports(iNumViewOldPort, &ViewPortOldDesc);


	COMPUTE_TIMEDELTA("Timer_Render_Shadow");
}

void CRenderer::Render_NonBlend()
{
	COMPUTE_TIMEDELTA("Timer_Render_NonBlend");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects")))) {
		return;
	}

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONBLEND)].clear();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_NonBlend");
}

void CRenderer::Render_LightAcc()
{
	COMPUTE_TIMEDELTA("Timer_Render_LightAcc");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc")))) {
		return;
	}

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
	m_pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));
	m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
	m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Surface"), m_pShader, "g_SurfaceTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_SSAO_BLUR"), m_pShader, "g_SSAOInputTexture"))) {
		return;
	}

	m_pVIBuffer->Bind_Resources();

	if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelID(), m_pShader, m_pVIBuffer))) {
		return;
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_LightAcc");
}

void CRenderer::Render_Combined()
{
	COMPUTE_TIMEDELTA("Timer_Render_Combined");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combined")))) {
		return;
	}
	{
		// Bind_Resorces

		m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
		m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
		m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
		m_pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
		m_pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));

		if (FAILED(m_pShader->Bind_RawValue("g_iMaxShadowWidth", &g_iMaxShadowWidth, sizeof(_uint)))) {
			return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_iMaxShadowHeight", &g_iMaxShadowHeight, sizeof(_uint)))) {
			return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
			return;
		}
		_float fShadowFar = m_pGameInstance->Get_ShadowBoxFar(0);
		if (FAILED(m_pShader->Bind_RawValue("g_fShadowFar_NEAR", &fShadowFar, sizeof(_float)))) {
			return;
		}
		fShadowFar = m_pGameInstance->Get_ShadowBoxFar(1);
		if (FAILED(m_pShader->Bind_RawValue("g_fShadowFar_MIDDDLE", &fShadowFar, sizeof(_float)))) {
			return;
		}
		fShadowFar = m_pGameInstance->Get_ShadowBoxFar(2);
		if (FAILED(m_pShader->Bind_RawValue("g_fShadowFar_FAR", &fShadowFar, sizeof(_float)))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix_NEAR", D3DTS::VIEW, 0))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix_NEAR", D3DTS::PROJ, 0))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix_MIDDLE", D3DTS::VIEW, 1))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix_MIDDLE", D3DTS::PROJ, 1))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix_FAR", D3DTS::VIEW, 2))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix_FAR", D3DTS::PROJ, 2))) {
			return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fPreShadowFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
			return;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_PreShadowLightViewMatrix", &m_PreShadowMatrices[ENUM_CLASS(D3DTS::VIEW)]))) {
			return;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_PreShadowLightProjMatrix", &m_PreShadowMatrices[ENUM_CLASS(D3DTS::PROJ)]))) {
			return;
		}
	}

	{
		// Bind_Targets

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow_Near"), m_pShader, "g_ShadowNearTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow_Middle"), m_pShader, "g_ShadowMiddleTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow_Far"), m_pShader, "g_ShadowFarTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_PreShadow"), m_pShader, "g_PreShadowTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurXTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X_Weight"), m_pShader, "g_BlurWeightXTexture"))) {
			return;
		}

	}

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_Combined");
}

void CRenderer::Render_EnvironmentPostProcess()
{
	COMPUTE_TIMEDELTA("Timer_Render_EnvironmentPostProcess");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ENV_Blur_X")))) {
		return;
	}

	m_pShader->Bind_Matrix("g_WorldMatrix",				&m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix",				&m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix",				&m_ProjMatrix);
	m_pShader->Bind_Matrix("g_invMatView",				m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
	m_pShader->Bind_Matrix("g_invmatProj",				m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));
	m_pShader->Bind_RawValue("g_fFar",					m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float));
	m_pShader->Bind_RawValue("g_fDepthThreshold",		&m_fDOF_ENV_CutThreshold, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFocusDistance",		&m_fDOF_ENV_FocusDistance, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFarBlurStart",			&m_fDOF_ENV_StartDistance, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFarBlurEnd",			&m_fDOF_ENV_MaxEnd, sizeof(_float));
	m_pShader->Bind_RawValue("g_fDOFBlurMultiplier",	&m_fDOF_ENV_AmountRadius, sizeof(_float));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
		return;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_AfterCombined"), m_pShader, "g_BlurTexture"))) {
		return;
	}
	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::BLURX_ENVIRONMENT));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_Fog")))) {
		return;
	}

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	m_pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
	m_pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));
	m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float));
	m_pShader->Bind_RawValue("g_fDepthThreshold", &m_fDOF_ENV_CutThreshold, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFocusDistance", &m_fDOF_ENV_FocusDistance, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFarBlurStart", &m_fDOF_ENV_StartDistance, sizeof(_float));
	m_pShader->Bind_RawValue("g_fFarBlurEnd", &m_fDOF_ENV_MaxEnd, sizeof(_float));
	m_pShader->Bind_RawValue("g_fDOFBlurMultiplier", &m_fDOF_ENV_AmountRadius, sizeof(_float));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ENV_Blur_X"), m_pShader, "g_BlurTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_AfterCombined"), m_pShader, "g_DiffuseTexture"))) {
		return;
	}
	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::POSTCOMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_EnvironmentPostProcess");
}

void CRenderer::Render_Fog()
{
	COMPUTE_TIMEDELTA("Timer_Render_Fog");
	m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float));

	m_pGameInstance->Bind_FogValue(m_pShader);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Fog"), m_pShader, "g_OriginalTexture"))) {
		return;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
		return;
	}

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::FOG));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	COMPUTE_TIMEDELTA("Timer_Render_Fog");
}

void CRenderer::Render_Effect()
{
	COMPUTE_TIMEDELTA("Timer_Render_Effect");
	if (FAILED(m_pGameInstance->Begin_MRT_NO_DepthStencil(TEXT("MRT_WB")))) {
		return;
	}



	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::EFFECT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::EFFECT)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_Effect");
}

void CRenderer::Render_WeightBlend()
{
	COMPUTE_TIMEDELTA("Timer_Render_WeightBlend");
	// Bind_Resorces

	m_pWeightBlendShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pWeightBlendShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pWeightBlendShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);


	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_WB_Color"), m_pWeightBlendShader, "g_MixedDiffuseTexture"))) {
		return;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_WB_Revealage"), m_pWeightBlendShader, "g_RevealageTexture"))) {
		return;
	}

	m_pWeightBlendShader->Begin(0);

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	COMPUTE_TIMEDELTA("Timer_Render_WeightBlend");
}

void CRenderer::Render_NonLight()
{
	COMPUTE_TIMEDELTA("Timer_Render_NonLight");
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_NonLight");
}

void CRenderer::Render_Blur()
{
	COMPUTE_TIMEDELTA("Timer_Render_Blur");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur")))) {
		return;
	}

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLUR)])
	{
		if (nullptr != pRenderObject)
			if (FAILED(pRenderObject->Render_Blur())) {
				assert(false);
			}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLUR)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X")))) {
		return;
	}

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur"), m_pShader, "g_BlurTexture"))) {
		return;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Weight"), m_pShader, "g_BlurWeightTexture"))) {
		return;
	}

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::BLUR));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_Blur");
}
void CRenderer::Render_SSAO()
{
	COMPUTE_TIMEDELTA("Timer_Render_SSAO");
	_uint iKernelSize = SSAO_SAMPLE_NUMBER;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_OCCLUSION")))) {
		return;
	}
	{
		_uint iNumViewPort = { 1 };
		D3D11_VIEWPORT vp = {};
		m_pContext->RSGetViewports(&iNumViewPort, &vp);
		_float2 vResolution = { vp.Width, vp.Height };
		if (FAILED(m_pShader->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2)))) {
			assert(false);
		}

		// Bind_Resorces
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV) ))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV)))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_iKernelSize", &iKernelSize, sizeof(_uint)))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fSSAORadius", &m_fSSAO_Radius, sizeof(_float)))){
			assert(false);
			return ;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
			assert(false);
			return;
		}

		if (FAILED(m_pShader->Bind_RawValue("g_fSSAO_BIAS", &m_fSSAO_BIAS, sizeof(_float)))) {
			assert(false);
			return;
		}
	}
	{
		// Bind_Targets
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
			assert(false);
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture"))) {
			assert(false);
			return;
		}
		if (FAILED(m_pShader->Bind_SRV("g_SSAONoiseTexture", m_pSSAO_NoiseSRV))) {
			assert(false);
			return;
		}
	}

	if (FAILED(m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::SSAO_OCCLUSION)))){
		assert(false);
		return ;
	}
	if (FAILED(m_pVIBuffer->Bind_Resources())){
		assert(false);
		return ;
	}
	if (FAILED(m_pVIBuffer->Render())){
		assert(false);
		return ;
	}
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_SSAO");
}
void CRenderer::Render_SSAO_BLUR()
{
	COMPUTE_TIMEDELTA("Timer_Render_SSAO_BLUR");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BLUR")))) {
		return;
	}
	{
		// Bind_Resorces
		m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
		m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
		m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
		if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
			return;
		}
		// Bind_Targets
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_SSAO_AmbientOcclusion"), m_pShader, "g_SSAOInputTexture"))) {
			return;
		}
	}
	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::SSAO_BLUR));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_SSAO_BLUR");
}
void CRenderer::Render_Blend()
{
	COMPUTE_TIMEDELTA("Timer_Render_Blend");
	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return pSour->Get_Depth() > pDest->Get_Depth();
		});


	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLEND)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_Blend");
}

void CRenderer::Render_Bloom()
{
	COMPUTE_TIMEDELTA("Timer_Render_Bloom");
	// "Target_Bloom_Input"
	// "Target_Bloom"
	// "Target_Bloom_2x2"
	// "Target_Bloom_4x4"
	// "Target_Bloom_8x8"
	// "Target_Bloom_2x2_1"
	// "Target_Bloom_4x4_1"
	// "Target_Bloom_8x8_1"
	// "Target_Bloom_8x8_X"
	// "Target_Bloom_4x4_2"
	// "Target_Bloom_2x2_2"

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom")))) {
		return;
	}

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLOOM)])
	{
		if (nullptr != pRenderObject)
			if (FAILED(pRenderObject->Render_Bloom())) {
				assert(false);
			}
		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BLOOM)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	m_pShader->Bind_RawValue("g_fBloomThreshold", &m_fBloomThreshold, sizeof(_float));
	m_pShader->Bind_RawValue("g_iBloomEmbossingPass", &m_iBloomEmbossingPass, sizeof(_int));

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom_2x2"), SHADER_PASS_DEFERRED::EMBOSSING); // 2

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2"), TEXT("Target_Bloom_4x4_1"), SHADER_PASS_DEFERRED::BLOOM_BLURX); // 3
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_1"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 3

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_8x8_1"), SHADER_PASS_DEFERRED::BLOOM_BLURX); // 4
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8_1"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 4

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_8x8_X"), SHADER_PASS_DEFERRED::BLOOM_BLURX); // 5
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8_X"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 5
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::UPSAMPLE); // 6

	m_pGameInstance->Accumulate_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_4x4_2"), SHADER_PASS_DEFERRED::BLOOM_ACCUM);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_4x4_2"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 7

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_2x2"), SHADER_PASS_DEFERRED::UPSAMPLE);
	m_pGameInstance->Accumulate_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2"), TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_2x2_2"), SHADER_PASS_DEFERRED::BLOOM_ACCUM);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2_2"), TEXT("Target_Bloom_2x2"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2"), TEXT("Target_Bloom_2x2_2"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 8

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2_2"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::UPSAMPLE);
	m_pGameInstance->Finish_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::BLOOM_FINISH); // 9

	COMPUTE_TIMEDELTA("Timer_Render_Bloom");
}

void CRenderer::Render_UI()
{
	COMPUTE_TIMEDELTA("Timer_Render_UI");
	m_RenderObjects[ENUM_CLASS(RENDER::UI)].sort([](CGameObject* pDest, CGameObject* pSrc)->_bool {
		_float3 vDstPos = {};
		_float3 vSrcPos = {};
		XMStoreFloat3(&vDstPos, pDest->Get_WorldPostion());
		XMStoreFloat3(&vSrcPos, pSrc->Get_WorldPostion());

		return vDstPos.z > vSrcPos.z;
		}); // z 소팅



	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::UI)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}
		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::UI)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_UI");
}

void CRenderer::Render_UI_Overley()
{
	COMPUTE_TIMEDELTA("Timer_Render_UI_Overley");
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::UI_OVERLAY)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}
		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::UI_OVERLAY)].clear();

	COMPUTE_TIMEDELTA("Timer_Render_UI_Overley");
}

void CRenderer::Render_LastColor()
{
	COMPUTE_TIMEDELTA("Timer_Render_LastColor");
	// Bind_Resorces

	m_pLastColorShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pLastColorShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pLastColorShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Color"), m_pLastColorShader, "g_ColorTexture"))) {
		return;
	}

	m_pLastColorShader->Begin(0);

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	COMPUTE_TIMEDELTA("Timer_Render_LastColor");
}

void CRenderer::Render_Tone_Mapping()
{
	COMPUTE_TIMEDELTA("Timer_Render_Tone_Mapping");
	{ // BackBuffer 
		ID3D11Texture2D* pBackBuffer = nullptr;
		m_pGameInstance->Get_BackBufferPTR(&pBackBuffer);
		m_pGameInstance->Paste_RenderTarget(TEXT("Target_ToneMapping"), pBackBuffer);
		SAFE_RELEASE(pBackBuffer);
	}

	m_pLastColorShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pLastColorShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pLastColorShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);
	
	if (FAILED(m_pShader->Bind_RawValue("g_iToneMappingType", &m_iToneMappingType, sizeof(_uint)))) {
		assert(false);
		return;
	}
	
	if (FAILED(m_pShader->Bind_RawValue("g_fToneMappingExposure", &m_fExposure, sizeof(_float)))) {
		assert(false);
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_ToneMapping"), m_pShader, "g_OriginalTexture"))) {
		assert(false);
		return;
	}


	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::TONE_MAPPING));
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	COMPUTE_TIMEDELTA("Timer_Render_Tone_Mapping");
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		if (nullptr != pDebugCom) {
			if (FAILED(pDebugCom->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pDebugCom);
	} m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix))) {
		return;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix))) {
		return;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return;
	}

	/* 렌더타겟을 디버그로 직교투영을 통해 그려라. */
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(m_pShader, m_pVIBuffer))) {
		return;
	}

}

#endif
