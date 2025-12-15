#include "pch.h"
#include "PipeLine.h"
#include "Shader.h"
#include "GameInstance.h"

CPipeLine::CPipeLine()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pGameInstance);
}

void CPipeLine::Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix)
{
	XMStoreFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)], TransformStateMatrix);
}

const _float4x4* CPipeLine::Get_Transform_Float4x4(D3DTS eState)
{
	return &m_TransformStateMatrices[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_Transform_Matrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)]);
}

const _float4* CPipeLine::Get_CamPosition()
{
	return &m_vCamPosition;
}

const _vector CPipeLine::Get_CamXMPosition()
{
	return XMLoadFloat4(&m_vCamPosition);
}

void CPipeLine::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	_float4		vLocalPoints[8] = {};
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&vLocalPoints[i],
			XMVector3TransformCoord(XMLoadFloat4(&m_vWorldPoints[i]), WorldMatrixInverse));
	}

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}

_bool CPipeLine::IsIn_WorldFrustum(_fvector vWorldPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
			return false;
	}

	return true;
}

_bool CPipeLine::IsIn_LocalFrustum(_fvector vLocalPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vLocalPos)))
			return false;
	}

	return true;
}

pair<_bool, _ubyte> CPipeLine::IsIn_ShadowViewFrustum(_fvector vWorldCenter, _float fRadius)
{
	pair<_bool, _ubyte> pairResult = { false, 0 };
	// 월드 센터를 셰도우뷰 센터로 바꿈
	_vector vShadowViewCenter = XMVectorSetW(XMVector3Rotate(vWorldCenter, XMLoadFloat4(&m_vShadowInvDirectionalRPYQuat)), 1.f);

	_float fSafeRadius = fRadius; // 인스턴싱된 객체 고려해서 좀 더 넓게 탐색

	for (_uint iCascadeIndex = 0; iCascadeIndex < ENUM_CLASS(SHADOW::END); ++iCascadeIndex) {
		const _float4* pTargetPlanes = nullptr;
		if (iCascadeIndex == 0) {
			pTargetPlanes = m_vNearShadowViewBoxPlane;
			fSafeRadius = fRadius;
		}
		if (iCascadeIndex == 1) {
			pTargetPlanes = m_vMiddleShadowViewBoxPlane;
			fSafeRadius = fRadius;
		}
		if (iCascadeIndex == 2) {
			pTargetPlanes = m_vFarShadowViewBoxPlane;
			fSafeRadius = fRadius;
		}

		_bool bIsInside = true;
		for (_uint planeIndex = 0; planeIndex < 6; ++planeIndex) {
			_vector vPlane = XMLoadFloat4(&pTargetPlanes[planeIndex]);
			_float fDistance = XMVectorGetX(XMPlaneDotCoord(vPlane, vShadowViewCenter));
			if (fSafeRadius < fDistance){
				bIsInside = false;
				break;
			}
		}
		if (true == bIsInside) {
			pairResult.first = true;
			pairResult.second |= 1 << iCascadeIndex;
		}
	}
	return pairResult;
}

