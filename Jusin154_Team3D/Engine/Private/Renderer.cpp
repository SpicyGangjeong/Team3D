#include "pch.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"

void CRenderer::Render()
{
	Bind_RawValue();
	Render_Priority();
	Render_Shadow();
	Render_NonBlend();
	Render_Decal();
	Render_EffectNonBlend();
	Render_SSAO();
	Render_SSAO_BLUR();
	Render_LightAcc();
	Render_Combined();
	Render_Occlusion();
	Render_EnvironmentPostProcess();
	Render_Fog();
	Render_Blur();
	Combine_Blur();
	Render_Distortion();
	Render_DistortionAcc();
	Render_Effect();
	Render_NonLight();
	Render_Blend();
	Render_Blur_Mesh();
	Render_WeightBlend();
	Render_PostProcessing();
	Render_Tone_Mapping();
	Render_UI();
	Render_UI_Overley();

	memcpy_s(&m_MotionBlurPreViewMatrix,	sizeof(_float4x4), m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW), sizeof(_float4x4));
	memcpy_s(&m_MotionBlurPreProjMatrix,	sizeof(_float4x4), m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ), sizeof(_float4x4));

#ifdef RELEASE_DEBUGGER
	static _bool m_bToggleDebug = false;
	static _bool m_bRenderSystem = false;
	if (m_pGameInstance->Key_Up(DIK_F6)) {
		m_bRenderSystem = !m_bRenderSystem;
	}
	if (true == m_bRenderSystem) {
		Describe_Entitiy();
		m_pGameInstance->RenderTarget_Debuger();
		GUI::Begin("RenderTarget Debuger", 0, IMGUI_GLOBAL_BEGIN_FLAG);
		GUI::Checkbox("DebugToggle", &m_bToggleDebug);

		if (m_bToggleDebug) {
			Render_Debug();
		}
		GUI::End();
	}
	if (m_pGameInstance->Key_Pressing(DIK_F10)) {
		Render_Debug();
	}
#endif
	m_eType = RENDER::END;
}

RENDER CRenderer::Get_CurrentRenderPass()
{
	return m_eType;
}

