#include "pch.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
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
	Tone_Mapping();
	Render_UI();

#ifdef _DEBUG
	Describe_Entitiy();
	m_pGameInstance->RenderTarget_Debuger();

	if (m_pGameInstance->Key_Pressing(DIK_F10)) {
		Render_Debug();
	}
#endif
}

void CRenderer::Render_Occlusion()
{
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
}

void CRenderer::Render_Priority()
{
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

}

void CRenderer::Render_Shadow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV))) {
		return;
	}

	D3D11_VIEWPORT			ViewPortOldDesc;
	D3D11_VIEWPORT			ViewPortDesc;
	_uint					iNumViewOldPort = { 1 };
	ZeroMemory(&ViewPortOldDesc, sizeof(D3D11_VIEWPORT));
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	{
		ViewPortDesc.TopLeftX = 0;
		ViewPortDesc.TopLeftY = 0;
		ViewPortDesc.Width = (_float)g_iMaxShadowWidth;
		ViewPortDesc.Height = (_float)g_iMaxShadowHeight;
		ViewPortDesc.MinDepth = 0.f;
		ViewPortDesc.MaxDepth = 1.f;
	}
	m_pContext->RSGetViewports(&iNumViewOldPort, &ViewPortOldDesc);
	m_pContext->RSSetViewports(1, &ViewPortDesc);


	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)])
	{
		if (nullptr != pRenderObject) {
			if (FAILED(pRenderObject->Render_Shadow())) {
				assert(false);
			}
		}

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::SHADOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}


	m_pContext->RSSetViewports(iNumViewOldPort, &ViewPortOldDesc);
}

void CRenderer::Render_NonBlend()
{
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
}

void CRenderer::Render_LightAcc()
{
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


	m_pVIBuffer->Bind_Resources();

	if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelID(), m_pShader, m_pVIBuffer))) {
		return;
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
}

void CRenderer::Render_Combined()
{
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

		//if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix", D3DTS::VIEW))) {
		//	return;
		//}
		//if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix", D3DTS::PROJ))) {
		//	return;
		//}
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
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture"))) {
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
}

void CRenderer::Render_EnvironmentPostProcess()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X")))) {
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
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurTexture"))) {
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
}

void CRenderer::Render_Fog()
{
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

}

void CRenderer::Render_Effect()
{
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
}

void CRenderer::Render_WeightBlend()
{
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

}

void CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		SAFE_RELEASE(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDER::NONLIGHT)].clear();
}

void CRenderer::Render_Blur()
{
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

}
void CRenderer::Render_Blend()
{
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
}

void CRenderer::Render_Bloom()
{
	// "Target_Bloom_Input"
	// "Target_Bloom"
	// "Target_Bloom_2x2"
	// "Target_Bloom_4x4"
	// "Target_Bloom_8x8"
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

	m_pShader->Bind_RawValue("m_fBloomThresholdy", &m_fBloomThreshold, sizeof(_float));
	m_pShader->Bind_RawValue("g_iBloomEmbossingPass", &m_iBloomEmbossingPass, sizeof(_int));

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom_2x2"), SHADER_PASS_DEFERRED::EMBOSSING); // 2
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::REFIT); // 3
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::REFIT); // 4

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_8x8_X"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8_X"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 5
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::REFIT); // 6

	m_pGameInstance->Accumulate_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_4x4_2"), SHADER_PASS_DEFERRED::BLOOM_ACCUM);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_4x4_2"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 7

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_2x2"), SHADER_PASS_DEFERRED::REFIT);
	m_pGameInstance->Accumulate_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2"), TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_2x2_2"), SHADER_PASS_DEFERRED::BLOOM_ACCUM);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2_2"), TEXT("Target_Bloom_2x2"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2"), TEXT("Target_Bloom_2x2_2"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 8

	m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_2x2_2"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::REFIT);
	m_pGameInstance->Finish_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::BLOOM_FINISH); // 9
	
}

void CRenderer::Render_UI()
{
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
}

void CRenderer::Render_LastColor()
{
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
}

void CRenderer::Tone_Mapping()
{
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