HRESULT CPipeLine::Bind_CascadeSplitRatio(class CShader* pShader, const _char* pConstantName, _bool bNear)
{
	if (true == bNear) {
		if (FAILED(pShader->Bind_RawValue(pConstantName, &m_fShadowNearBoxRatio, sizeof(_float)))) {
			return E_FAIL;
		}
	}
	else {
		if (FAILED(pShader->Bind_RawValue(pConstantName, &m_fShadowFarBoxRatio, sizeof(_float)))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CPipeLine::Bind_CascadeBias(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_RawValue(pConstantName, &m_vShadowBias, sizeof(_float4));
}

HRESULT CPipeLine::Bind_GlobalSRV(CShader* pShader, const _tchar* wszKeyGlobalSRV, const _char* pConstantName)
{
	ID3D11ShaderResourceView* pSRV = Find_GlobalShaderResourceView(wszKeyGlobalSRV);
	if (FAILED(pShader->Bind_SRV(pConstantName, pSRV))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPipeLine::Load_GlobalSRV(const _tchar* wszKeyGlobalSRV, filesystem::path pathSRVFolder)
{
	ID3D11ShaderResourceView* pSRV = Find_GlobalShaderResourceView(wszKeyGlobalSRV);
	if (nullptr != pSRV) {
		return E_FAIL;
	}
	pSRV = m_pGameInstance->Add_Resource(pathSRVFolder.string().c_str());
	if (nullptr == pSRV) {
		return E_FAIL;
	}
	m_mapGlobalSRV.emplace(wszKeyGlobalSRV, pSRV);
	return S_OK;
}

HRESULT CPipeLine::Ready_Shadow_Light(const _float4& vShadowDirRPYQuat)
{
	_vector vShadowQuat = XMQuaternionNormalize(XMLoadFloat4(&vShadowDirRPYQuat));
	XMStoreFloat4(&m_vShadowDirectionalRPYQuat, vShadowQuat);
	XMStoreFloat4(&m_vShadowInvDirectionalRPYQuat, XMQuaternionInverse(vShadowQuat));
	return S_OK;
}

HRESULT CPipeLine::Bind_Shadow_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType, SHADOW eShadowType) const
{
	_uint iPass = UINT_MAX;
	if (0 < ((_ubyte)eShadowType & (_ubyte)SHADOW::SHADOW_NEAR)) {
		iPass = 0;
	}
	else if (0 < ((_ubyte)eShadowType & (_ubyte)SHADOW::SHADOW_MIDDLE)) {
		iPass = 1;
	}
	else if (0 < ((_ubyte)eShadowType & (_ubyte)SHADOW::SHADOW_FAR)) {
		iPass = 2;
	}
	else if (0 < ((_ubyte)eShadowType & (_ubyte)SHADOW::SHADOW_PRE)) {
		return m_pGameInstance->Bind_PreShadowMatrix(pShader, pConstantName, eType);
	}
	assert(iPass != UINT_MAX);

	return pShader->Bind_Matrix(pConstantName, &m_ShadowTransformStateMatrices[iPass][ENUM_CLASS(eType)]);
}

const _float4x4* CPipeLine::Get_ShadowMatricesPtr(_uint iShadowBoxIndex)
{
	return m_ShadowTransformStateMatrices[iShadowBoxIndex];
}

_float CPipeLine::Get_ShadowBoxFar(_uint iShadowBoxIndex)
{
	_float4* targetPlanes = nullptr;
	if (iShadowBoxIndex == 0) targetPlanes = m_vNearShadowViewBoxPlane;
	if (iShadowBoxIndex == 1) targetPlanes = m_vMiddleShadowViewBoxPlane;
	if (iShadowBoxIndex == 2) targetPlanes = m_vFarShadowViewBoxPlane;

	if (nullptr == targetPlanes) {
		assert(false);
		return -1.f;
	}
	return -targetPlanes[5].w; // Far
}

void CPipeLine::Update()
{
#ifdef _DEBUG
	Describe_Entity();
#endif // DEBUG
	Update_ShadowDepthNdcZ();

	XMStoreFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(D3DTS::VIEW_INV)], XMMatrixInverse(nullptr, Get_Transform_Matrix(D3DTS::VIEW)));
	XMStoreFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(D3DTS::PROJ_INV)], XMMatrixInverse(nullptr, Get_Transform_Matrix(D3DTS::PROJ)));


	memcpy(&m_vCamPosition, &m_TransformStateMatrices[ENUM_CLASS(D3DTS::VIEW_INV)].m[3], sizeof(_float4));
	_matrix ProjInvMatrix = XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(D3DTS::PROJ_INV)]);
	_matrix ViewInvMatrix = XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(D3DTS::VIEW_INV)]);
	_matrix WorldFromClipMatrix = XMMatrixMultiply(ProjInvMatrix, ViewInvMatrix);

	_vector vShadowInvQuat = XMLoadFloat4(&m_vShadowInvDirectionalRPYQuat);
	for (size_t i = 0; i < 8; ++i)
	{
		XMStoreFloat4(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_vOriginalRenderFrustumPoints[i]), WorldFromClipMatrix));
	}

	for (size_t i = 0; i < 16; ++i)
	{ // World Point To 라이트 정렬 공간
		_vector vPoint = XMVector3TransformCoord(XMLoadFloat4(&m_vOriginalShadowFrustumPoints[i]), WorldFromClipMatrix);
		vPoint = XMVectorSetW(XMVector3Rotate(vPoint, vShadowInvQuat), 1.f);
		XMStoreFloat4(&m_vShadowViewPoints[i], vPoint);
	}

	Make_Planes(m_vWorldPoints, m_vWorldPlanes);
	Make_LightBoxes();
}

