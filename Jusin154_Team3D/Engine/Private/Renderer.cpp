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


void CRenderer::Refresh_Renderer()
{
	if (nullptr == m_pGameInstance->Get_CamPosition()) {
		return;
	}
	_matrix matViewInv = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix matProjInv = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

	matProjInv = XMMatrixInverse(nullptr, matProjInv);
	matViewInv = XMMatrixInverse(nullptr, matViewInv);

	memcpy_s(&m_CubeViewFrustum, sizeof(_float3) * 8, m_CubeNDC, sizeof(_float3) * 8);

	for (int i = 0; i < 8; ++i) {
		XMStoreFloat3(&m_CubeViewFrustum[i], XMVector3TransformCoord(XMVectorSet(m_CubeViewFrustum[i].x, m_CubeViewFrustum[i].y, m_CubeViewFrustum[i].z, 1.f), matProjInv));
		XMStoreFloat3(&m_CubeViewFrustum[i], XMVector3TransformCoord(XMVectorSet(m_CubeViewFrustum[i].x, m_CubeViewFrustum[i].y, m_CubeViewFrustum[i].z, 1.f), matViewInv));
	}
	// 0(1, 3, 4), 6(2, 5, 7) ( 법선벡터 후보 )
	// 0, 6( 한 점 후보)
	//_float3								m_CubeNDC[8] = {
	//	근	좌하	우하	우상	좌상
	//	원	좌하	우하	우상	좌상
	//		0,		1,		2,		3
	//		4,		5,		6,		7
	//{ -1.f, -1.f, 0.f }, { 1.f, -1.f, 0.f }, { 1.f, 1.f, 0.f }, { -1.f, 1.f, 0.f },
	//{ -1.f, -1.f, 1.f }, { 1.f, -1.f, 1.f }, { 1.f, 1.f, 1.f }, { -1.f, 1.f, 1.f }
	//};
	_vector vPlanes[8] = {};
	XMStoreFloat4(&m_vPlanes[0], XMPlaneNormalize(XMPlaneFromPoints(XMLoadFloat3(&m_CubeViewFrustum[0]), XMLoadFloat3(&m_CubeViewFrustum[3]), XMLoadFloat3(&m_CubeViewFrustum[4]))));
	XMStoreFloat4(&m_vPlanes[1], XMPlaneNormalize(XMPlaneFromPoints(XMLoadFloat3(&m_CubeViewFrustum[5]), XMLoadFloat3(&m_CubeViewFrustum[6]), XMLoadFloat3(&m_CubeViewFrustum[2]))));
	XMStoreFloat4(&m_vPlanes[2], XMPlaneNormalize(XMPlaneFromPoints(XMLoadFloat3(&m_CubeViewFrustum[1]), XMLoadFloat3(&m_CubeViewFrustum[2]), XMLoadFloat3(&m_CubeViewFrustum[3]))));
	XMStoreFloat4(&m_vPlanes[3], XMPlaneNormalize(XMPlaneFromPoints(XMLoadFloat3(&m_CubeViewFrustum[4]), XMLoadFloat3(&m_CubeViewFrustum[7]), XMLoadFloat3(&m_CubeViewFrustum[6]))));
	XMStoreFloat4(&m_vPlanes[4], XMPlaneNormalize(XMPlaneFromPoints(XMLoadFloat3(&m_CubeViewFrustum[0]), XMLoadFloat3(&m_CubeViewFrustum[4]), XMLoadFloat3(&m_CubeViewFrustum[5]))));
	XMStoreFloat4(&m_vPlanes[5], XMPlaneNormalize(XMPlaneFromPoints(XMLoadFloat3(&m_CubeViewFrustum[3]), XMLoadFloat3(&m_CubeViewFrustum[2]), XMLoadFloat3(&m_CubeViewFrustum[7]))));
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject* pRenderObject, _float4& vPos, _float fCullRadius)
{
	if (nullptr == pRenderObject)
		return E_FAIL;

	_bool	bPossible = { true };
	//if (pRenderObject->isDead()) {
	//	return E_FAIL;
	//}
	//// 프러스텀컬링 예외 추가
	//if (RENDER::UI == eRenderGroup || RENDER::PRIORITY == eRenderGroup /* || RENDER::SHADOW == eRenderGroup*/ || RENDER::BLUR == eRenderGroup) {
	//	m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);
	//	SAFE_ADDREF(pRenderObject);
	//	return S_OK;
	//}
	//for (int i = 0; i < 6; ++i) {
	//	if (XMVectorGetX(XMVector4Dot(XMLoadFloat4(&m_vPlanes[i]),
	//		XMLoadFloat4(&vPos))) + fCullRadius < 0) {
	//		bPossible = false;
	//		break;
	//	}
	//}
	if (bPossible) {
		m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);
		SAFE_ADDREF(pRenderObject);
		return S_OK;
	}
	return S_OK;
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
	Render_NonLight();
	Render_Blend();
	Render_UI();

