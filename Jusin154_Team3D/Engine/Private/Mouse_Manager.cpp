#include "pch.h"
#include "Mouse_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"


CMouse_Manager::CMouse_Manager(HWND g_hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_ghWnd(g_hWnd)
	, m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(pDevice);
	SAFE_ADDREF(pContext);
}

HRESULT CMouse_Manager::Initialize()
{
	GetCursorPos(&m_ptCurrentMouseCur);
	ScreenToClient(m_ghWnd, &m_ptCurrentMouseCur);
	m_ptOldMouseCur = m_ptCurrentMouseCur;

	D3D11_VIEWPORT	viewPort;
	_uint TargetViewPort = 1;
	m_pContext->RSGetViewports(&TargetViewPort, &viewPort);

	m_iWinCX = (_uint)viewPort.Width;
	m_iWinCY = (_uint)viewPort.Height;

	//m_pGameInstance->Load_SRV("../Bin/Resources/Textures/HUD/BasicCursor.dds", &m_pMouseSRV);


	//XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	//XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(1280.f, 720.f, 0.f, 1.f + FLT_EPSILON));

	//m_pShader = m_pGameInstance->Clone_Prototype<CShader>(g_iStaticLevel,
	//	m_pGameInstance->Find_Shader_Effects(TEXT("FX_POSTEX")), nullptr);
	//if (nullptr == m_pShader) {
	//	return E_FAIL;
	//}

	//m_pRect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	//if (nullptr == m_pRect) {
	//	return E_FAIL;
	//}
	return S_OK;
}

void CMouse_Manager::Update()
{
	if (GetForegroundWindow() == m_ghWnd) {
		/* 뷰포트 마우스 좌표*/
		GetCursorPos(&m_ptCurrentMouseCur);

		m_vMove = { _float(m_ptCurrentMouseCur.x - m_ptOldMouseCur.x), _float(m_ptCurrentMouseCur.y - m_ptOldMouseCur.y) , 0.f };
		if (m_bLockMouseToCenter) {
			Lock_MouseToCenter();
		}
		m_ptOldMouseCur = m_ptCurrentMouseCur;

#ifdef _DEBUG
		GUI::Begin("Mouse_Manager");
		GUI::Text("AccumulateMomentum %f, %f", m_vMove.x, m_vMove.y);
		GUI::Text("CurPos %d, %d", m_ptCurrentMouseCur.x, m_ptCurrentMouseCur.y);
		GUI::Text("Lock %d", m_bLockMouseToCenter);
		POINT pt = Get_MouseViewPortPos();
		ImGuiIO& io = ImGui::GetIO();
		GUI::Text("IMGUI MousePos: %.1f, %.1f", io.MousePos.x, io.MousePos.y);
		//io.MousePos = ImVec2((float)pt.x, (float)pt.y);
		GUI::Text("VPPOS: %d, %d", pt.x, pt.y);
		pt = Get_MouseScreenPos();
		GUI::Text("SCPOS: %d, %d", pt.x, pt.y);

		pt = Get_MouseViewPortPos();
		D3D11_VIEWPORT	viewPort;
		_uint TargetViewPort = 1;
		m_pContext->RSGetViewports(&TargetViewPort, &viewPort);
		GUI::Text("INV_VPPOS: %f, %f", pt.x - (viewPort.Width * 0.5f), pt.y - (viewPort.Height * 0.5f));

		GUI::End();
#endif
		Picking();
	}
	else {
		m_vMove = { 0.f, 0.f, 0.f };
		ZeroMemory(&m_ptCurrentMouseCur, sizeof(POINT));
		ZeroMemory(&m_ptOldMouseCur, sizeof(POINT));
	}
}

POINT CMouse_Manager::Get_MouseViewPortPos()
{
	POINT ptViewportPos = m_ptCurrentMouseCur;
	ScreenToClient(m_ghWnd, &ptViewportPos);
	return ptViewportPos;
}

