#include "pch.h"
#include "Volumetric.h"
#include "GameInstance.h"
#include "RenderEventDebugger.h"

CVolumetric::CVolumetric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CVolumetric::Initialize()
{
	if (FAILED(CreateBlob()))
		return E_FAIL;

	if (FAILED(CreateComputeShader()))
		return E_FAIL;

	if (FAILED(CreateBuffers()))
		return E_FAIL;

	if(FAILED(CreateResourceViews()))
		return E_FAIL;

	return S_OK;
}

void CVolumetric::Update()
{

}

void CVolumetric::Cumpute_Volume()
{
	EVENTSCOPE_("Render_Cumpute_Volume");

	D3D11_MAPPED_SUBRESOURCE ConstantSubResource = {};

	if (FAILED(m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantSubResource)))
	{
		MSG_BOX("Failed to Map : CVolumetric");
	}
	else
	{
		CS_VOLUME_CONSTANT_DESC* pDesc = static_cast<CS_VOLUME_CONSTANT_DESC*>(ConstantSubResource.pData);

		pDesc->VolumeSize = { 160, 90, 128 };
		pDesc->fDepthPackExponent = m_fDepthPackExponent;

		pDesc->fTemporalAccum = 0.f;
		pDesc->fFrameIndex = 0.f;

		pDesc->fNear = m_pGameInstance->Get_CameraNear();
		pDesc->fFar = *(m_pGameInstance->Get_CurrentCameraFar());

		memcpy(&pDesc->vCameraPosition, m_pGameInstance->Get_CamPosition(), sizeof(_float3));
		pDesc->fDensity = m_fDensity;
		pDesc->fLightIntensity = m_fLightIntensity;

		const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light_Info(m_pGameInstance->Get_CurrentLevelID(), 0);
		if(nullptr != pLightDesc)
		{
			memcpy(&pDesc->vLightDirection, pLightDesc->pDirection, sizeof(_float3));
			pDesc->vLightColor = pLightDesc->vDiffuse;
		}
		else
			pDesc->vLightDirection = _float3(-0.5f, -0.5f, 0.f);

		XMStoreFloat4x4(&pDesc->InvProjMatrix, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ_INV));
		XMStoreFloat4x4(&pDesc->InvViewMatrix, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW_INV));

		pDesc->fAsymmetryParameter = m_fAsymmetryParameter;
		pDesc->padding0 = 0.f;
		pDesc->padding1 = 0.f;
		pDesc->padding2 = 0.f;

		m_pContext->Unmap(m_pConstantBuffer, 0);
	}

	//컴퓨트 쉐이더 바인딩
	m_pContext->CSSetShader(m_pCSVolumeCalculate, nullptr, 0);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//SRV, UAV 바인딩
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pVolumeUAV, nullptr);

	//컴퓨트 쉐이더 실행
	XMUINT3 vGroupCount = { (160 + 7) / 8, (90 + 7) / 8, (128 + 3) / 4 };
	m_pContext->Dispatch((_uint)vGroupCount.x, (_uint)vGroupCount.y, (_uint)vGroupCount.z);

	//m_pContext->CopyResource(ppBuffers[i], m_pOutputBuffer[i]);


	ID3D11ShaderResourceView* nullSRV = { nullptr };
	ID3D11UnorderedAccessView* nullUAV = { nullptr };
	// Reset
	//m_pContext->CSSetShaderResources(0, 1, &nullSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	m_pContext->CSSetShader(nullptr, nullptr, 0);
}

void CVolumetric::Accumulate_Volume()
{
	EVENTSCOPE_("Render_Accumulate_Volume");
	 
	//컴퓨트 쉐이더 바인딩
	m_pContext->CSSetShader(m_pCSVolumeAccumulate, nullptr, 0);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//SRV, UAV 바인딩
	m_pContext->CSSetShaderResources(0, 1, &m_pVolumeSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pAccVolumeUAV, nullptr);

	//컴퓨트 쉐이더 실행
	XMUINT3 vGroupCount = { (160 + 7) / 8, (90 + 7) / 8, (128 + 3) / 4 };
	m_pContext->Dispatch((_uint)vGroupCount.x, (_uint)vGroupCount.y, (_uint)vGroupCount.z);

	ID3D11ShaderResourceView* nullSRV = { nullptr };
	ID3D11UnorderedAccessView* nullUAV = { nullptr };
	// Reset
	m_pContext->CSSetShaderResources(0, 1, &nullSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
	m_pContext->CSSetShader(nullptr, nullptr, 0);
}

HRESULT CVolumetric::Dispatch()
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	Cumpute_Volume();

	Accumulate_Volume();

	return S_OK;

}

CVolumetric* CVolumetric::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVolumetric* pInstance = new CVolumetric(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CVolumetric");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVolumetric::Free()
{
	__super::Free();
	
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pLightInfoBuffer);

	SAFE_RELEASE(m_pAccVolumeUAV);
	SAFE_RELEASE(m_pAccVolumeSRV);
	SAFE_RELEASE(m_pVolumeUAV);
	SAFE_RELEASE(m_pVolumeSRV);
	SAFE_RELEASE(m_pTexture3D);
	SAFE_RELEASE(m_pAccumulateTexture3D);

	SAFE_RELEASE(m_pCSBlobCalculate);
	SAFE_RELEASE(m_pCSVolumeCalculate);
	SAFE_RELEASE(m_pCSVolumeAccumulate);
	SAFE_RELEASE(m_pCSBlobAccumulate);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}

