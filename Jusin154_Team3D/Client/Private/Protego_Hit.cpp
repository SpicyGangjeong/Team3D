#include "pch.h"
#include "Protego_Hit.h"

#include "GameInstance.h"
#include "EffectParts.h"


CProtego_Hit::CProtego_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CProtego_Hit::CProtego_Hit(const CProtego_Hit& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CProtego_Hit::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/ProtegoHit")))
		return E_FAIL;

	return S_OK;

}

HRESULT CProtego_Hit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"ProtegoHit";

	m_pHit_SphereLay = Get_PartObject<CEffectParts>("ProtegoHit_Lay");
	m_pHit_Light = Get_PartObject<CEffectParts>("Hit_Light");

	SAFE_ADDREF(m_pHit_SphereLay);
	SAFE_ADDREF(m_pHit_Light);
	

	m_fDuration = 3.f;

	_float3 vScale = _float3(1.1f, 1.1f, 1.1f);

	m_pHit_SphereLay->Get_Component<CTransform>()->Set_Scale(vScale);
	m_pHit_Light->Get_Component<CTransform>()->Set_Scale(vScale);

	return S_OK;
}

void CProtego_Hit::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CProtego_Hit::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;


	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	m_pHit_SphereLay->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pHit_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
}

void CProtego_Hit::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CProtego_Hit::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_vector vHitPos = XMLoadFloat4(static_cast<_float4*>(pArg));

	CEffectParts* pProtego_HitPT =  Get_PartObject<CEffectParts>("Protego_HitPT");
	CEffectParts* pProtego_HitPT_Line = Get_PartObject<CEffectParts>("Protego_HitPT_Line");
	CEffectParts* pProtegoHit = Get_PartObject<CEffectParts>("ProtegoHit");

	m_pHit_SphereLay->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pHit_Light->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	pProtego_HitPT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);
	pProtego_HitPT_Line->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);
	pProtegoHit->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);

	
	m_pHit_SphereLay->Set_Visible(true);
	m_pHit_Light->Set_Visible(true);

	pProtego_HitPT->Set_Visible(true);
	pProtego_HitPT_Line->Set_Visible(true);
	pProtegoHit->Set_Visible(true);




	return S_OK;
}

HRESULT CProtego_Hit::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CProtego_Hit::Ready_Child()
{
	return S_OK;
}

CProtego_Hit* CProtego_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtego_Hit* pInstance = new CProtego_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProtego_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CProtego_Hit::Clone(void* pArg, CGameObject* pOwner)
{
	CProtego_Hit* pInstance = new CProtego_Hit(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProtego_Hit");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CProtego_Hit::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CProtego_Hit::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pHit_SphereLay);
	SAFE_RELEASE(m_pHit_Light);
}
#ifdef _DEBUG

void CProtego_Hit::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CProtego_Hit::Bind_ShaderResources()
{
	return S_OK;
}