POINT CMouse_Manager::Get_MouseScreenPos()
{
	return m_ptCurrentMouseCur;
}

void CMouse_Manager::Picking()
{
	_float4  vMousePos = {};
	POINT  vViewPortMousePos = Get_MouseViewPortPos();

	/* 뷰포트 좌표를 투영 스페이스 좌표로 바꾸는 작업 */
	vMousePos.x = vViewPortMousePos.x / (m_iWinCX * 0.5f) - 1.f;
	vMousePos.y = vViewPortMousePos.y / -(m_iWinCY * 0.5f) + 1.f;
	vMousePos.z = 0.f; // 근평면상의 좌표이기 때문에 0
	vMousePos.w = 1.f; // W 나누기를 수행한 상태이기 때문에 1

	/* 투영스페이스 좌표 -> 뷰스페이스 좌표*/
	_matrix InverseProjectionMatrix = XMMatrixInverse(nullptr, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));
	XMStoreFloat4(&vMousePos, XMVector4Transform(XMLoadFloat4(&vMousePos), InverseProjectionMatrix));


	/* 뷰스페이스 좌표  -> 월드 스페이스 좌표*/
	_float3    vRayPos, vRayDir = {};

	vRayPos = _float3{ 0.f,0.f,0.f }; // 뷰스페이스 상에서 카메라의 위치는 0,0,0
	vRayDir = _float3{ (_float)vMousePos.x, (_float)vMousePos.y, (_float)vMousePos.z }; // 레이의 방향벡터는 마우스위치에서 카메라 위치 뺀 값

	_matrix InverseViewSpaceMatrix = XMMatrixInverse(nullptr, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));

	XMStoreFloat3(&m_vRayPos[ENUM_CLASS(RAY::WORLD)], XMVector4Transform(XMVectorSetW(XMLoadFloat3(&vRayPos), 1.f), InverseViewSpaceMatrix));
	XMStoreFloat3(&m_vRayDir[ENUM_CLASS(RAY::WORLD)], XMVector3Normalize(XMVector4Transform(XMVectorSetW(XMLoadFloat3(&vRayDir), 0.f), InverseViewSpaceMatrix)));
}

HRESULT CMouse_Manager::Ray_WorldToLocal(_fmatrix* InvWorldMatrix)
{
	if (InvWorldMatrix == nullptr)
		return E_FAIL;

	XMStoreFloat3(&m_vRayPos[ENUM_CLASS(RAY::LOCAL)], XMVector4Transform(XMVectorSetW(XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::WORLD)]), 1.f), *InvWorldMatrix));
	XMStoreFloat3(&m_vRayDir[ENUM_CLASS(RAY::LOCAL)], XMVector3Normalize(XMVector4Transform(XMVectorSetW(XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::WORLD)]), 0.f), *InvWorldMatrix)));

	return S_OK;
}

_bool CMouse_Manager::MousePicking_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut)
{
	_bool isCollision;

	_float fU, fV, fDist = {};

	isCollision = CMyTools::IntersectTri(XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::LOCAL)]), XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::LOCAL)]),
		XMLoadFloat3(&vPointA), XMLoadFloat3(&vPointB), XMLoadFloat3(&vPointC), fDist, fU, fV);

	if (isCollision == true)
	{
		_float3 vResult = { m_vRayPos[ENUM_CLASS(RAY::LOCAL)].x + (m_vRayDir[ENUM_CLASS(RAY::LOCAL)].x * fDist),
							m_vRayPos[ENUM_CLASS(RAY::LOCAL)].y + (m_vRayDir[ENUM_CLASS(RAY::LOCAL)].y * fDist),
							m_vRayPos[ENUM_CLASS(RAY::LOCAL)].z + (m_vRayDir[ENUM_CLASS(RAY::LOCAL)].z * fDist) };
		pOut = vResult;
	}

	return isCollision;
}