void CRenderer::Render_PreShadow(const _float4x4& ViewMatrix, const _float4x4& ProjMatrix)
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PreShadow"), m_pPreShadowDSV))) {
		return;
	}

	m_PreShadowView = ViewMatrix;
	m_PreShadowProj = ProjMatrix;

	D3D11_VIEWPORT			ViewPortOldDesc;
	D3D11_VIEWPORT			ViewPortDesc;
	_uint					iNumViewOldPort = { 1 };
	ZeroMemory(&ViewPortOldDesc, sizeof(D3D11_VIEWPORT));
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	{
		ViewPortDesc.TopLeftX = 0;
		ViewPortDesc.TopLeftY = 0;
		ViewPortDesc.Width		= (_float)g_iPreShadowWidth;
		ViewPortDesc.Height		= (_float)g_iPreShadowHeight;
		ViewPortDesc.MinDepth = 0.f;
		ViewPortDesc.MaxDepth = 1.f;
	}
	m_pContext->RSGetViewports(&iNumViewOldPort, &ViewPortOldDesc);
	m_pContext->RSSetViewports(1, &ViewPortDesc);

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRESHADOW)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render_Shadow(SHADOW::SHADOW_PRE))) {
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

HRESULT CRenderer::Bind_PreShadowMatrix(class CShader* pShader, const _char* pConstants, D3DTS eType)
{
	if (D3DTS::VIEW == eType) {
		if (FAILED(pShader->Bind_Matrix(pConstants, &m_PreShadowView))) {
			return E_FAIL;
		}
	} else if (D3DTS::PROJ == eType) {
		if (FAILED(pShader->Bind_Matrix(pConstants, &m_PreShadowProj))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CRenderer::Bind_PrevMatrix(class CShader* pShader, const _char* pConstants, D3DTS eType)
{
	if (D3DTS::VIEW == eType) {
		if (FAILED(pShader->Bind_Matrix(pConstants, &m_MotionBlurPreViewMatrix))) {
			return E_FAIL;
		}
	}
	else if (D3DTS::PROJ == eType) {
		if (FAILED(pShader->Bind_Matrix(pConstants, &m_MotionBlurPreProjMatrix))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

void CRenderer::Set_Environment(_float3 vSSAO, _float fExposure)
{
	m_fSSAO_Radius = vSSAO.x;
	m_fSSAO_BIAS = vSSAO.y;
	m_fSSAOStrength = vSSAO.z;
	m_fExposure = fExposure;
	// m_fSSAO_Radius = 0.585f;
	// m_fSSAO_BIAS = 0.182f;
	// m_fSSAOStrength = 2.83f;
}

void CRenderer::Bind_RawValue()
{
	_uint iNumViewPort = { 1 };
	D3D11_VIEWPORT vp = {};
	m_pContext->RSGetViewports(&iNumViewPort, &vp);
	_float2 vResolution = { vp.Width, vp.Height };
	if (FAILED(m_pShader->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2)))) {
		assert(false);
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vNearShadowResolution", &m_vNearShadowResoltion, sizeof(_float2)))) {
		return;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vMiddleShadowResolution", &m_vMiddleShadowResoltion, sizeof(_float2)))) {
		return;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vFarShadowResolution", &m_vFarShadowResoltion, sizeof(_float2)))) {
		return;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vPreShadowResolution", &m_vPreShadowResoltion, sizeof(_float2)))) {
		return;
	}
}

void CRenderer::Render_Occlusion()
{
	COMPUTE_TIMEDELTA("Timer_Render_Occlusion");
	EVENTSCOPE_("Render_Occlusion");
	m_eType = RENDER::OCCLUSION;
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
	EVENTSCOPE_("Render_Priority");
	m_eType = RENDER::PRIORITY;
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LDRBackbuffer")))) {
		return;
	}
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	m_RenderObjects[ENUM_CLASS(RENDER::PRIORITY)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_Priority");
}

void CRenderer::Render_Shadow()
{
	COMPUTE_TIMEDELTA("Timer_Render_Shadow");
	EVENTSCOPE_("Render_Shadow");
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
	if (FAILED(m_pShader->Bind_RawValue("g_fMinShadowBrightness", &m_fMinShadowBrightness, sizeof(_float)))) {
		assert(false);
	}
	if (FAILED(m_pGameInstance->Bind_CascadeValues(m_pShader))) {
		assert(false);
	}


	{ // MRT_Shadow_Near
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Near"), m_pShadowDSV_NEAR))) {
			return;
		}

		m_eType = RENDER::SHADOW_NEAR;
		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		{
			ViewPortDesc.TopLeftX = 0;
			ViewPortDesc.TopLeftY = 0;
			ViewPortDesc.Width = (_float)(m_vNearShadowResoltion.x);
			ViewPortDesc.Height = (_float)(m_vNearShadowResoltion.y);
			ViewPortDesc.MinDepth = 0.f;
			ViewPortDesc.MaxDepth = 1.f;
		}
		m_pContext->RSSetViewports(1, &ViewPortDesc);

		for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_NEAR)]) {
			if (nullptr != pRenderObject) {
				if (FAILED(pRenderObject->Render_Shadow(SHADOW::SHADOW_NEAR))) {
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

		m_eType = RENDER::SHADOW_MIDDLE;
		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		{
			ViewPortDesc.TopLeftX = 0;
			ViewPortDesc.TopLeftY = 0;
			ViewPortDesc.Width = (_float)(m_vMiddleShadowResoltion.x);
			ViewPortDesc.Height = (_float)(m_vMiddleShadowResoltion.y);
			ViewPortDesc.MinDepth = 0.f;
			ViewPortDesc.MaxDepth = 1.f;
		}
		m_pContext->RSSetViewports(1, &ViewPortDesc);

		for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW_MIDDLE)]) {
			if (nullptr != pRenderObject) {
				if (FAILED(pRenderObject->Render_Shadow(SHADOW::SHADOW_MIDDLE))) {
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
	m_pContext->RSSetViewports(iNumViewOldPort, &ViewPortOldDesc);

	COMPUTE_TIMEDELTA("Timer_Render_Shadow");
}

void CRenderer::Render_NonBlend()
{
	COMPUTE_TIMEDELTA("Timer_Render_NonBlend");
	EVENTSCOPE_("Render_NonBlend");
	m_eType = RENDER::NONBLEND;

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

void CRenderer::Render_Decal()
{
	m_pGameInstance->Copy_RenderTargetAToB(TEXT("Target_Normal"), TEXT("Target_NormalCopy"));

	EVENTSCOPE_("Render_Decal");
	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_Decal")))) {
		return;
	}
	m_eType = RENDER::DECAL;
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::DECAL)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::DECAL)].clear();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	m_pGameInstance->Clear_RenderTarget(TEXT("Target_NormalCopy"));
}

void CRenderer::Render_EffectNonBlend()
{
	COMPUTE_TIMEDELTA("Timer_Render_EffectNonBlend");
	EVENTSCOPE_("Render_EffectNonBlend");
	m_eType = RENDER::EFFECT_NONBLEND;

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_GameObjects")))) {
		return;
	}

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::EFFECT_NONBLEND)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::EFFECT_NONBLEND)].clear();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_EffectNonBlend");
}

