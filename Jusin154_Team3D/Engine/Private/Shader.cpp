#include "pch.h"
#include "Shader.h"

constexpr uint64_t FNV1A64_OFFSET = 14695981039346656037ull;
constexpr uint64_t FNV1A64_PRIME = 1099511628211ull;

constexpr uint64_t Hash_FNV1a64(const char* str)
{
	uint64_t hash = FNV1A64_OFFSET;
	while (*str)
	{
		hash ^= static_cast<uint8_t>(*str++);
		hash *= FNV1A64_PRIME;
	}
	return hash;
}

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect{ rhs.m_pEffect }
	, m_iNumPasses{ rhs.m_iNumPasses }
	, m_InputLayouts{ rhs.m_InputLayouts }
	, m_umapShaderRawVariables{ rhs.m_umapShaderRawVariables }

#ifdef _DEBUG
	, m_strShaderPath { rhs.m_strShaderPath }
	, m_pElements{ rhs.m_pElements }
	, m_iNumElements{ rhs.m_iNumElements }
#endif
{
	SAFE_ADDREF(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		SAFE_ADDREF(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{


#ifdef _DEBUG
	m_strShaderPath = pShaderFilePath;	
	m_pElements = pElements;
	m_iNumElements = iNumElements;
#endif


	if (FAILED(CreateShader(pShaderFilePath, pElements, iNumElements)))
		return E_FAIL;


	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}



HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	SHADERVARIABLE* pTagVariable = Find_VariableByName(pConstantName);
	if (nullptr == pTagVariable){
		return E_FAIL;
	}

	return pTagVariable->pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	SHADERVARIABLE* pTagVariable = Find_VariableByName(pConstantName);
	if (nullptr == pTagVariable) {
		return E_FAIL;
	}

	return pTagVariable->pMatrix->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices)
{
	SHADERVARIABLE* pTagVariable = Find_VariableByName(pConstantName);
	if (nullptr == pTagVariable) {
		return E_FAIL;
	}

	return pTagVariable->pMatrix->SetMatrixArray(reinterpret_cast<const _float*>(pMatrix), 0, iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	SHADERVARIABLE* pTagVariable = Find_VariableByName(pConstantName);
	if (nullptr == pTagVariable) {
		return E_FAIL;
	}

	return pTagVariable->pSRV->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumSRVs)
{
	SHADERVARIABLE* pTagVariable = Find_VariableByName(pConstantName);
	if (nullptr == pTagVariable) {
		return E_FAIL;
	}

	return pTagVariable->pSRV->SetResourceArray(ppSRV, 0, iNumSRVs);
}

HRESULT CShader::Bind_IntArray(const _char* pConstantName, const int* pData, _uint elementCount)
{
	SHADERVARIABLE* pTagVariable = Find_VariableByName(pConstantName);
	if (nullptr == pTagVariable) {
		return E_FAIL;
	}

	return pTagVariable->pScalar->SetIntArray(pData, 0, elementCount);
}


HRESULT CShader::CreateShader(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{

	_uint		iHlslFlag = {};

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_DEBUG_NAME_FOR_SOURCE;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1 | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR ;
#endif

	ID3DBlob* pBlob;

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, &pBlob))) {
		if (pBlob) {
			OutputDebugStringA((char*)pBlob->GetBufferPointer());
			pBlob->Release();
		}
		return E_FAIL;
	}

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC	TechniqueDesc{};

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; i++)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
		if (nullptr == pPass)
			return E_FAIL;

		D3DX11_PASS_DESC	PassDesc{};
		if (FAILED(pPass->GetDesc(&PassDesc)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateInputLayout(
			pElements,
			iNumElements,
			PassDesc.pIAInputSignature/*쉐이더에서 받아줄수 있는 정점의 정보*/,
			PassDesc.IAInputSignatureSize/*쉐이더에서 받아줄수 있는 정점의 멤버변수갯수*/,

			&pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}
	if (FAILED(Hash_Variables())) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShader::Hash_Variables()
{
	if (nullptr == m_pEffect) {
		return E_FAIL;
	}
	Safe_Delete(m_umapShaderRawVariables);
	m_umapShaderRawVariables = new unordered_map<size_t, SHADERVARIABLE>;
	HRESULT hr = m_pEffect->GetDesc(&m_EffectDesc);
	if (FAILED(hr))
		return hr;
	m_umapShaderRawVariables->clear();
	m_umapShaderRawVariables->reserve(m_EffectDesc.GlobalVariables);

	for (_uint iIndexVariable = 0; iIndexVariable < m_EffectDesc.GlobalVariables; ++iIndexVariable) {
		ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByIndex(iIndexVariable);
		if (pVariable == nullptr || pVariable->IsValid() != true) {
			assert(false); // 불완전한 변수선언
			continue;
		}
		
        D3DX11_EFFECT_VARIABLE_DESC variableDesc{};
        hr = pVariable->GetDesc(&variableDesc);
        if (FAILED(hr) || variableDesc.Name == nullptr) {
            assert(false);
            continue;
        }
		SHADERVARIABLE* pTagVariable = Find_VariableByName(variableDesc.Name);
		if (nullptr != pTagVariable) {
			assert(false); // 중복된 변수정의
			continue;
		}

		LPD3D11EFFECTTYPE VariableType = pVariable->GetType();
		D3DX11_EFFECT_TYPE_DESC VariableTypeDesc = {};
		VariableType->GetDesc(&VariableTypeDesc);
		CShader::SHADERVARIABLE TagVariable = {};
		TagVariable.pVariable = pVariable;
		TagVariable.eType = VariableTypeDesc.Type;
		TagVariable.eClass = VariableTypeDesc.Class;
		TagVariable.iNumElement = VariableTypeDesc.Elements;
		TagVariable.iNumMember = VariableTypeDesc.Members;
		switch (TagVariable.eClass)
		{
		case D3D_SVC_SCALAR:
			TagVariable.pScalar = pVariable->AsScalar();
			if (TagVariable.pScalar && !TagVariable.pScalar->IsValid()) {
				TagVariable.pScalar = nullptr;
			}
			break;

		case D3D_SVC_VECTOR:
			TagVariable.pVector = pVariable->AsVector();
			if (TagVariable.pVector && !TagVariable.pVector->IsValid()) {
				TagVariable.pVector = nullptr;
			}
			break;

		case D3D_SVC_MATRIX_ROWS:
		case D3D_SVC_MATRIX_COLUMNS:
			TagVariable.pMatrix = pVariable->AsMatrix();
			if (TagVariable.pMatrix && !TagVariable.pMatrix->IsValid()) {
				TagVariable.pMatrix = nullptr;
			}
			break;

		case D3D_SVC_OBJECT:
			if (TagVariable.eType == D3D_SVT_SAMPLER ||
				TagVariable.eType == D3D_SVT_SAMPLER1D ||
				TagVariable.eType == D3D_SVT_SAMPLER2D ||
				TagVariable.eType == D3D_SVT_SAMPLER3D ||
				TagVariable.eType == D3D_SVT_SAMPLERCUBE)
			{
				TagVariable.pSampler = pVariable->AsSampler();
				if (TagVariable.pSampler && !TagVariable.pSampler->IsValid()) {
					TagVariable.pSampler = nullptr;
				}
			}
			else
			{
				TagVariable.pSRV = pVariable->AsShaderResource();
				if (TagVariable.pSRV && !TagVariable.pSRV->IsValid()) {
					TagVariable.pSRV = nullptr;
				}
			}
			break;

		default:
			assert(false);
			break;
		}

		m_umapShaderRawVariables->insert({ Hash_FNV1a64(variableDesc.Name), TagVariable });
	}


	return S_OK;
}

CShader::SHADERVARIABLE* CShader::Find_VariableByName(const _char* pConstantName)
{
	CShader::SHADERVARIABLE* pResult = { nullptr };

	unordered_map<size_t, SHADERVARIABLE>::iterator iter = m_umapShaderRawVariables->find(Hash_FNV1a64(pConstantName));
	if (m_umapShaderRawVariables->end() != iter) {
		pResult = &(*iter).second;
	}
	return pResult;
}

#ifdef _DEBUG

HRESULT CShader::Shader_Refresh()
{
	// 내 인풋 레이아웃 이펙트 소멸시키고

	Safe_Release(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	//그 경로로 재 생성한다.

	if (FAILED(CreateShader(m_strShaderPath.c_str(), m_pElements, m_iNumElements)))
	{
		MessageBox(NULL, L"쉐이더 변경 실패", L"System Message", MB_OK);
		return E_FAIL;
	}


	MessageBox(NULL, L"쉐이더 변경 성공", L"System Message", MB_OK);

	return S_OK;
}

#endif //  


HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

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

CComponent* CShader::Clone(void* pArg, class CGameObject* pOwner)
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

	if (false == m_bCloned) {
		Safe_Delete(m_umapShaderRawVariables);
	}

	SAFE_RELEASE(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		SAFE_RELEASE(pInputLayout);

	m_InputLayouts.clear();
}
#ifdef _DEBUG

void CShader::Describe_Entity()
{

	if(GUI::TreeNode("SHADER"))
	{
		if (GUI::Button("Shader Refresh"))
		{

			Shader_Refresh();

		}

		GUI::TreePop();
	}

}

#endif // _DEBUG
