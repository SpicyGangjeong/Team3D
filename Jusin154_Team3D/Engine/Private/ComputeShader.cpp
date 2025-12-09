#include "pch.h"
#include "ComputeShader.h"
#include "GameInstance.h"
#include "Instance_Model.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CComputeShader::Initialize(const _tchar* pShaderFilePath, const _char* pStartFunctionName, _uint iNumElement, _uint iNumInputBuffer, _uint iNumOutputBuffer, _uint iInputStructStride[], _uint iOutputStructStride[])
{
	m_iNumElement = iNumElement;
	m_iNumInputBuffer = iNumInputBuffer;
	m_iNumOutputBuffer = iNumOutputBuffer;

	if (FAILED(CreateComputeShader(pShaderFilePath, pStartFunctionName)))
		return E_FAIL;

	if (FAILED(CreateBuffer(iNumElement, iInputStructStride, iOutputStructStride)))
		return E_FAIL;

	if (FAILED(CreateResurceViews(iNumElement, iNumInputBuffer)))
		return E_FAIL;



	return S_OK;
}

vector<D3D11_MAPPED_SUBRESOURCE> CComputeShader::Dispatch(_uint iSRVIndex, _uint iUAVIndex, _float3 vGroupCount, ID3D11Buffer** ppBuffers, ID3D11Buffer* pConstantBuffer)
{
	//VBBuffer-> dynamic , pConstantBuffer-> dynamic  , m_pInputBuffer, m_pOutputBuffer-> default , m_pStagingBuffer-> Staging

	//내 시작 버퍼 값을 채운다.

	//다이나믹으로 생성된 버퍼를 바로 디폴트 버퍼로 때려박을 수 없음
	//따라서 스테이징 버퍼를 따로 선언하여 값을 복사받는다.


	for (_uint i = 0; i < m_iNumInputBuffer; i++)
	{
		m_pContext->CopyResource(m_pInputBuffer[i], ppBuffers[i]);
	}



	//컴퓨트 쉐이더 바인딩
	m_pContext->CSSetShader(m_pComputeShader, nullptr, 0);
	m_pContext->CSSetConstantBuffers(0, 1, &pConstantBuffer);

	//SRV, UAV 바인딩
	Bind_SRV(iSRVIndex);
	Bind_UAV(iUAVIndex);

	//컴퓨트 쉐이더 실행

	//내 버퍼의 차원(1,2,3차원 배열)에 따른 그룹 카운트를 설정 << 정확히 모르겠음
	m_pContext->Dispatch((_uint)vGroupCount.x, (_uint)vGroupCount.y, (_uint)vGroupCount.z);


	//디폴트-> 스테이징 버퍼는 복사가 가능함 (다이나믹은 cpu 기반이라 복사 안됨)
	//0번 스테이징 버퍼가 아우풋 스테이징 버퍼와 같음

	vector<D3D11_MAPPED_SUBRESOURCE> StagingSubResources = {};

	for (_uint i = 0; i < m_iNumOutputBuffer; i++)
	{
		m_pContext->CopyResource(ppBuffers[i], m_pOutputBuffer[i]);
	}

	Reset();


	return StagingSubResources;
}

void CComputeShader::Bind_SRV(_uint iIndex)
{
	m_pContext->CSSetShaderResources(iIndex, // 시작슬롯 번호
		m_iNumInputBuffer,  // 버퍼 개수
		&m_pInputSRV[0]); // 버퍼 시작 주소
}

void CComputeShader::Bind_UAV(_uint iIndex)
{
	m_pContext->CSSetUnorderedAccessViews(iIndex, m_iNumOutputBuffer, &m_pOutputUAV[0], nullptr);
}

void CComputeShader::Bind_OutPut_SRV(_uint iIndex, _uint iBufferIndex)
{
	m_pContext->PSSetShaderResources(iIndex, // 시작슬롯 번호
		1,  // 버퍼 개수
		&m_pOutputSRV[iBufferIndex]); // 버퍼 시작 주

}

void CComputeShader::Bind_OutPut_SRV_VS(_uint iIndex, _uint iBufferIndex)
{
	m_pContext->VSSetShaderResources(iIndex, // 시작슬롯 번호
		1,  // 버퍼 개수
		&m_pOutputSRV[iBufferIndex]); // 버퍼 시작 주
}

ID3D11UnorderedAccessView* CComputeShader::GetOutputUAV(_uint iIndex) const
{
	return m_pOutputUAV[iIndex];
}



void CComputeShader::Reset()
{
	//다음 랜더링 패스에 연관되지 않도록 초기화

	vector<ID3D11ShaderResourceView*> pResetSRVs = {};
	vector<ID3D11UnorderedAccessView*> pResetUAVs = {};

	for (_uint i = 0; i < m_iNumInputBuffer; i++)
	{
		pResetSRVs.push_back(nullptr);
	}

	for (_uint i = 0; i < m_iNumOutputBuffer; i++)
	{
		pResetUAVs.push_back(nullptr);
	}

	m_pContext->CSSetShaderResources(0, m_iNumInputBuffer, &pResetSRVs[0]);
	m_pContext->CSSetUnorderedAccessViews(0, m_iNumOutputBuffer, &pResetUAVs[0], nullptr);
	m_pContext->CSSetShader(nullptr, nullptr, 0);
}