void CRenderer::Render_LightAcc()
{
	COMPUTE_TIMEDELTA("Timer_Render_LightAcc");
	EVENTSCOPE_("Render_LightAcc");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc")))) {
		return;
	}

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);
	m_pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
	m_pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));
	m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));
	m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float));
	m_pShader->Bind_RawValue("g_fLightSpecularMaximum", &m_fLightSpecularMaximum, sizeof(_float));

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
	EVENTSCOPE_("Render_Combined");


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combined")))) {
		return;
	}
	{
		// Bind_Resorces

		m_pShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
		m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
		m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);
		m_pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
		m_pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));
		
		if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix_NEAR", D3DTS::VIEW, SHADOW::SHADOW_NEAR))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix_NEAR", D3DTS::PROJ, SHADOW::SHADOW_NEAR))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix_MIDDLE", D3DTS::VIEW, SHADOW::SHADOW_MIDDLE))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix_MIDDLE", D3DTS::PROJ, SHADOW::SHADOW_MIDDLE))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix_FAR", D3DTS::VIEW, SHADOW::SHADOW_FAR))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix_FAR", D3DTS::PROJ, SHADOW::SHADOW_FAR))) {
			return;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_PreShadowLightViewMatrix", &m_PreShadowView))) {
			return;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_PreShadowLightProjMatrix", &m_PreShadowProj))) {
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
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_PreShadow"), m_pShader, "g_PreShadowTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurXTexture"))) {
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
	EVENTSCOPE_("Render_EnvironmentPostProcess");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ENV_Blur_X")))) { // Target_ENV_Blur_X
		return;
	}
	
	m_pShader->Bind_Matrix("g_WorldMatrix",				&m_ScreenWorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix",				&m_ScreenViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix",				&m_ScreenProjMatrix);
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

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);
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
	EVENTSCOPE_("Render_Fog");


	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Fog"), m_pShader, "g_Texture"))) {
		return;
	}

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::PRINT_BACKBUFFER));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	if (FAILED(m_pGameInstance->Copy_RenderTargetAToB(TEXT("Target_LDRBackBuffer"), TEXT("Target_Fog")))) {
		return;
	}


	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		return;
	}

	m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float));
	m_pShader->Bind_RawValue("g_fDepthPackExponent", m_pGameInstance->Get_DepthPackExponentPtr(), sizeof(_float));

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Fog"), m_pShader, "g_OriginalTexture"))) {
		return;
	}

	if (FAILED(m_pShader->Bind_SRV("g_VolumeTexture", m_pGameInstance->Get_VolumeSRV())))
		return;

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::FOG));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_Fog");
}

void CRenderer::Render_Effect()
{
	COMPUTE_TIMEDELTA("Timer_Render_Effect");
	EVENTSCOPE_("Render_Effect");

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_WB")))) {
		return;
	}

	m_eType = RENDER::EFFECT;

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
	EVENTSCOPE_("Render_WeightBlend");
	// Bind_Resorces

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		assert(false); return;
	}
	m_pWeightBlendShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pWeightBlendShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pWeightBlendShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);


	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_WB_Color"), m_pWeightBlendShader, "g_MixedDiffuseTexture"))) {
		return;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_WB_Revealage"), m_pWeightBlendShader, "g_RevealageTexture"))) {
		return;
	}

	m_pWeightBlendShader->Begin(0);

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_WeightBlend");
}

