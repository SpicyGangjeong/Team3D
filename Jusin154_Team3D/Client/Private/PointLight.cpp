#include "pch.h"
#include "PointLight.h"

#include "GameInstance.h"
#include "Layer.h"

CPointLight::CPointLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPointLight::CPointLight(const CPointLight& rhs)
	: CGameObject(rhs)
{
}

void CPointLight::Priority_Update(_float fTimeDelta)
{
}

void CPointLight::Update(_float fTimeDelta)
{
	m_fAttenuationTime += fTimeDelta * m_fLightIntensitySpeed;
}

void CPointLight::Late_Update(_float fTimeDelta)
{
	m_pLightCom->Set_AttPower(sinf(m_fAttenuationTime) * sinf(m_fAttenuationTime) * m_fLightIntensity + m_fLightIntensity + 1.f);
}

HRESULT CPointLight::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPointLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPointLight::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPointLight::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	LIGHT_DESC			LightDesc{};
	LightDesc.iLevel = NEXT_LEVEL;
	LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
	
	
	POINTLIGHT_DESC* pDesc = static_cast<POINTLIGHT_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	LightDesc.eType = LIGHT::POINT;
	LightDesc.vDiffuse = pDesc->vDiffuse;
	LightDesc.vAmbient = pDesc->vAmbient;
	LightDesc.vSpecular = pDesc->vSpecular;
	LightDesc.vPosOffset = _float4(pDesc->vPosOffset.x, pDesc->vPosOffset.y, pDesc->vPosOffset.z, 1.f);
	LightDesc.fRange = pDesc->fRange;

	m_fLightIntensity = pDesc->fLightAttRange;
	m_fLightIntensitySpeed = pDesc->fLightAttSpeed;

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Add_Light(NEXT_LEVEL, m_pLightCom);

	return S_OK;
}

HRESULT CPointLight::Bind_ShaderResources()
{
	return S_OK;
}

CPointLight* CPointLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPointLight* pInstance = new CPointLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPointLight");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CPointLight::Clone(void* pArg, CGameObject* pOwner)
{
	CPointLight* pInstance = new CPointLight(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPointLight");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CPointLight::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLightCom);
}

#ifdef _DEBUG
void CPointLight::Describe_Entity()
{
	
}
#endif // _DEBUG