#ifdef _DEBUG
void CVolumetric::Describe_Entity()
{
	_bool open = true;
	GUI::Begin("SYSTEM", &open, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("VOLUMETRIC")) {
		GUI::SliderFloat("Density", &m_fDensity,0.01f, 5.f);
		GUI::SliderFloat("LightIntensity", &m_fLightIntensity, 0.01f, 5.f);
		GUI::SliderFloat("AsymmetryParameter", &m_fAsymmetryParameter, 0.01f, 5.f);
		GUI::SliderFloat("DepthPackExponent", &m_fDepthPackExponent, 0.01f, 5.f);
	}
	GUI::End();

	
}
#endif // _DEBUG

HRESULT CVolumetric::CreateComputeShader()
{
	if (FAILED(m_pDevice->CreateComputeShader(
		m_pCSBlobCalculate->GetBufferPointer(),				// 컴파일된 쉐이더 코드
		m_pCSBlobCalculate->GetBufferSize(),				// 코드 길이
		nullptr,											// 클래스 인스턴스 (없음)
		&m_pCSVolumeCalculate								// 컴퓨트 쉐이더 객체
	)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateComputeShader(
		m_pCSBlobAccumulate->GetBufferPointer(),
		m_pCSBlobAccumulate->GetBufferSize(),
		nullptr,                               
		&m_pCSVolumeAccumulate
	)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolumetric::CreateBlob()
{
	UINT HLSLFlags = {};
#ifdef _DEBUG
	HLSLFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	HLSLFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

	if(FAILED(D3DCompileFromFile(TEXT("../Bin/Resources/ShaderFiles/Shader_Volumetric_Compute.hlsl"),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS_MAIN",
		"cs_5_0",
		HLSLFlags,
		0,
		&m_pCSBlobCalculate,
		nullptr)))
		return E_FAIL;

	if (FAILED(D3DCompileFromFile(TEXT("../Bin/Resources/ShaderFiles/Shader_Volumetric_Accumulate.hlsl"),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"CS_MAIN",
		"cs_5_0",
		HLSLFlags,
		0,
		&m_pCSBlobAccumulate,
		nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVolumetric::CreateBuffers()
{
	//컨스턴트 버퍼 생성
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CS_VOLUME_CONSTANT_DESC);

	D3D11_SUBRESOURCE_DATA	CBInitialDesc = {};

	if (FAILED(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE ConstantSubResource = {};

	if (FAILED(m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantSubResource)))
	{
		MSG_BOX("Failed to Map : CVolumetric");
	}
	else
	{
		CS_VOLUME_CONSTANT_DESC* pDesc = static_cast<CS_VOLUME_CONSTANT_DESC*>(ConstantSubResource.pData);

		pDesc->VolumeSize = { 160, 90, 128 };
		pDesc->fDepthPackExponent = m_fDepthPackExponent;

		pDesc->fTemporalAccum = 0.f;
		pDesc->fFrameIndex = 0.f;

		pDesc->fNear = m_pGameInstance->Get_CameraNear();
		pDesc->fFar = *(m_pGameInstance->Get_CurrentCameraFar());

		memcpy(&pDesc->vCameraPosition, m_pGameInstance->Get_CamPosition(), sizeof(_float3));
		pDesc->fDensity = m_fDensity;
		pDesc->fLightIntensity = m_fLightIntensity;

		// GetLingtInfo   ....................
		pDesc->vLightDirection = _float3(0.f, 0.577f, 0.f);
		pDesc->vLightColor = _float4(1.f, 1.f, 1.f, 1.f);
	
		XMStoreFloat4x4(&pDesc->InvProjMatrix, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ_INV));
		XMStoreFloat4x4(&pDesc->InvViewMatrix, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW_INV));

		pDesc->fAsymmetryParameter = 0.0f;
		pDesc->padding0 = 0.f;
		pDesc->padding1 = 0.f;
		pDesc->padding2 = 0.f;

		m_pContext->Unmap(m_pConstantBuffer, 0);
	}

	return S_OK;
}

HRESULT CVolumetric::CreateResourceViews()
{
	D3D11_TEXTURE3D_DESC	TextureDesc{};

	TextureDesc.Width = 160;
	TextureDesc.Height = 90;
	TextureDesc.Depth = 128; // 64 or 128
	TextureDesc.MipLevels = 1;
	TextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture3D(&TextureDesc, nullptr, &m_pTexture3D))) {
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateTexture3D(&TextureDesc, nullptr, &m_pAccumulateTexture3D))) {
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture3D, nullptr, &m_pVolumeSRV))) {
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pAccumulateTexture3D, nullptr, &m_pAccVolumeSRV))) {
		return E_FAIL;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAV_Decs{};

	UAV_Decs.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UAV_Decs.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UAV_Decs.Texture3D.FirstWSlice = 0;
	UAV_Decs.Texture3D.MipSlice = 0;
	UAV_Decs.Texture3D.WSize = TextureDesc.Depth;
	
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture3D, &UAV_Decs, &m_pVolumeUAV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pAccumulateTexture3D, &UAV_Decs, &m_pAccVolumeUAV)))
		return E_FAIL;

	return S_OK;
}