void CRenderer::Render_NonLight()
{
	COMPUTE_TIMEDELTA("Timer_Render_NonLight");
	EVENTSCOPE_("Render_NonLight");
	m_eType = RENDER::NONLIGHT;

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		return;
	}
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject){
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_NonLight");
}

void CRenderer::Render_Blur()
{
	COMPUTE_TIMEDELTA("Timer_Render_Blur");
	EVENTSCOPE_("Render_Blur");
	m_eType = RENDER::BLUR;

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

	m_pBlurShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pBlurShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pBlurShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur"), m_pBlurShader, "g_BlurTexture"))) {
		return;
	}

	m_pBlurShader->Begin(ENUM_CLASS(SHADER_PASS_BLUR::BLUR_X));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return;
	}

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y")))) {
		return;
	}

	m_pBlurShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pBlurShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pBlurShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pBlurShader, "g_BlurXTexture"))) {
		return;
	}

	m_pBlurShader->Begin(ENUM_CLASS(SHADER_PASS_BLUR::BLUR_Y));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return;
	}

	COMPUTE_TIMEDELTA("Timer_Render_Blur");
}

void CRenderer::Combine_Blur()
{
	COMPUTE_TIMEDELTA("Timer_Combine_Blur");
	EVENTSCOPE_("Combine_Blur");

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_Y"), m_pBlurShader, "g_BlurYTexture"))) {
		return;
	}

	_uint iNumViewPort = { 1 };
	D3D11_VIEWPORT vp = {};
	m_pContext->RSGetViewports(&iNumViewPort, &vp);
	_float2 vResolution = { vp.Width, vp.Height };

	if (FAILED(m_pBlurShader->Bind_RawValue("g_vResolution", &vResolution, sizeof(_float2)))) {
		assert(false);
	}

	m_pBlurShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pBlurShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pBlurShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);


	m_pBlurShader->Begin(ENUM_CLASS(SHADER_PASS_BLUR::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Combine_Blur");
}

void CRenderer::Render_SSAO()
{
	COMPUTE_TIMEDELTA("Timer_Render_SSAO");
	EVENTSCOPE_("Render_SSAO");
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

		if (FAILED(m_pShader->Bind_RawValue("g_fSSAOStrength", &m_fSSAOStrength, sizeof(_float)))) {
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
	EVENTSCOPE_("Render_SSAO_BLUR");
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BLUR")))) {
		return;
	}
	{
		// Bind_Resorces
		m_pShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
		m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
		m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);
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
	EVENTSCOPE_("Render_Blend");
	m_eType = RENDER::BLEND;
	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return pSour->Get_Depth() > pDest->Get_Depth();
		});

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		return;
	}
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BLEND)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	m_RenderObjects[ENUM_CLASS(RENDER::BLEND)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_Blend");
}

void CRenderer::Render_Blur_Mesh()
{
	COMPUTE_TIMEDELTA("Timer_Render_Blur_Mesh");
	EVENTSCOPE_("Render_Blur_Mesh");
	m_eType = RENDER::BLUR;
	m_RenderObjects[ENUM_CLASS(RENDER::BULR_MESH)].sort([](CGameObject* pSour, CGameObject* pDest)->_bool {
		return pSour->Get_Depth() > pDest->Get_Depth();
		});

	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_Blur_Mesh")))) {
		assert(false); return;
	}
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::BULR_MESH)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	m_RenderObjects[ENUM_CLASS(RENDER::BULR_MESH)].clear();
	COMPUTE_TIMEDELTA("Timer_Render_Blur_Mesh");
}

