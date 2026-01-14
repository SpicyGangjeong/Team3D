#include "pch.h"
#include "Ranrok_EtherInfo.h"

#include "GameInstance.h"

CRanrok_EtherInfo::CRanrok_EtherInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CRanrok_EtherInfo::CRanrok_EtherInfo(const CRanrok_EtherInfo& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CRanrok_EtherInfo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRanrok_EtherInfo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_Ether_Info.eRenderOrder = RENDER::BLEND;

	return S_OK;
}

void CRanrok_EtherInfo::Priority_Update(_float fTimeDelta)
{

}

void CRanrok_EtherInfo::Update(_float fTimeDelta)
{
	Update_Time(fTimeDelta);
}

void CRanrok_EtherInfo::Late_Update(_float fTimeDelta)
{

}

HRESULT CRanrok_EtherInfo::Render()
{
	return S_OK;
}



void CRanrok_EtherInfo::Update_Time(_float fTimeDelta)
{

	m_ErherTimeInfo.vAnimTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vAnimTime.x >= m_ErherTimeInfo.vAnimTime.y)
	{
		m_ErherTimeInfo.vAnimTime.x = 0.f;

		m_ErherTimeInfo.vAnimIndex.x += 1.f;

		if (m_ErherTimeInfo.vAnimIndex.x > m_ErherTimeInfo.vAnimIndex.y) // 애니메이션에 끝에 다다랐다면
		{
			m_ErherTimeInfo.vAnimIndex.x = 0.f; // 인덱스 초기화

		}
	}

	m_ErherTimeInfo.vLifeTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vLifeTime.x >= m_ErherTimeInfo.vLifeTime.y)
	{
		m_ErherTimeInfo.vLifeTime.x = 0.f;
	}

	m_ErherTimeInfo.vDiffuseUVMoveTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vDiffuseUVMoveTime.x >= m_ErherTimeInfo.vDiffuseUVMoveTime.y)
	{
		m_ErherTimeInfo.vDiffuseUVMoveTime.x = 0.f;
	}

	m_ErherTimeInfo.vMaskingUVMoveTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vMaskingUVMoveTime.x >= m_ErherTimeInfo.vMaskingUVMoveTime.y)
	{
		m_ErherTimeInfo.vMaskingUVMoveTime.x = 0.f;
	}

	m_ErherTimeInfo.vNoiseUVMoveTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vNoiseUVMoveTime.x >= m_ErherTimeInfo.vNoiseUVMoveTime.y)
	{
		m_ErherTimeInfo.vNoiseUVMoveTime.x = 0.f;
	}

	m_ErherTimeInfo.vDistortionUVMoveTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vDistortionUVMoveTime.x >= m_ErherTimeInfo.vDistortionUVMoveTime.y)
	{
		m_ErherTimeInfo.vDistortionUVMoveTime.x = 0.f;
	}

	m_ErherTimeInfo.vDissolveUVMoveTime.x += fTimeDelta;

	if (m_ErherTimeInfo.vDissolveUVMoveTime.x >= m_ErherTimeInfo.vDissolveUVMoveTime.y)
	{
		m_ErherTimeInfo.vDissolveUVMoveTime.x = 0.f;
	}

}

void CRanrok_EtherInfo::Edit_Ether()
{

}

