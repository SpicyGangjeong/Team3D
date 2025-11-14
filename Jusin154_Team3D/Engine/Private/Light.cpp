#include "pch.h"
#include "Light.h"
#include "Shader.h"
#include "VIBuffer.h"
#include "GameInstance.h"


CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CLight::CLight(const CLight& rhs)
	: CComponent{ rhs }
{

}

_bool CLight::operator==(const CLight& rhs) const
{
	return this->operator==(rhs.m_LightDesc);
}

_bool CLight::operator==(const LIGHT_DESC& rhs) const
{
	_float4 vSrcPositon = *m_LightDesc.pPosition;
	_float4 vDstPositon = *rhs.pPosition;

	if (m_LightDesc.eType != rhs.eType ||
		vSrcPositon.x != vDstPositon.x ||
		vSrcPositon.y != vDstPositon.y ||
		vSrcPositon.z != vDstPositon.z) {
		return false;
	}

	return true;
}

HRESULT CLight::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLight::Initialize(void* pArg)
{
	LIGHT_DESC* pLightDesc = static_cast<LIGHT_DESC*>(pArg);

	if (nullptr == pLightDesc)
		return E_FAIL;

	m_LightDesc = *pLightDesc;

	m_pGameInstance->Add_Light(m_LightDesc.iLevel, this);

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer* pVIBuffer) const
{
	_uint iPassIndex = { 0 };
	if (LIGHT::DIRECTIONAL == m_LightDesc.eType) {
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", m_LightDesc.pDirection, sizeof(_float4)))) {
			return E_FAIL;
		}
		iPassIndex = ENUM_CLASS(SHADER_PASS_DEFERRED::DIRECTIONAL);
	}
	else if (LIGHT::SPOT == m_LightDesc.eType) {
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", m_LightDesc.pDirection, sizeof(_float4)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", m_LightDesc.pPosition, sizeof(_float4)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fSpotInnerAngle", &m_LightDesc.vSpotAngles.x, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fSpotOuterAngle", &m_LightDesc.vSpotAngles.y, sizeof(_float)))) {
			return E_FAIL;
		}
		iPassIndex = ENUM_CLASS(SHADER_PASS_DEFERRED::SPOT);
	}
	else {
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", m_LightDesc.pDirection, sizeof(_float4)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float)))) {
			return E_FAIL;
		}
		iPassIndex = ENUM_CLASS(SHADER_PASS_DEFERRED::POINT);
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(pShader->Begin(iPassIndex))) {
		return E_FAIL;
	}

	return pVIBuffer->Render();

	return S_OK;
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLight");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CLight::Clone(void* pArg, class CGameObject* pOwner)
{
	CLight* pInstance = new CLight(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLight");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();

}


void CLight::Describe_Entity()
{
	if (ImGui::TreeNode("LIGHT"))
	{
		ImGui::Separator(); ImGui::Spacing();

		const char* pLightTypeNames[] = { "DIRECTIONAL", "POINT", "SPOT" };

		int iCurrentItem = static_cast<int>(m_LightDesc.eType);

		if (ImGui::Combo("LightType", &iCurrentItem, pLightTypeNames, ENUM_CLASS(LIGHT::END)))
		{
			m_LightDesc.eType = static_cast<LIGHT>(iCurrentItem);
		}

		ImGui::DragFloat4("Diffuse", reinterpret_cast<float*>(&m_LightDesc.vDiffuse));
		ImGui::DragFloat4("Ambient", reinterpret_cast<float*>(&m_LightDesc.vAmbient));
		ImGui::DragFloat4("Specular", reinterpret_cast<float*>(&m_LightDesc.vSpecular));

		ImGui::TreePop();
	}

	ImGui::Separator();
}
