#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;
public:
	void Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix);
	const _float4x4* Get_Transform_Float4x4(D3DTS eState);
	_matrix Get_Transform_Matrix(D3DTS eState);

	const _float4* Get_CamPosition();
	const _vector Get_CamXMPosition();

	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse);
	_bool isIn_WorldFrustum(_fvector vWorldPos, _float fRadius);
	_bool isIn_LocalFrustum(_fvector vLocalPos, _float fRadius);

	HRESULT Bind_GlobalSRV(class CShader* pShader, const _tchar* wszKeyGlobalSRV, const _char* pConstantName);
	HRESULT Load_GlobalSRV(const _tchar* wszKeyGlobalSRV, filesystem::path pathSRVFolder);
public:
	void Update();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_float4x4	m_TransformStateMatrices[ENUM_CLASS(D3DTS::END)] = {};
	_float4		m_vCamPosition = {};

	_float4		m_vOriginalPoints[8] = {};
	_float4		m_vWorldPoints[8] = {};
	_float4		m_vWorldPlanes[6] = {};
	_float4		m_vLocalPlanes[6] = {};

	unordered_map<_wstring, ID3D11ShaderResourceView*> m_mapGlobalSRV = {};
private:
	HRESULT Initialize();
	void Make_Planes(const _float4* pPoints, _float4* pPlanes);
	ID3D11ShaderResourceView* Find_GlobalShader(const _tchar* wszKeyGlobalSRV);

public:
	static CPipeLine* Create();
	virtual void Free() override;

};

NS_END
