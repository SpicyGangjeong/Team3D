#include "Light.h"
#include "pch.h"
#include "Light.h"
#include "Shader.h"
#include "VIBuffer.h"
#include "GameInstance.h"
#include "GameObject.h"


CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CLight::CLight(const CLight& rhs)
	: CComponent(rhs)
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

	return S_OK;
}

void CLight::Set_Color(_float4& vDiffuse, _float4& vAmbient, _float4& vSpecular)
{
	memcpy(&m_LightDesc.vDiffuse, &vDiffuse, sizeof(_float4));
	memcpy(&m_LightDesc.vAmbient, &vAmbient, sizeof(_float4));
	memcpy(&m_LightDesc.vSpecular, &vSpecular, sizeof(_float4));
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
		if (FAILED(pShader->Bind_RawValue("g_vLightPosOffset", &m_LightDesc.vPosOffset, sizeof(_float4)))) {
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
		if (FAILED(pShader->Bind_RawValue("g_bUsePowerLightAttenuation", &m_bUsePowerAttenuation, sizeof(_bool)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fLightAttenuationPower", &m_fLightAttenuationPower, sizeof(_float)))) {
			return E_FAIL;
		}
		iPassIndex = ENUM_CLASS(SHADER_PASS_DEFERRED::SPOT);
	}
	else {
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", m_LightDesc.pPosition, sizeof(_float4)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_vLightPosOffset", &m_LightDesc.vPosOffset, sizeof(_float4)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_bUsePowerLightAttenuation", &m_bUsePowerAttenuation, sizeof(_bool)))) {
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_fLightAttenuationPower", &m_fLightAttenuationPower, sizeof(_float)))) {
			return E_FAIL;
		}
		iPassIndex = ENUM_CLASS(SHADER_PASS_DEFERRED::POINT);
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)))) {
		return E_FAIL;
	}

	/* 라이트를 서서히 감쇄시키기 위한 감쇄값 */
	



	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4)))) {
		return E_FAIL;
	}

	_float fLightIntensity = m_fIntensityRatio * m_fLightIntensity;

	if (FAILED(pShader->Bind_RawValue("g_fLightIntensity", &fLightIntensity, sizeof(_float)))) {
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

#ifdef _DEBUG

void CLight::Describe_Entity()
{
	if (GUI::TreeNode("LIGHT_INFO"))
	{
		GUI::Separator(); GUI::Spacing();

		const char* pLightTypeNames[] = { "DIRECTIONAL", "POINT", "SPOT" };

		int iCurrentItem = static_cast<int>(m_LightDesc.eType);

		if (GUI::Combo("LightType", &iCurrentItem, pLightTypeNames, ENUM_CLASS(LIGHT::END)))
		{
			m_LightDesc.eType = static_cast<LIGHT>(iCurrentItem);
		}

	 

		GUI::ColorEdit4("Diffuse", (_float*)&m_LightDesc.vDiffuse);
		GUI::ColorEdit4("Ambient", (_float*)&m_LightDesc.vAmbient);
		GUI::ColorEdit4("Specular", (_float*)&m_LightDesc.vSpecular);
		GUI::DragFloat("Range", &m_LightDesc.fRange, 1.f, 0.f);
		GUI::DragFloat2("SpotAngle", reinterpret_cast<float*>(&m_LightDesc.vSpotAngles));
		GUI::Checkbox("UseLinearAtt", &m_bUsePowerAttenuation);
		if (true == m_bUsePowerAttenuation) {
			GUI::SameLine();
			GUI::DragFloat("Power", &m_fLightAttenuationPower, 0.01f, 0.01f);
		}

		GUI::Spacing();
		GUI::Spacing();

		GUI::DragFloat3("PosOffest", reinterpret_cast<float*>(&m_LightDesc.vPosOffset));

		GUI::Spacing();

		m_pOwner->Get_Component<CTransform>()->Describe_Entity();

		GUI::TreePop();
	}

	GUI::Separator();
}

#endif // _DEBUG
