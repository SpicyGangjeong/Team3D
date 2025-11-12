#include "pch.h"
#include "EffectObject.h"
#include "GameInstance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
	:CGameObject(rhs)
{
}

void CEffectObject::Priority_Update(_float fTimeDelta)
{
}

void CEffectObject::Update(_float fTimeDelta)
{
	
}

void CEffectObject::Late_Update(_float fTimeDelta)
{

}

HRESULT CEffectObject::Render()
{
	if (FAILED(Bind_ShaderResources())) 
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pInstance_ModelCom->Get_NumMeshes(); i++)
	{

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_INSTANCE_MODEL::NON_NOMALMAP)))) {
			return E_FAIL;
		}

		if (FAILED(m_pInstance_ModelCom->Bind_CS_Output(5, 1)))
			return E_FAIL;


		if (FAILED(m_pInstance_ModelCom->Render(i)))
		{
			return E_FAIL;
		}


	}

	return S_OK;
}

HRESULT CEffectObject::Render_Blur()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBlurIntensity", &m_fBlurIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pInstance_ModelCom->Get_NumMeshes(); i++)
	{

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_INSTANCE_MODEL::BLUR)))) {
			return E_FAIL;
		}

		if (FAILED(m_pInstance_ModelCom->Bind_CS_Output(5, 1)))
			return E_FAIL;


		if (FAILED(m_pInstance_ModelCom->Render(i)))
		{
			return E_FAIL;
		}

	}



	return S_OK;
}

HRESULT CEffectObject::Bind_ShaderResources()
{
	
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDiffuse", &m_isDiffuse, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMasking", &m_isMasking, sizeof(_bool)))) {
		return E_FAIL;						
	}										

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDissolve", &m_isDissolve, sizeof(_bool)))) {
		return E_FAIL;						
	}										

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isNoise", &m_isNoise, sizeof(_bool)))) {
		return E_FAIL;						
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isDiffuseUVMove", &m_isDiffuseUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMaskUVMove", &m_isMaskUVMove, sizeof(_bool)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)))) {
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &m_vEmissive, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fColorOption", &m_fColorOption, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveCutAlpha", &m_fEmissiveCutAlpha, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseDistortionIntensity", &m_fNoiseDistortionIntensity, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseUVGainAmount", &m_vDiffuseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskingUVGainAmount", &m_vMaskingUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseNoiseUVGainAmount", &m_vDiffuseNoiseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskNoiseUVGainAmount", &m_vMaskNoiseUVGainAmount, sizeof(_float2)))) {
		return E_FAIL;
	}
	
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVCutting", &m_vUVCutting, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (m_pDiffuse_TextureCom != nullptr)
	{
		if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pNoise_TextureCom != nullptr)
	{
		if (FAILED(m_pNoise_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pMasking_TextureCom != nullptr)
	{
		if (FAILED(m_pMasking_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskingTexture", 0))) {
			return E_FAIL;
		}
	}

	if (m_pDissolve_TextureCom != nullptr)
	{
		if (FAILED(m_pDissolve_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0))) {
			return E_FAIL;
		}
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}




	return S_OK;
}

HRESULT CEffectObject::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffectObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}



void CEffectObject::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pNoise_TextureCom);
	SAFE_RELEASE(m_pMasking_TextureCom);
	SAFE_RELEASE(m_pDissolve_TextureCom);

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pInstance_ModelCom);
}


