#include "pch.h"
#include "EditEffect.h"

#include "GameInstance.h"


CEditEffect::CEditEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CEditEffect::CEditEffect(const CEditEffect& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CEditEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CEditEffect::Priority_Update(_float fTimeDelta)
{

}

void CEditEffect::Update(_float fTimeDelta)
{

}

void CEditEffect::Late_Update(_float fTimeDelta)
{

	if (m_pInstance_ModelCom == nullptr)
		return;

	if (m_pTextureCom == nullptr)
		return;

	_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);

	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this, *vPos, m_pTransformCom->Get_Radius());
}

HRESULT CEditEffect::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_MODEL, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CEditEffect* CEditEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditEffect* pInstance = new CEditEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEditEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEditEffect::Clone(void* pArg, CGameObject* pOwner)
{
	CEditEffect* pInstance = new CEditEffect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEditEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEditEffect::Free()
{
	__super::Free();
}

void CEditEffect::Describe_Entity()
{
	//여기서 모델, 텍스쳐, 선택할 수 있도록 함

	CInstance_Model::INSTANCE_DESC InstanceDesc = {};

	InstanceDesc.iNumInstance = 100;
	InstanceDesc.vLifeTime = _float2(1.f, 10.f);
	InstanceDesc.vRange = _float3(5.f, 0.f, 5.f);
	InstanceDesc.vSizeMin = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSizeMax = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vSpeed = _float2(0.f, 5.f);

	m_pGameInstance->Asset_Description<CInstance_Model>(ENUM_CLASS(LEVEL::EFFECT), "INSTANCE_MODEL" , (CComponent**)&m_pInstance_ModelCom , &InstanceDesc , this);


	if (m_pInstance_ModelCom != nullptr)
	{
		m_pInstance_ModelCom->Describe_Entity();
	}


}
