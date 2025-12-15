#include "pch.h"
#include "TestEffect.h"

#include "GameInstance.h"


CTestEffect::CTestEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CTestEffect::CTestEffect(const CTestEffect& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CTestEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTestEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CTestEffect::Priority_Update(_float fTimeDelta)
{

}

void CTestEffect::Update(_float fTimeDelta)
{
	//m_pInstance_ModelCom->Drop(fTimeDelta);
}

void CTestEffect::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::EFFECT, this);
	}
}

HRESULT CTestEffect::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}


 
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Texture_Test_Noise"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr))) 
	{
		return E_FAIL;
	}

	CInstance_Model::INSTANCE_DESC InstanceDesc = {};

	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(1.f, 10.f);
	InstanceDesc.vRange = _float3(5.f, 0.f, 5.f);
	InstanceDesc.vSizeMin = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSizeMax = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSpeed = _float2(0.f , 5.f);

	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_GoblinBody_Instance_Model"), reinterpret_cast<CComponent**>(&m_pInstance_ModelCom), &InstanceDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_MODEL, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CTestEffect* CTestEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestEffect* pInstance = new CTestEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTestEffect::Clone(void* pArg, CGameObject* pOwner)
{
	CTestEffect* pInstance = new CTestEffect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTestEffect::Free()
{
	__super::Free();
}

void CTestEffect::Describe_Entity()
{
}
