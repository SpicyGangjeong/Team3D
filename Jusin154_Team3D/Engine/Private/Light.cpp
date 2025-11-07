#include "pch.h"
#include "Light.h"
#include "Shader.h"
#include "VIBuffer.h"

CLight::CLight()
{
}

_bool CLight::operator==(const CLight& rhs) const
{
	return this->operator==(rhs.m_LightDesc);
}

_bool CLight::operator==(const LIGHT_DESC& rhs) const
{
	if (m_LightDesc.eType != rhs.eType ||
		m_LightDesc.vPosition.x != rhs.vPosition.x ||
		m_LightDesc.vPosition.y != rhs.vPosition.y ||
		m_LightDesc.vPosition.z != rhs.vPosition.z) {
		return false;
	}
	return true;
}

HRESULT CLight::Replace_LightTo(_fvector vPosition)
{
	//GUI::Begin("Light_Position");
	//GUI::SliderFloat("fRange", (_float*)&m_LightDesc.fRange, 0.f, 30.f);
	//GUI::SliderFloat4("vDiffuse", (_float*)&m_LightDesc.vDiffuse, 0.f, 1.f);
	//GUI::SliderFloat4("vAmbient", (_float*)&m_LightDesc.vAmbient, 0.f, 1.f);
	//GUI::SliderFloat4("vSpecular", (_float*)&m_LightDesc.vSpecular, 0.f, 1.f);
	//GUI::SliderFloat3("fAngle", (_float*)&m_LightDesc.vDirection.x, -1.f, 1.f);
	//GUI::SliderFloat("fSpotInner", (_float*)&m_LightDesc.vSpotAngles.x, 0.f, 1.f);
	//GUI::SliderFloat("fSpotOutter", (_float*)&m_LightDesc.vSpotAngles.y, 0.f, 1.f);
	//GUI::End();
	if (LIGHT::POINT == m_LightDesc.eType || LIGHT::SPOT == m_LightDesc.eType) {
		XMStoreFloat4(&m_LightDesc.vPosition, vPosition);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;
	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer* pVIBuffer) const
{
	_uint iPassIndex = { 0 };
	if (LIGHT::DIRECTIONAL == m_LightDesc.eType) {
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4)))) {
			return E_FAIL;
		}
		iPassIndex = ENUM_CLASS(SHADER_PASS_DEFERRED::DIRECTIONAL);
	}
	else if (LIGHT::SPOT == m_LightDesc.eType) {
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4)))) {
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
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4)))) {
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
CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();


}