void CRenderer::Render_PostProcessing()
{
	/* POSTPROCESSING */
	COMPUTE_TIMEDELTA("Timer_Render_PostProcessing");
	EVENTSCOPE_("Render_PostProcessing");

	{
		if (FAILED(m_pGameInstance->Copy_RenderTargetAToB(TEXT("Target_LDRBackBuffer"), TEXT("Target_AfterBlend")))) {
			return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_iMBTileSize", &m_iMBTileSize, sizeof(_int)))) {
			assert(false);  return;
		}
		m_pGameInstance->Copy_RenderTargetAToB(TEXT("Target_AfterBlend"), TEXT("Target_MotionBlur"));
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_VelocityMap"), TEXT("Target_VelocityTile"), SHADER_PASS_DEFERRED::MOTIONBLURTILE);
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_VelocityTile"), TEXT("Target_VelocityTent"), SHADER_PASS_DEFERRED::MOTIONBLURTENT);
	}
	/* MotionBlur */
	if (true == m_bMB) {
		if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
			assert(false); return;
		}
	}
	else {
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur")))) {
			assert(false); return;
		}
	}
	{	// Bind_Resorces
		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fMBMaxBlurRadius", &m_fMBMaxBlurRadius, sizeof(_float)))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_fMBSampleBias", &m_fMBSampleBias, sizeof(_float)))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_iMBSampleCount", &m_iMBSampleCount, sizeof(_int)))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_iMBMaxSampleCount", &m_iMBMaxSampleCount, sizeof(_int)))) {
			assert(false); return;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_iMBType", &m_iMBType, sizeof(_int)))) {
			assert(false); return;
		}
	}
	{
		// Bind_Targets
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_AfterBlend"), m_pShader, "g_DiffuseTexture"))) {
			assert(false);
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_AfterBlend"), m_pShader, "g_ColorTexture"))) {
			assert(false);
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
			assert(false);
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_VelocityMap"), m_pShader, "g_VelocityTexture"))) {
			assert(false);
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_VelocityTent"), m_pShader, "g_TileVelocityTexture"))) {
			assert(false);
			return;
		}
	}
	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::MOTIONBLUR));
	
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom")))) {
		return;
	}
	m_eType = RENDER::BLOOM;

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


	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		assert(false); return;
	}
	m_pGameInstance->Finish_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::BLOOM_FINISH); // 9
	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_PostProcessing");
}

void CRenderer::Render_Distortion()
{
	COMPUTE_TIMEDELTA("Timer_Render_Distortion");
	EVENTSCOPE_("Render_Distortion");
	m_eType = RENDER::DISTORTION;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion")))) {
		return;
	}

	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::DISTORTION)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::DISTORTION)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}

	COMPUTE_TIMEDELTA("Timer_Render_Distortion");
}

void CRenderer::Render_DistortionAcc()
{
	COMPUTE_TIMEDELTA("Timer_Render_DistortionAcc");
	EVENTSCOPE_("Render_DistortionAcc");
	// Bind_Resorces

	if (FAILED(m_pGameInstance->Copy_RenderTargetAToB(TEXT("Target_LDRBackBuffer"), TEXT("Target_PreEffect")))) {
		return;
	}
	if (FAILED(m_pGameInstance->Begin_MRT_NonClear(TEXT("MRT_LDRBackbuffer")))) {
		assert(false); return;
	}
	m_pDistortionShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pDistortionShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pDistortionShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Distortion"), m_pDistortionShader, "g_DistortionTexture"))) {
		return;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_PreEffect"), m_pDistortionShader, "g_SceneTexture"))) {
		return;
	}

	m_pDistortionShader->Begin(0);

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
	COMPUTE_TIMEDELTA("Timer_Render_DistortionAcc");
}

void CRenderer::Render_UI()
{
	COMPUTE_TIMEDELTA("Timer_Render_UI");
	EVENTSCOPE_("Render_UI");
	m_eType = RENDER::UI;
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
	EVENTSCOPE_("Render_UI_Overley");
	m_eType = RENDER::UI_OVERLAY;
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


void CRenderer::Render_Tone_Mapping()
{
	COMPUTE_TIMEDELTA("Timer_Render_Tone_Mapping");
	EVENTSCOPE_("Render_Tone_Mapping");
	if (FAILED(m_pGameInstance->Copy_RenderTargetAToB(TEXT("Target_LDRBackBuffer"), TEXT("Target_ToneMapping")))) {
		return;
	}

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_ScreenWorldMatrix);
	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix);
	
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

#ifdef RELEASE_DEBUGGER

void CRenderer::Render_Debug()
{
	EVENTSCOPE_("Render_Debug");
	for (auto& pDebugCom : m_DebugComponents)
	{
		if (nullptr != pDebugCom) {
			if (FAILED(pDebugCom->Render())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pDebugCom);
	} m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ScreenViewMatrix))) {
		return;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ScreenProjMatrix))) {
		return;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return;
	}

	/* 렌더타겟을 디버그로 직교투영 */
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(m_pShader, m_pVIBuffer))) {
		return;
	}

}

#endif
