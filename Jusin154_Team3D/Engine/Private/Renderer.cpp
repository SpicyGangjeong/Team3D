#include "pch.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance); 
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject || pRenderObject->isDead()){
		return E_FAIL;
	}

	_bool	bPossible = { false };
	// 프러스텀컬링 예외 추가
	if (RENDER::UI == eRenderGroup || RENDER::PRIORITY == eRenderGroup /* || RENDER::SHADOW == eRenderGroup*/ || RENDER::BLUR == eRenderGroup) {
		bPossible = true;
	}

	bPossible = true;
	if (true == bPossible) {
		m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);
		SAFE_ADDREF(pRenderObject);
		return S_OK;
	}
	return E_FAIL;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	m_DebugComponents.push_back(pDebugCom);

	SAFE_ADDREF(pDebugCom);

	return S_OK;
}
#endif

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
	Render_Effect();
	Render_NonLight();
	Render_Blend();
	Render_WeightBlend();
	Render_Bloom();
	Render_LastColor();
	Render_UI();

#ifdef _DEBUG
	Describe_Entitiy();
	m_pGameInstance->RenderTarget_Debuger();

	if(m_pGameInstance->Key_Pressing(DIK_F10)){
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
	if (FAILED(m_pGameInstance->Begin_MRT_Include_BackBuffer(TEXT("MRT_Combined")))) {
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
	if (m_pGameInstance->Key_Up(DIK_END)) {
		m_bDOF_ENV = !m_bDOF_ENV;
	}
	if (false == m_bDOF_ENV) {
		return;
	}
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X")))) {
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

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_AfterCombined"), m_pShader, "g_BlurTexture"))) {
		return;
	}
	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::BLURX_ENVIRONMENT));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();
	if (FAILED(m_pGameInstance->End_MRT())) {
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
	if (m_pGameInstance->Key_Up(DIK_HOME)) {
		m_bPostProcessing_BLOOM = !m_bPostProcessing_BLOOM;
	}
	if(true == m_bPostProcessing_BLOOM) {
		//{ // BackBuffer 
		//	ID3D11Texture2D* pBackBuffer = nullptr;
		//	m_pGameInstance->Get_BackBufferPTR(&pBackBuffer);
		//	//m_pGameInstance->Copy_RenderTarget(TEXT("Target_Diffuse"), pBackBuffer);
		//	m_pGameInstance->Paste_RenderTarget(TEXT("Target_Bloom_Input"), pBackBuffer);
		//	SAFE_RELEASE(pBackBuffer);
		//}

		// "Target_Bloom_Input"
		// "Target_Bloom"
		// "Target_Bloom_4x4"
		// "Target_Bloom_8x8"
		// "Target_Bloom_16x16"
		// "Target_Bloom_16x16_X"
		// "Target_Bloom_8x8_2"
		// "Target_Bloom_4x4_2"

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

		m_pShader->Bind_RawValue("g_fThreshold", &m_fThreshold, sizeof(_float));
		m_pShader->Bind_RawValue("g_iBloomEmbossingPass", &m_iBloomEmbossingPass, sizeof(_int));

		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::EMBOSSING); // 2
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::REFIT); // 3
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_16x16"), SHADER_PASS_DEFERRED::REFIT); // 4

		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_16x16"), TEXT("Target_Bloom_16x16_X"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_16x16_X"), TEXT("Target_Bloom_16x16"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 5
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_16x16"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::REFIT); // 6

		m_pGameInstance->Accumulate_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_16x16"), TEXT("Target_Bloom_8x8_2"), SHADER_PASS_DEFERRED::BLOOM_ACCUM);
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8_2"), TEXT("Target_Bloom_8x8"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8"), TEXT("Target_Bloom_8x8_2"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 7

		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_8x8_2"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::REFIT);
		m_pGameInstance->Accumulate_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_8x8_2"), TEXT("Target_Bloom_4x4_2"), SHADER_PASS_DEFERRED::BLOOM_ACCUM);
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom_4x4"), SHADER_PASS_DEFERRED::BLOOM_BLURX);
		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4"), TEXT("Target_Bloom_4x4_2"), SHADER_PASS_DEFERRED::BLOOM_BLURY); // 8

		m_pGameInstance->Refit_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_4x4_2"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::REFIT);
		m_pGameInstance->Finish_RenderTarget(m_pVIBuffer, m_pShader, TEXT("Target_Bloom_Input"), TEXT("Target_Bloom"), SHADER_PASS_DEFERRED::BLOOM_FINISH); // 9
	}
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