HRESULT CPipeLine::Initialize()
{
	m_vOriginalShadowFrustumPoints[0] = m_vOriginalRenderFrustumPoints[0] = _float4(-1.f,	1.f,	0.f, 1.f); // LT
	m_vOriginalShadowFrustumPoints[1] = m_vOriginalRenderFrustumPoints[1] = _float4(1.f,	1.f,	0.f, 1.f); // RT
	m_vOriginalShadowFrustumPoints[2] = m_vOriginalRenderFrustumPoints[2] = _float4(1.f,	-1.f,	0.f, 1.f); // RB
	m_vOriginalShadowFrustumPoints[3] = m_vOriginalRenderFrustumPoints[3] = _float4(-1.f,	-1.f,	0.f, 1.f); // LB

	m_vOriginalShadowFrustumPoints[4] = _float4(-1.f,	1.f,	m_fShadowNearBoxRatio, 1.f); // LT
	m_vOriginalShadowFrustumPoints[5] = _float4(1.f,	1.f,	m_fShadowNearBoxRatio, 1.f); // RT
	m_vOriginalShadowFrustumPoints[6] = _float4(1.f,	-1.f,	m_fShadowNearBoxRatio, 1.f); // RB
	m_vOriginalShadowFrustumPoints[7] = _float4(-1.f,	-1.f,	m_fShadowNearBoxRatio, 1.f); // LB

	m_vOriginalShadowFrustumPoints[8] = _float4(-1.f,	1.f,	m_fShadowFarBoxRatio, 1.f); // LT
	m_vOriginalShadowFrustumPoints[9] = _float4(1.f,	1.f,	m_fShadowFarBoxRatio, 1.f); // RT
	m_vOriginalShadowFrustumPoints[10]= _float4(1.f,	-1.f,	m_fShadowFarBoxRatio, 1.f); // RB
	m_vOriginalShadowFrustumPoints[11]= _float4(-1.f,	-1.f,	m_fShadowFarBoxRatio, 1.f); // LB

	m_vOriginalShadowFrustumPoints[12] = m_vOriginalRenderFrustumPoints[4] = _float4(-1.f,	1.f,	1.f, 1.f); // LT
	m_vOriginalShadowFrustumPoints[13] = m_vOriginalRenderFrustumPoints[5] = _float4(1.f,	1.f,	1.f, 1.f); // RT
	m_vOriginalShadowFrustumPoints[14] = m_vOriginalRenderFrustumPoints[6] = _float4(1.f,	-1.f,	1.f, 1.f); // RB
	m_vOriginalShadowFrustumPoints[15] = m_vOriginalRenderFrustumPoints[7] = _float4(-1.f,	-1.f,	1.f, 1.f); // LB
	
	for (_uint i = ENUM_CLASS(D3DTS::VIEW); i < ENUM_CLASS(D3DTS::END); ++i) {
		XMStoreFloat4x4(&m_TransformStateMatrices[i], XMMatrixIdentity());
	}

	return S_OK;
}