HRESULT CRanrok_EtherInfo::Bind_Ether_ShaderResources(CShader* pShader)
{

	if (FAILED(pShader->Bind_RawValue("g_fAnimIndex", &m_ErherTimeInfo.vAnimIndex.x, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vLifeTime", &m_ErherTimeInfo.vLifeTime, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vDiffuseUVMoveTime", &m_ErherTimeInfo.vDiffuseUVMoveTime, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vMaskingUVMoveTime", &m_ErherTimeInfo.vMaskingUVMoveTime, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vNoiseUVMoveTime", &m_ErherTimeInfo.vNoiseUVMoveTime, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vDistortionUVMoveTime", &m_ErherTimeInfo.vDistortionUVMoveTime, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vDissolveUVMoveTime", &m_ErherTimeInfo.vDissolveUVMoveTime, sizeof(_float2)))) {
		return E_FAIL;
	}



	if (m_Ether_Info.eRenderOrder != RENDER::BULR_MESH)
	{
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), pShader, "g_DepthTexture"))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_NormalCopy"), pShader, "g_NormalMapTexture"))) {
			return E_FAIL;
		}
	}

	_float2 vViewPortSize = m_pGameInstance->Get_ViewPortSize();

	if (FAILED(pShader->Bind_RawValue("g_fWinSizeX", &vViewPortSize.x, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fWinSizeY", &vViewPortSize.y, sizeof(_float)))) {
		return E_FAIL;
	}



	if (FAILED(pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV)))) {
		return E_FAIL;
	}

	if (m_Ether_Info.isMotionBlur == true || m_Ether_Info.eShaderPass == SHADER_PASS_INSTANCE_MODEL::DEFAULT)
	{

		if (FAILED(m_pGameInstance->Bind_PrevMatrix(pShader, "g_PrevProjMatrix", D3DTS::PROJ))) {
			return E_FAIL;
		}


		if (FAILED(m_pGameInstance->Bind_PrevMatrix(pShader, "g_PrevViewMatrix", D3DTS::VIEW))) {
			return E_FAIL;
		}

	

	}

	if (m_Ether_Info.eShaderPass == SHADER_PASS_INSTANCE_MODEL::DECAL_WB || m_Ether_Info.eShaderPass == SHADER_PASS_INSTANCE_MODEL::DECAL_BLUR)
	{
		if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Normal"), pShader, "g_NormalMapTexture"))) {
			return E_FAIL;
		}

	}

	if (FAILED(pShader->Bind_RawValue("g_isDiffuse", &m_Ether_Info.isDiffuse, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isMasking", &m_Ether_Info.isMasking, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isDissolve", &m_Ether_Info.isDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNoise", &m_Ether_Info.isNoise, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isEmissive", &m_Ether_Info.isEmissive, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isDistortion", &m_Ether_Info.isDistortion, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isDiffuseUVMove", &m_Ether_Info.isDiffuseUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isMaskUVMove", &m_Ether_Info.isMaskUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNoiseUVMove", &m_Ether_Info.isNoiseUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isReverseDissolve", &m_Ether_Info.isReverseDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isEmissiveDissolve", &m_Ether_Info.isEmissiveDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isMaskClampSample", &m_Ether_Info.isMaskClampSample, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNoiseColor", &m_Ether_Info.isNoiseColor, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNoiseAlpha", &m_Ether_Info.isNoiseAlpha, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNomalDissolve", &m_Ether_Info.isNomalDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isEmissiveDissolveReverse", &m_Ether_Info.isEmissiveDissolveReverse, sizeof(_bool)))) {
		return E_FAIL;
	}


	if (FAILED(pShader->Bind_RawValue("g_isBlurNoEmissive", &m_Ether_Info.isBlurNoEmissive, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vColor", &m_Ether_Info.vColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vEmissive", &m_Ether_Info.vEmissive, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fEmissiveStrength", &m_Ether_Info.fEmissiveStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fNoiseDistortionIntensity", &m_Ether_Info.fNoiseDistortionIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vDiffuseUVGainAmount", &m_Ether_Info.vDiffuseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vMaskingUVGainAmount", &m_Ether_Info.vMaskingUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vNoiseUVGainAmount", &m_Ether_Info.vNoiseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vDistortionTime", &m_Ether_Info.vDistortionTime, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vMaskDistortionUVGainAmount", &m_Ether_Info.vMaskDistortionUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}



	if (FAILED(pShader->Bind_RawValue("g_vUVCutting", &m_Ether_Info.vUVCutting, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vUVMaskCutting", &m_Ether_Info.vUVMaskCutting, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_iMaskMoveLerpOption", &m_Ether_Info.iMaskMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_iDiffuseMoveLerpOption", &m_Ether_Info.iDiffuseMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}


	if (FAILED(pShader->Bind_RawValue("g_iMaskDistortionMoveLerpOption", &m_Ether_Info.iMaskDistortionMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}


	if (FAILED(pShader->Bind_RawValue("g_iDiffuseDistortionMoveLerpOption", &m_Ether_Info.iDiffuseDistortionMoveLerpOption, sizeof(_int)))) {
		return E_FAIL;
	}



	if (FAILED(pShader->Bind_RawValue("g_fDiffuseAlpha", &m_Ether_Info.fDiffuseAlpha, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fSoftenExp", &m_Ether_Info.fSoftenExp, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fSoftStrength", &m_Ether_Info.fSoftStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fCoreBoost", &m_Ether_Info.fCoreBoost, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fRadius", &m_Ether_Info.fRadius, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fSoftMaskEdge", &m_Ether_Info.fSoftMaskEdge, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fSoftMask", &m_Ether_Info.fSoftMask, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fDissolveDelay", &m_Ether_Info.fDissolveDelay, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fReverseDissolveDelay", &m_Ether_Info.fReverseDissolveDelay, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vDissolveUVGainAmount", &m_Ether_Info.vDissolveUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isDissolveMove", &m_Ether_Info.isDissolveMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNoDissolveSmoothStep", &m_Ether_Info.isNoDissolveSmoothStep, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fDissolveMaskEdge", &m_Ether_Info.vDissolveValue.x, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fDissolveSoftMask", &m_Ether_Info.vDissolveValue.y, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fDissolveCutRatio", &m_Ether_Info.vDissolveValue.z, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isBlurColor", &m_Ether_Info.isBlurColor, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vBlurColor", &m_Ether_Info.vBlurColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vNoiseColor", &m_Ether_Info.vNoiseColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fEmissiveColorCut", &m_Ether_Info.fEmissiveColorCut, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isRimLight", &m_Ether_Info.isRimLight, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vRimLightColor", &m_Ether_Info.vRimLightColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fRimLightPower", &m_Ether_Info.fRimLightPower, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fRimLightStrength", &m_Ether_Info.fRimLightStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fModelDistortIntensity", &m_Ether_Info.fModelDistortIntensity, sizeof(_float)))) {
		return E_FAIL;
	}


	if (FAILED(pShader->Bind_RawValue("g_vMaskOffset", &m_Ether_Info.vMaskOffset, sizeof(_float2)))) {
		return E_FAIL;
	}


	if (FAILED(pShader->Bind_RawValue("g_vUVNoiseCutting", &m_Ether_Info.vUVNoiseCutting, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vDissolveSmoothRange", &m_Ether_Info.vDissolveSmoothRange, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fModelBlurIntensity", &m_Ether_Info.fModelBlurIntensity, sizeof(_float)))) {
		return E_FAIL;
	}




	if (FAILED(pShader->Bind_RawValue("g_isMask_G", &m_Ether_Info.isMask_G, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isMask_B", &m_Ether_Info.isMask_B, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNoise_G", &m_Ether_Info.isNoise_G, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isNoise_B", &m_Ether_Info.isNoise_B, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isDissolve_G", &m_Ether_Info.isDissolve_G, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isDissolve_B", &m_Ether_Info.isDissolve_B, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isDiffuse_R", &m_Ether_Info.isDiffuse_R, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isDiffuse_G", &m_Ether_Info.isDiffuse_G, sizeof(_bool)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_isDiffuse_B", &m_Ether_Info.isDiffuse_B, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isBlurDissolve", &m_Ether_Info.isBlurDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isNonSoftEffect", &m_Ether_Info.isNonSoftEffect, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isScreenFX", &m_Ether_Info.isScreenFX, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fBlurIntensity", &m_Ether_Info.fBlurIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_fBloomStrength", &m_Ether_Info.fBloomStrength, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_iBloomType", &m_Ether_Info.eBloomType, sizeof(_int)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isBloomDissolve", &m_Ether_Info.isBloomDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_isBloomReverseDissolve", &m_Ether_Info.isBloomReverseDissolve, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (m_pDiffuse_TextureCom != nullptr)
	{
		if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(pShader, "g_DiffuseTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pNoise_TextureCom != nullptr)
	{
		if (FAILED(m_pNoise_TextureCom->Bind_ShaderResource(pShader, "g_NoiseTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pMasking_TextureCom != nullptr)
	{
		if (FAILED(m_pMasking_TextureCom->Bind_ShaderResource(pShader, "g_MaskingTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pDissolve_TextureCom != nullptr)
	{
		if (FAILED(m_pDissolve_TextureCom->Bind_ShaderResource(pShader, "g_DissolveTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pEmissive_TextureCom != nullptr)
	{
		if (FAILED(m_pEmissive_TextureCom->Bind_ShaderResource(pShader, "g_EmissiveTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pDistortion_TextureCom != nullptr)
	{
		if (FAILED(m_pDistortion_TextureCom->Bind_ShaderResource(pShader, "g_DistortionTexture", 0))) {
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CRanrok_EtherInfo::Ether_Begin(CShader* pShader)
{
	if(FAILED(pShader->Begin(ENUM_CLASS(m_Ether_Info.eShaderPass))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRanrok_EtherInfo::Ready_Components()
{
	return S_OK;
}

HRESULT CRanrok_EtherInfo::Bind_ShaderResources()
{

	return S_OK;
}

HRESULT CRanrok_EtherInfo::Load_Info(const _char* pFilePath, LEVEL eLevel)
{

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pDissolve_TextureCom);
	SAFE_RELEASE(m_pEmissive_TextureCom);

	_string strPerfectFilePath = pFilePath;

	m_strPath = strPerfectFilePath;

	strPerfectFilePath += ".bin";

	HANDLE hFile = CreateFileW(
		CMyTools::ToWstring(strPerfectFilePath).c_str(),               // 파일 이름
		GENERIC_READ,              // 읽기 모드
		FILE_SHARE_READ,           // 다른 프로세스도 읽기 가능
		NULL,
		OPEN_EXISTING,             // 기존 파일 열기
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"에테르 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}



	DWORD	dwByte(0);

	if (!ReadFile(hFile, &m_Ether_Info, sizeof(EFFECT_INFO), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}

	if (m_Ether_Info.isDiffuse)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strDiffuseName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDiffuseName),
				reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}

		}
	}

	if (m_Ether_Info.isNoise)
	{
		size_t iComponentLength = {};


		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strNoiseName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strNoiseName),
				reinterpret_cast<CComponent**>(&m_pNoise_TextureCom))))
				return E_FAIL;
		}


	}

	if (m_Ether_Info.isMasking)
	{
		size_t iComponentLength = {};


		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{

			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strMaskingName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strMaskingName),
				reinterpret_cast<CComponent**>(&m_pMasking_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}


	}

	if (m_Ether_Info.isDissolve)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strDissolveName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDissolveName),
				reinterpret_cast<CComponent**>(&m_pDissolve_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}


		}


	}

	if (m_Ether_Info.isEmissive)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strEmissiveName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strEmissiveName),
				reinterpret_cast<CComponent**>(&m_pEmissive_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}


	}

	if (m_Ether_Info.isDistortion)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			m_strDistortionName = szName;

			if (FAILED(__super::Add_Asset_Component(ENUM_CLASS(eLevel), CMyTools::ToWstring(m_strDistortionName),
				reinterpret_cast<CComponent**>(&m_pDistortion_TextureCom))))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}


	}

	if (!ReadFile(hFile, &m_ErherTimeInfo, sizeof(TIME_INFO), &dwByte, nullptr)) {
		return E_FAIL;
	}

	CloseHandle(hFile);


	Set_Visible(true);

	return S_OK;
}


CRanrok_EtherInfo* CRanrok_EtherInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_EtherInfo* pInstance = new CRanrok_EtherInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_EtherInfo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRanrok_EtherInfo::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_EtherInfo* pInstance = new CRanrok_EtherInfo(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_EtherInfo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_EtherInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pDissolve_TextureCom);
	SAFE_RELEASE(m_pEmissive_TextureCom);
}
#ifdef _DEBUG


void CRanrok_EtherInfo::Describe_Entity()
{
}

#endif // _DEBUG