HRESULT CRenderer::Ready_ShadowDepthStencilView(_uint iSizeX, _uint iSizeY)
{
	ID3D11Texture2D* pShadowDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pShadowDepthStencilTexture))) {
		return E_FAIL;
	}


	if (FAILED(m_pDevice->CreateDepthStencilView(pShadowDepthStencilTexture, nullptr, &m_pShadowDSV))) {
		return E_FAIL;
	}

	SAFE_RELEASE(pShadowDepthStencilTexture);

	return S_OK;
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

HRESULT CRenderer::Initialize()
{
	_uint		iNumViewports = { 1 };

	D3D11_VIEWPORT		Viewport{};
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);
	{
		/* Target_Diffuse */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f)))) {
			return E_FAIL;
		}

		/* Target_Normal */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f)))) {
			return E_FAIL;
		}

		/* Target_Depth */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.f, 0.f, 0.f)))) {
			return E_FAIL;
		}

		/* Target_Depth */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Terrain_Depth"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.f, 0.f, 0.f)))) {
			return E_FAIL;
		}

		/* Target_Surface */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Surface"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f)))) {
			return E_FAIL;
		}

		/* Target_Shade */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 1.f)))) {
			return E_FAIL;
		}

		/* Target_Specular */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.f, 0.f, 0.f)))) {
			return E_FAIL;
		}
		/* Target_Shadow */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow"), (_uint)g_iMaxShadowWidth, (_uint)g_iMaxShadowHeight,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)))) {
			return E_FAIL;
		}

		/* Target_Shadow */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PreShadow"), (_uint)g_iMaxShadowWidth, (_uint)g_iMaxShadowHeight,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)))) {
			return E_FAIL;
		}

		/* Target_Blur */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Weight"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f)))) {
			return E_FAIL;
		}


		/* Target_Blur_X */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X_Weight"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f)))) {
			return E_FAIL;
		}

		/* Target_Bloom */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Input"), (_uint)(Viewport.Width), (_uint)(Viewport.Height),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), (_uint)(Viewport.Width), (_uint)(Viewport.Height),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4x4"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8x8"), (_uint)(Viewport.Width * 0.125f), (_uint)(Viewport.Height * 0.125f),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_16x16"), (_uint)(Viewport.Width * 0.0625f), (_uint)(Viewport.Height * 0.0625f),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_16x16_X"), (_uint)(Viewport.Width * 0.0625f), (_uint)(Viewport.Height * 0.0625f),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8x8_2"), (_uint)(Viewport.Width * 0.125f), (_uint)(Viewport.Height * 0.125f),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4x4_2"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f),
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		/* Target_AfterCombined */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AfterCombined"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}


		/* Target_Color*/
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Color"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}


		/*WB_COLOR*/
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_WB_Color"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}


		/* WB_A*/
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_WB_Revealage"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}


		
		if (FAILED(Ready_ShadowDepthStencilView(g_iMaxShadowWidth, g_iMaxShadowHeight))) {
			return E_FAIL;
		}

	}
	{
		/* MRT_GameObjects */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Color")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Surface")))) {
			return E_FAIL;
		}

		/* MRT_Combined */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combined"), TEXT("Target_AfterCombined")))) {
			return E_FAIL;
		}

		/* MRT_LightAcc */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular")))) {
			return E_FAIL;
		}

		/* MRT_Shadow */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow")))) {
			return E_FAIL;
		}
		/* MRT_PreShadow */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PreShadow"), TEXT("Target_PreShadow")))) {
			return E_FAIL;
		}

		/* MRT_Blur */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur_Weight")))) {
			return E_FAIL;
		}

		/* MRT_Blur */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom_Input")))) {
			return E_FAIL;
		}

		/* MRT_Blur_X */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X")))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X_Weight")))) {
			return E_FAIL;
		}

		/* MRT_Color */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Color"), TEXT("Target_Color")))) {
			return E_FAIL;
		}

		/* MRT_WB */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WB"), TEXT("Target_WB_Color")))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WB"), TEXT("Target_WB_Revealage")))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WB"), TEXT("Target_Color")))) {
			return E_FAIL;
		}
	}

	m_pShader = (CShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, FX_DEFERRED, nullptr, nullptr);
	if (nullptr == m_pShader) {
		return E_FAIL;
	}


	m_pLastColorShader = (CShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, FX_LASTCOLOR, nullptr, nullptr);

	if (nullptr == m_pLastColorShader) {
		return E_FAIL;
	}

	m_pWeightBlendShader = (CShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, FX_WEIGHTBELND, nullptr, nullptr);
	
	if (nullptr == m_pWeightBlendShader) {
		return E_FAIL;
	}

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer) {
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));


	return S_OK;
}
CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();