#ifdef _DEBUG
	if(m_pGameInstance->Key_Pressing(DIK_F10))
		Render_Debug();
#endif
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

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), m_pShader, "g_NormalTexture"))) {
		return;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShader, "g_DepthTexture"))) {
		return;
	}


	m_pVIBuffer->Bind_Resources();

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer))) {
		return;
	}

	if (FAILED(m_pGameInstance->End_MRT())) {
		return;
	}
}

void CRenderer::Render_Combined()
{
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

		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightViewMatrix", D3DTS::VIEW))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShader, "g_LightProjMatrix", D3DTS::PROJ))) {
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
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_PreShadow"), m_pShader, "g_PreShadowTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Blur_X"), m_pShader, "g_BlurXTexture"))) {
			return;
		}
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Color"), m_pShader, "g_ColorTexture"))) {
			return;
		}

	}

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::COMBINED));

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

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_DEFERRED::BLUR));

	m_pVIBuffer->Bind_Resources();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT())) {
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

HRESULT CRenderer::Ready_DepthStencilView(_uint iSizeX, _uint iSizeY)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

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

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture))) {
		return E_FAIL;
	}


	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV))) {
		return E_FAIL;
	}

	SAFE_RELEASE(pDepthStencilTexture);

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
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer))) {
		return;
	}
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer))) {
		return;
	}
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer))) {
		return;
	}
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(TEXT("MRT_PreShadow"), m_pShader, m_pVIBuffer))) {
		return;
	}
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(TEXT("MRT_Blur"), m_pShader, m_pVIBuffer))) {
		return;
	}
	if (FAILED(m_pGameInstance->Render_RenderTarget_Debug(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer))){
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

		/* Target_Blur_X */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}

		/* Target_Color*/
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Color"), (_uint)Viewport.Width, (_uint)Viewport.Height,
			DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 0.0f, 0.0f, 0.0f)))) {
			return E_FAIL;
		}



		if (FAILED(Ready_DepthStencilView(g_iMaxShadowWidth, g_iMaxShadowHeight))) {
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

		/* MRT_Blur_X */
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X")))) {
			return E_FAIL;
		}

	}
	m_pShader = (CShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, TEXT("FX_DEFERRED"), nullptr, nullptr);
	if (nullptr == m_pShader) {
		return E_FAIL;
	}

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer) {
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(Viewport.Width, Viewport.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Diffuse"), 75.f, 75.f, 150.0f, 150.0f))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Normal"), 75.f, 225.f, 150.0f, 150.0f))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shade"), 225.f, 75.f, 150.0f, 150.0f))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Specular"), 225.f, 225.f, 150.0f, 150.0f))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Color"), 375.f, 75.f, 150.0f, 150.0f))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Shadow"), Viewport.Width - 150.0f, 150.0f, 300.f, 300.f))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_PreShadow"), Viewport.Width - 150.0f, Viewport.Height - 150.0f, 300.f, 300.f))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Blur"), 150.0f, Viewport.Height - 150.0f, 300.f, 300.f))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_RenderTarget_Debug(TEXT("Target_Blur_X"), Viewport.Width * 0.75f, Viewport.Height * 0.75f, Viewport.Width * 0.5f, Viewport.Height * 0.5f))) {
		return E_FAIL;
	}




#endif

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
	SAFE_RELEASE(m_pVIBuffer);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
