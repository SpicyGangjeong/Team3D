#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
	typedef struct tagShaderGlobalVariable {
		D3D_SHADER_VARIABLE_CLASS eClass = D3D_SVC_FORCE_DWORD;
		D3D_SHADER_VARIABLE_TYPE  eType = D3D_SVT_FORCE_DWORD;
		_uint iNumElement;
		_uint iNumMember;
		union {
			ID3DX11EffectVariable*				 pVariable = { nullptr };
			ID3DX11EffectScalarVariable*		 pScalar;
			ID3DX11EffectVectorVariable*         pVector;
			ID3DX11EffectMatrixVariable*         pMatrix;
			ID3DX11EffectShaderResourceVariable* pSRV;
			ID3DX11EffectSamplerVariable*        pSampler;
		};
		ID3D11ShaderResourceView*				pBeforeBindedSRV = nullptr;
		vector<ID3D11ShaderResourceView*>		beforeBindedSRVs;
	}SHADERVARIABLE;

private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);

	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumSRVs);
	HRESULT Bind_IntArray(const _char* pConstantName, const _int* pData, _uint elementCount);

private:
	D3DX11_EFFECT_DESC m_EffectDesc = {};
	unordered_map<uint64_t, SHADERVARIABLE>* m_umapShaderRawVariables = {};

private:
	HRESULT CreateShader(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	HRESULT Hash_Variables();

	SHADERVARIABLE* Find_VariableByName(const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Shader_Refresh();
#endif

public:
	HRESULT Begin(_uint iPassIndex);

private:
	ID3DX11Effect*					m_pEffect = { nullptr };
	_uint							m_iNumPasses = {};
	vector<ID3D11InputLayout*>		m_InputLayouts;


#ifdef _DEBUG
private:
	_wstring						m_strShaderPath = {};
	const D3D11_INPUT_ELEMENT_DESC* m_pElements = nullptr;
	_uint							m_iNumElements = {};
#endif

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
