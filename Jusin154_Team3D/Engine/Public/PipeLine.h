#pragma once

#include "Base.h"

NS_BEGIN(Engine)
/*
cascade
	set.1
	1. viewspace의 16개의 점을 구해야함
		이는 near middle far 3사각형의 교점이 됨
	2. 근평면 원평면 8개, near-middle 평면, middle-far 평면 8개 총 16개
	3. Frustum 컬링할 때 같이 넣어줘야 할 듯

	set.2
	1. 각 캐스케이드 상자 마다 메인 디렉셔널 라이트의 방향벡터에 맞춰서 바운딩 박스를 만들어야 한다.
	2. 해당 캐스케이드 바운딩박스는 최대한 셰도우 포인트에 맞춰서 세팅되어야 한다.
	3. 왜냐하면 가장 작으면서도 셰도우 포인트를 포함하고 있는 박스로 직교투영하게 되면 가장 고해상도의 캐스케이드 그림자를 얻을 수 있다.
	4. 이제 이 바운딩 박스로 오브젝트들이 어느 캐스케이드 셰도우 맵에 할당되는지 정해진다.

*/
class CPipeLine final : public CBase
{
private:
	CPipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPipeLine() = default;
public:
	void Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix);
	const _float4x4* Get_Transform_Float4x4(D3DTS eState);
	_matrix Get_Transform_Matrix(D3DTS eState);
	_matrix Get_ShadowTransform_Matrix(D3DTS eState, SHADOW eShadowType);

	const _float4* Get_CamPosition();
	const _vector Get_CamXMPosition();

	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool IsIn_WorldFrustum(_fvector vWorldPos, _float fRadius);
	_bool IsIn_LocalFrustum(_fvector vLocalPos, _float fRadius);

	pair<_bool, _ubyte> IsIn_ShadowViewFrustum(_fvector vWorldCenter, _float fRadius);

	HRESULT Bind_CascadeSplitRatio(class CShader* pShader, const _char* pConstantName, _bool bNear);
	HRESULT Bind_CascadeBias(class CShader* pShader, const _char* pConstantName);

	HRESULT Bind_GlobalSRV(class CShader* pShader, const _tchar* wszKeyGlobalSRV, const _char* pConstantName);
	HRESULT Load_GlobalSRV(const _tchar* wszKeyGlobalSRV, filesystem::path pathSRVFolder);

	HRESULT Begin_OutLine_Write(_uint iDSSRef);
	HRESULT End_OutLine_Write();

	HRESULT Ready_Shadow_Light(const _float4& vShadowDirRPYQuat);
	HRESULT Bind_Shadow_Resource(class CShader* pShader, const _char* pConstantName, D3DTS eType, SHADOW eShadowType) const;
	const _float4x4* Get_ShadowMatricesPtr(_uint iShadowBoxIndex);
	_float Get_ShadowBoxFar(_uint iShadowBoxIndex);
public:
	void Update(); // Objects의 Priority, Update사이에서 불려야만 함

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	_float4x4	m_TransformStateMatrices[ENUM_CLASS(D3DTS::END)] = {};
	_float4x4	m_ShadowTransformStateMatrices[3][ENUM_CLASS(D3DTS::END)] = {};
	_float4		m_vCamPosition = {};

	_float		m_fShadowNearBoxRatio = { 0.07f };
	_float		m_fShadowFarBoxRatio = { 0.15f };
	_float		m_fSafe_RadiusMultiplier = { 2.3f };
	_float		m_fSafe_RadiusMargin = { 10.f };
	_float3		m_vShadowBoxMarginMin = { 0.f, 0.f, -25.f };
	_float3		m_vShadowBoxMarginMax = { 0.f, 0.f, 25.f };

	_float4		m_vShadowBias = { 0.0018f, 0.0018f, 0.0018f, 0.0018f };

	_float4		m_vOriginalRenderFrustumPoints[8] = {};
	_float4		m_vOriginalShadowFrustumPoints[16] = {};// 3개로 나눈 절두체의 부분을 나타내는 16개의 점
	_float4		m_vWorldPoints[8] = {};
	_float4		m_vShadowViewPoints[16] = {}; // 라이트 정렬 공간
	_float4		m_vWorldPlanes[6] = {};
	_float4		m_vLocalPlanes[6] = {};

	_float4 m_vNearShadowViewBoxPlane[6] = {};
	_float4 m_vMiddleShadowViewBoxPlane[6] = {};
	_float4 m_vFarShadowViewBoxPlane[6] = {};

	_float4 m_vLocalNearShadowViewBoxPlane[6] = {};
	_float4 m_vLocalMiddleShadowViewBoxPlane[6] = {};
	_float4 m_vLocalFarShadowViewBoxPlane[6] = {};
	_float4 m_vShadowDirectionalRPYQuat = { 0.f, 0.f, 0.f, 1.f };
	_float4 m_vShadowInvDirectionalRPYQuat = { 0.f, 0.f, 0.f, 1.f };

	unordered_map<_wstring, ID3D11ShaderResourceView*> m_mapGlobalSRV = {};
	ID3D11DepthStencilState*	m_pDSS_OutLineWrite = { nullptr };
	ID3D11DepthStencilState*	m_pPrevDSS = { nullptr };
	_uint						m_iPrevDSSRef = { };

private:
	HRESULT Initialize();
	void Make_Planes(const _float4* pPoints, _float4* pPlanes);
	void Make_LightBoxes();
	void Get_CascadePoints8(const _float4* shadowViewPoints16, _uint cascadeIndex, _float4* outPoints8); // 
	void Update_ShadowDepthNdcZ();
	void Adjust_ShadowTexcel(_float& fMinX, _float& fMinY, _float& fMaxX, _float& fMaxY, _uint iShadowWidth, _uint iShadowHeight);
	ID3D11ShaderResourceView* Find_GlobalShaderResourceView(const _tchar* wszKeyGlobalSRV);

public:
	static CPipeLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity();
#endif // _DEBUG

};

NS_END