void CPipeLine::Make_Planes(const _float4* pPoints, _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

void CPipeLine::Make_LightBoxes()
{
	// AABB Lighting space
	_vector vShadowInvQuat = XMLoadFloat4(&m_vShadowInvDirectionalRPYQuat);
	_matrix ViewMatrix = XMMatrixRotationQuaternion(vShadowInvQuat);
	_matrix ViewInvMatrix = XMMatrixInverse(nullptr, ViewMatrix);

	for (_uint iIndex = 0; iIndex < 3; ++iIndex)
	{
		_float4 vCascadePoints8[8] = {};
		Get_CascadePoints8(m_vShadowViewPoints, iIndex, vCascadePoints8);

		_float fMinX = vCascadePoints8[0].x;
		_float fMinY = vCascadePoints8[0].y;
		_float fMinZ = vCascadePoints8[0].z;

		_float fMaxX = vCascadePoints8[0].x;
		_float fMaxY = vCascadePoints8[0].y;
		_float fMaxZ = vCascadePoints8[0].z;

		for (_uint iPointIndex = 1; iPointIndex < 8; ++iPointIndex)
		{
			fMinX = min(fMinX, vCascadePoints8[iPointIndex].x);
			fMinY = min(fMinY, vCascadePoints8[iPointIndex].y);
			fMinZ = min(fMinZ, vCascadePoints8[iPointIndex].z);

			fMaxX = max(fMaxX, vCascadePoints8[iPointIndex].x);
			fMaxY = max(fMaxY, vCascadePoints8[iPointIndex].y);
			fMaxZ = max(fMaxZ, vCascadePoints8[iPointIndex].z);
		}

		_float4* targetPlanes = nullptr;
		if (iIndex == 0) targetPlanes = m_vNearShadowViewBoxPlane;
		if (iIndex == 1) targetPlanes = m_vMiddleShadowViewBoxPlane;
		if (iIndex == 2) targetPlanes = m_vFarShadowViewBoxPlane;

		targetPlanes[0] = _float4(-1.f, 0.f, 0.f, fMinX); // Left
		targetPlanes[1] = _float4(1.f, 0.f, 0.f, -fMaxX); // Right
		targetPlanes[2] = _float4(0.f, -1.f, 0.f, fMinY); // Bottom
		targetPlanes[3] = _float4(0.f, 1.f, 0.f, -fMaxY); // Top
		targetPlanes[4] = _float4(0.f, 0.f, -1.f, fMinZ); // Near
		targetPlanes[5] = _float4(0.f, 0.f, 1.f, -fMaxZ); // Far


		XMStoreFloat4x4(&m_ShadowTransformStateMatrices[iIndex][ENUM_CLASS(D3DTS::VIEW)], ViewMatrix);
		XMStoreFloat4x4(&m_ShadowTransformStateMatrices[iIndex][ENUM_CLASS(D3DTS::VIEW_INV)], ViewInvMatrix);

		_matrix ProjMatrix = XMMatrixOrthographicOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ);
		XMStoreFloat4x4(&m_ShadowTransformStateMatrices[iIndex][ENUM_CLASS(D3DTS::PROJ)], ProjMatrix);
		XMStoreFloat4x4(&m_ShadowTransformStateMatrices[iIndex][ENUM_CLASS(D3DTS::PROJ_INV)], XMMatrixInverse(nullptr, ProjMatrix));
	}

}

void CPipeLine::Get_CascadePoints8(const _float4* pShadowViewPoints16, _uint iIndexCascade, _float4* pOutPoints8)
{
	_uint iIndexNear = 0;
	_uint iIndexFar = 0;

	if (iIndexCascade == 0) { iIndexNear = 0;  iIndexFar = 4; }   // near ~ shadownear
	if (iIndexCascade == 1) { iIndexNear = 4;  iIndexFar = 8; }   // shadownear ~ shadowfar
	if (iIndexCascade == 2) { iIndexNear = 8;  iIndexFar = 12; }   // shadowfar ~ far

	for (_uint iIndex = 0; iIndex < 4; ++iIndex) {
		pOutPoints8[iIndex] = pShadowViewPoints16[iIndexNear + iIndex];
		pOutPoints8[4 + iIndex] = pShadowViewPoints16[iIndexFar + iIndex];
	}
}