#ifdef _DEBUG
	for (auto& pDebugCom : m_DebugComponents) {
		SAFE_RELEASE(pDebugCom);
	} m_DebugComponents.clear();
#endif


	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects) {
			SAFE_RELEASE(pRenderObject);
		}
		RenderObjects.clear();
	}

	SAFE_RELEASE(m_pShadowDSV);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pLastColorShader);
	SAFE_RELEASE(m_pWeightBlendShader);
	SAFE_RELEASE(m_pVIBuffer);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
#ifdef _DEBUG
void CRenderer::Describe_Entitiy()
{
	GUI::Begin("Renderer");
	GUI::PushItemWidth(80);
	if (GUI::CollapsingHeader("PostProcessing_Bloom"))
	{
		GUI::BeginChild("PostProcessing_Bloom");
		GUI::PushItemWidth(80);
		GUI::DragFloat("g_fThreshold", &m_fThreshold, 0.01f, 0.02f, 100.f, "%.3f");
		GUI::SliderInt("g_iEmbossingPass", &m_iBloomEmbossingPass, 0, 2, "%d");
		GUI::PopItemWidth();
		GUI::EndChild();
	}
	if (GUI::CollapsingHeader("Environment DOF"))
	{
		GUI::BeginChild("DOF_ENV", ImVec2(0, 0), true);
		GUI::PushItemWidth(80.0f);

		GUI::Text("Far DOF (Environment)");
		GUI::Separator();

		GUI::DragFloat("Depth Cut", &m_fDOF_ENV_CutThreshold, 0.001f, 0.0f, 20.0f, "%.4f");

		GUI::DragFloat("Focus Dist", &m_fDOF_ENV_FocusDistance, 1.0f, 0.1f, 125.f, "%.1f");

		GUI::DragFloat("Blur Start", &m_fDOF_ENV_StartDistance, 1.0f, 0.1f, 250.f, "%.1f");

		GUI::DragFloat("Blur End", &m_fDOF_ENV_MaxEnd, 1.0f, 1.0f, 500.f, "%.1f");

		GUI::DragFloat("Max Radius", &m_fDOF_ENV_AmountRadius, 0.1f, 0.0f, 10.0f, "%.2f");

		if (m_fDOF_ENV_StartDistance < m_fDOF_ENV_FocusDistance){
			m_fDOF_ENV_StartDistance = m_fDOF_ENV_FocusDistance;
		}

		if (m_fDOF_ENV_MaxEnd < m_fDOF_ENV_StartDistance + 1.0f){
			m_fDOF_ENV_MaxEnd = m_fDOF_ENV_StartDistance + 1.0f;
		}

		GUI::PopItemWidth();
		GUI::EndChild();
	}
	GUI::End();
}
#endif