HRESULT CComputeShader::CreateBuffer(_uint iNumElement, _uint iInputStructStride[], _uint iOuputStructStride[])
{
	//인 아웃풋 버퍼


	for (_uint i = 0; i < m_iNumInputBuffer; i++) // 인풋 버퍼를 여러개 사용할 수 있도록 만든다
	{
		D3D11_BUFFER_DESC InputBufferdesc = {};

		InputBufferdesc.ByteWidth = iInputStructStride[i] * iNumElement;
		InputBufferdesc.StructureByteStride = iInputStructStride[i];
		InputBufferdesc.Usage = D3D11_USAGE_DEFAULT;
		InputBufferdesc.CPUAccessFlags = 0;
		InputBufferdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		InputBufferdesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		ID3D11Buffer* pInputBuffer = nullptr;

		if (FAILED(m_pDevice->CreateBuffer(&InputBufferdesc, nullptr, &pInputBuffer)))
			return E_FAIL;

		m_pInputBuffer.push_back(pInputBuffer);
	}

	for (_uint i = 0; i < m_iNumOutputBuffer; i++)
	{
		// 아웃풋 버퍼

		D3D11_BUFFER_DESC Bufferdesc = {};
		Bufferdesc.ByteWidth = iOuputStructStride[i] * iNumElement;
		Bufferdesc.StructureByteStride = iOuputStructStride[i];
		Bufferdesc.Usage = D3D11_USAGE_DEFAULT; // 인 아웃풋 버퍼는 디폴트로 설정해야함
		Bufferdesc.CPUAccessFlags = 0; // 디폴트 버퍼는 CPU에서 Read Write가 불가능하며
		//오직 Update_SubResource를 통해서만 갱신이 가능하다.

		Bufferdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		Bufferdesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		ID3D11Buffer* pOutputBuffer = nullptr;


		if (FAILED(m_pDevice->CreateBuffer(&Bufferdesc, nullptr, &pOutputBuffer)))
			return E_FAIL;

		m_pOutputBuffer.push_back(pOutputBuffer);
	}

	return S_OK;
}

HRESULT CComputeShader::CreateResurceViews(_uint iNumElement, _uint iNumInputBuffer)
{
	// SRV
	for (_uint i = 0; i < iNumInputBuffer; i++) // 인풋 버퍼를 여러개 사용할 수 있도록 만든다
	{
		ID3D11ShaderResourceView* pSrv = {};

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = iNumElement;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pInputBuffer[i], &srvDesc, &pSrv)))
			return E_FAIL;

		m_pInputSRV.push_back(pSrv);

	}

	//OUTPUT

	for (_uint i = 0; i < m_iNumOutputBuffer; i++)
	{
		//UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = iNumElement;

		ID3D11UnorderedAccessView* pOutputUAV = { nullptr };

		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pOutputBuffer[i], &uavDesc, &pOutputUAV)))
			return E_FAIL;

		m_pOutputUAV.push_back(pOutputUAV);


		//SRV

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN; // StructuredBuffer는 UNKNOWN
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.NumElements = iNumElement;
		srvDesc.Buffer.FirstElement = 0;


		ID3D11ShaderResourceView* pOutputSrv = {};

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pOutputBuffer[i], &srvDesc, &pOutputSrv)))
			return E_FAIL;

		m_pOutputSRV.push_back(pOutputSrv);
	}


	return S_OK;
}

HRESULT CComputeShader::CreateComputeShader(const _tchar* pShaderFilePath, const _char* pStartFunctionName)
{
	ID3DBlob* pCSBlob = nullptr;

	// Debug 모드와 release 모드의 플레그를 다르게 줘야함
	UINT HLSLFlags = {};
#ifdef _DEBUG
	HLSLFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	HLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

	HRESULT hr = D3DCompileFromFile(pShaderFilePath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pStartFunctionName,
		"cs_5_0",
		HLSLFlags,
		0,
		&pCSBlob,
		nullptr);


	if (FAILED(m_pDevice->CreateComputeShader(
		pCSBlob->GetBufferPointer(),         // 컴파일된 쉐이더 코드
		pCSBlob->GetBufferSize(),            // 코드 길이
		nullptr,                                // 클래스 인스턴스 (없음)
		&m_pComputeShader // 컴퓨트 쉐이더 객체
	)))
		return E_FAIL;

	SAFE_RELEASE(pCSBlob);

	return S_OK;
}


CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pStartFunctionName, _uint iNumElement, _uint iNumInputBuffer, _uint iNumOutputBuffer, _uint iInputStructStride[], _uint iOutputStructStride[])
{
	CComputeShader* pInstance = new CComputeShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pShaderFilePath, pStartFunctionName, iNumElement, iNumInputBuffer, iNumOutputBuffer, iInputStructStride, iOutputStructStride)))
	{
		MSG_BOX("Failed to Created : CComputeShader Prototype");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CComputeShader::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumInputBuffer; i++)
	{
		SAFE_RELEASE(m_pInputSRV[i]);
		SAFE_RELEASE(m_pInputBuffer[i]);
	}

	for (_uint i = 0; i < m_iNumOutputBuffer; i++)
	{
		SAFE_RELEASE(m_pOutputUAV[i]);
		SAFE_RELEASE(m_pOutputBuffer[i]);
		SAFE_RELEASE(m_pOutputSRV[i]);
	}


	SAFE_RELEASE(m_pComputeShader);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