void CPipeLine::Update_ShadowDepthNdcZ()
{
	_matrix ProjectionMatrix = Get_Transform_Matrix(D3DTS::PROJ);

	_float fProjMatrix22 = ProjectionMatrix.r[2].m128_f32[2];
	_float fProjMatrix32 = ProjectionMatrix.r[3].m128_f32[2];

	_float fNearPlane = -fProjMatrix32 / fProjMatrix22;
	_float fFarPlane = (fProjMatrix22 * fNearPlane) / (fProjMatrix22 - 1.0f);

	_float fShadowViewDepth0 = fNearPlane + (fFarPlane - fNearPlane) * m_fShadowNearBoxRatio;
	_float fShadowViewDepth1 = fNearPlane + (fFarPlane - fNearPlane) * m_fShadowFarBoxRatio;

	_float fNDCZ0 = { };
	{
		_vector vViewPort = XMVectorSet(0.f, 0.f, fShadowViewDepth0, 1.f);
		_vector vProj = XMVector4Transform(vViewPort, ProjectionMatrix);
		_float fProjZ = XMVectorGetZ(vProj);
		_float fProjW = XMVectorGetW(vProj);
		fNDCZ0 = (fProjZ / fProjW);
	}
	_float fNDCZ1 = { };
	{
		_vector vViewPort = XMVectorSet(0.f, 0.f, fShadowViewDepth1, 1.f);
		_vector vProj = XMVector4Transform(vViewPort, ProjectionMatrix);
		_float fProjZ = XMVectorGetZ(vProj);
		_float fProjW = XMVectorGetW(vProj);
		fNDCZ1 = (fProjZ / fProjW);
	}

	fNDCZ0 = clamp(fNDCZ0, 0.0f, 1.0f);
	fNDCZ1 = clamp(fNDCZ1, 0.0f, 1.0f);

	for (size_t iIndex = 0; iIndex < 4; ++iIndex)
	{
		m_vOriginalShadowFrustumPoints[4 + iIndex].z = fNDCZ0;
		m_vOriginalShadowFrustumPoints[8 + iIndex].z = fNDCZ1;
	}
}

ID3D11ShaderResourceView* CPipeLine::Find_GlobalShaderResourceView(const _tchar* wszKeyGlobalSRV)
{
	auto iter = m_mapGlobalSRV.find(wszKeyGlobalSRV);
	if (iter == m_mapGlobalSRV.end()) {
		return nullptr;
	}
	return (*iter).second;
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CPipeLine::Free()
{
	__super::Free();
	for (auto& pairSRV : m_mapGlobalSRV) {
		SAFE_RELEASE(pairSRV.second);
	}
	m_mapGlobalSRV.clear();
	SAFE_RELEASE(m_pGameInstance);
}
#ifdef _DEBUG

void CPipeLine::Describe_Entity()
{
	GUI::Begin("SYSTEM");
	if (GUI::CollapsingHeader("PipeLine")) {
		_bool bModified = { false };
		_float fNear = m_fShadowNearBoxRatio;
		_float fFar = m_fShadowFarBoxRatio;
		_float4 vShadowBias = m_vShadowBias;
		if (GUI::DragFloat("m_fShadowNearBoxRatio", &fNear, 0.01f, 0.01f, 0.999f, "%.2f")) {
			bModified = true;
		}
		if (GUI::DragFloat("m_fShadowFarBoxRatio", &fFar, 0.01f, 0.01f, 0.999f, "%.2f")) {
			bModified = true;
		}
		GUI::PushItemWidth(150.f);
		if (GUI::DragFloat4("ShadowBiasNMFS", (_float*)&vShadowBias, 0.0001f, 0.0001f, 1.f, "%.4f")) {
			bModified = true;
		}
		GUI::PushItemWidth(80.f);
		if (true == bModified) {
			m_fShadowNearBoxRatio = fNear;
			m_fShadowFarBoxRatio = fFar;
			m_vShadowBias = vShadowBias;
		}
	}
	GUI::End();
}

#endif // _DEBUG
