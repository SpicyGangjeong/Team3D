#include "pch.h"
#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect{ rhs.m_pEffect }
	, m_iNumPasses{ rhs.m_iNumPasses }
	, m_pLayouts{ rhs.m_pLayouts }
{
	SAFE_ADDREF(m_pEffect);

	for (auto& pInputLayout : m_pLayouts){
		SAFE_ADDREF(pInputLayout);
	}
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable){
		return E_FAIL;
	}

	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable) {
		return E_FAIL;
	}

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable) {
		return E_FAIL;
	}

	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable) {
		return E_FAIL;
	}

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable) {
		return E_FAIL;
	}

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable) {
		return E_FAIL;
	}

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable) {
		return E_FAIL;
	}

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** pSRVs, _uint iOffset, _uint iCount)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable) {
		return E_FAIL;
	}

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable) {
		return E_FAIL;
	}

	return pSRVariable->SetResourceArray(pSRVs, iOffset, iCount);
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses) {
		return E_FAIL;
	}

	m_pContext->IASetInputLayout(m_pLayouts[iPassIndex]);

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	UINT HLSLFlags = 0;

#ifdef _DEBUG
	HLSLFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	HLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pErrorBlob = { nullptr };
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath,
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, HLSLFlags,
		0, m_pDevice, &m_pEffect, &pErrorBlob))) {

		if (pErrorBlob) {
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}

		return E_FAIL;
	}

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique) {
		return E_FAIL;
	}

	D3DX11_TECHNIQUE_DESC TechniqueDesc = {};
	if (FAILED(pTechnique->GetDesc(&TechniqueDesc))) {
		return E_FAIL;
	}

	m_iNumPasses = TechniqueDesc.Passes;
	for (_uint i = 0; i < m_iNumPasses; ++i) {
		ID3D11InputLayout* pInputLayout = { nullptr };
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass) {
			return E_FAIL;
		}
		D3DX11_PASS_DESC PassDesc = {};
		if (FAILED(pPass->GetDesc(&PassDesc))) {
			return E_FAIL;
		}
		if (FAILED(m_pDevice->CreateInputLayout(
			pElements, iNumElements,
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
			&pInputLayout
		))) {
			return E_FAIL;
		}
		m_pLayouts.push_back(pInputLayout);
	}
	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg, CGameObject* pOwner)
{
	CShader* pInstance = new CShader(*this);
	pInstance->m_pOwner = pOwner;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pEffect);
	for (auto& iter : m_pLayouts) {
		SAFE_RELEASE(iter);
	}	
	m_pLayouts.clear();
}

void CShader::Describe_Entity()
{
}