_bool CMouse_Manager::MousePicking_InWorldSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3& pOut)
{
	_bool isCollision;

	_float fU, fV, fDist = {};
	
	isCollision = CMyTools::IntersectTri(XMLoadFloat3(&m_vRayPos[ENUM_CLASS(RAY::WORLD)]), XMLoadFloat3(&m_vRayDir[ENUM_CLASS(RAY::WORLD)]),
		XMLoadFloat3(&vPointA), XMLoadFloat3(&vPointB), XMLoadFloat3(&vPointC), fDist, fU, fV);

	if (isCollision == true)
	{
		_float3 vResult = { m_vRayPos[ENUM_CLASS(RAY::WORLD)].x + (m_vRayDir[ENUM_CLASS(RAY::WORLD)].x * fDist),
							m_vRayPos[ENUM_CLASS(RAY::WORLD)].y + (m_vRayDir[ENUM_CLASS(RAY::WORLD)].y * fDist),
							m_vRayPos[ENUM_CLASS(RAY::WORLD)].z + (m_vRayDir[ENUM_CLASS(RAY::WORLD)].z * fDist) };
		pOut = vResult;
	}

	return isCollision;
}

//void CMouse_Manager::Mouse_Render()
//{
//	RECT rc{}; GetClientRect(m_ghWnd, &rc);
//	const float cw = float(rc.right - rc.left);
//	const float ch = float(rc.bottom - rc.top);
//
//	// 2) 뷰/프로젝션: 화면 좌상단(0,0) ~ (cw,ch), Y-다운
//	XMFLOAT4X4 view{}, proj{};
//	XMStoreFloat4x4(&view, XMMatrixIdentity());
//	XMStoreFloat4x4(&proj, XMMatrixOrthographicOffCenterLH(0.f, cw, ch, 0.f, 0.f, 1.f));
//
//	// 3) 마우스 좌표 (클라이언트 좌표로 변환)
//	POINT pt{}; GetCursorPos(&pt); ScreenToClient(m_ghWnd, &pt);
//
//	// 4) 월드 행렬: 30x30 스케일 + 위치(픽셀 센터 정렬: +0.5f)
//	XMFLOAT4X4 world{};
//	world._11 = 30.f;  // width
//	world._22 = 30.f;  // height
//	world._33 = 1.f;   // keep
//	world._44 = 1.f;   // keep
//	world._41 = floorf((float)pt.x) + 0.5f;  // translation X (row-vector 규약)
//	world._42 = floorf((float)pt.y) + 0.5f;  // translation Y
//
//	// 5) 바인딩
//	m_pShader->Bind_Matrix("g_WorldMatrix", &world);
//	m_pShader->Bind_Matrix("g_ViewMatrix", &view);
//	m_pShader->Bind_Matrix("g_ProjMatrix", &proj);
//
//	m_pShader->Bind_SRV("g_Texture", m_pMouseSRV);
//	m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(float));
//
//	// 6) 상태 셋업(권장)
//	// - DepthTest OFF / DepthWrite OFF (UI)
//	// - AlphaBlend ON (필요시)
//	m_pShader->Begin(2);
//	m_pRect->Bind_Resources();
//	m_pRect->Render();
//}

POINT CMouse_Manager::Get_ScreenCenterPos()
{
	D3D11_VIEWPORT	viewPort;
	_uint TargetViewPort = 1;
	m_pContext->RSGetViewports(&TargetViewPort, &viewPort);

	RECT	currRect = {};
	GetWindowRect(m_ghWnd, &currRect);
	return { currRect.left + _long(viewPort.Width * 0.5f), currRect.top + _long(viewPort.Height * 0.5f) };
}

void CMouse_Manager::Lock_MouseToCenter()
{
	m_ptCurrentMouseCur = Get_ScreenCenterPos();
	SetCursorPos(m_ptCurrentMouseCur.x, m_ptCurrentMouseCur.y);
}

CMouse_Manager* CMouse_Manager::Create(HWND _g_hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMouse_Manager* pInstance = new CMouse_Manager(_g_hWnd, pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMouse_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMouse_Manager::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
