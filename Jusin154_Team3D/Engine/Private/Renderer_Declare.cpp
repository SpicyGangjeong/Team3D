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
	if (nullptr == pRenderObject || pRenderObject->isDead()) {
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

HRESULT CRenderer::Initialize()
{
	_uint		iNumViewports = { 1 };

	D3D11_VIEWPORT		Viewport{};
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);
	{
		/* Target_ToneMapping */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ToneMapping"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 1.f, 1.f)))) {
			return E_FAIL;
		}
		/* Target_Diffuse */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f)))) {
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

		/* Target_Surface */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Surface"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f)))) {
			return E_FAIL;
		}

		/* Target_Shade */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 1.f)))) {
			return E_FAIL;
		}

		/* Target_Specular */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f)))) {
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
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Weight"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f)))) {
			return E_FAIL;
		}


		/* Target_Blur_X */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X_Weight"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 1.0f)))) {
			return E_FAIL;
		}

		/* Target_Bloom */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Input"), (_uint)(Viewport.Width), (_uint)(Viewport.Height),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), (_uint)(Viewport.Width), (_uint)(Viewport.Height),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_2x2"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4x4"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8x8"), (_uint)(Viewport.Width * 0.125f), (_uint)(Viewport.Height * 0.125f),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_8x8_X"), (_uint)(Viewport.Width * 0.125f), (_uint)(Viewport.Height * 0.125f),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_4x4_2"), (_uint)(Viewport.Width * 0.25f), (_uint)(Viewport.Height * 0.25f),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_2x2_2"), (_uint)(Viewport.Width * 0.5f), (_uint)(Viewport.Height * 0.5f),
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		/* Target_AfterCombined */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AfterCombined"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
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

		/* Target_Diffuse */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Fog"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.0f, 0.f, 0.f, 0.f)))) {
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
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combined"), TEXT("Target_Fog")))) {
			return E_FAIL;
		}
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

		/* MRT_Fog */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Fog"), TEXT("Target_Fog")))) {
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
		GUI::DragFloat("m_fBloomThreshold", &m_fBloomThreshold, 0.01f, 0.02f, 100.f, "%.3f");
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

		if (m_fDOF_ENV_StartDistance < m_fDOF_ENV_FocusDistance) {
			m_fDOF_ENV_StartDistance = m_fDOF_ENV_FocusDistance;
		}

		if (m_fDOF_ENV_MaxEnd < m_fDOF_ENV_StartDistance + 1.0f) {
			m_fDOF_ENV_MaxEnd = m_fDOF_ENV_StartDistance + 1.0f;
		}

		GUI::PopItemWidth();
		GUI::EndChild();
	}
	if (GUI::CollapsingHeader("ToneMapping")) {
		GUI::BeginChild("TONE_MAP", ImVec2(0, 0), true);
		GUI::SliderInt("m_iToneMappingType", &m_iToneMappingType, 0, 2, "%d");
		GUI::SliderFloat("m_fExposure", &m_fExposure, 0.5f, 2.f, "%.3f");
		GUI::EndChild();
	}
	GUI::End();
}
#endif
