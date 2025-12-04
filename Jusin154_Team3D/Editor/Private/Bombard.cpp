#include "pch.h"
#include "Bombard.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"

CBombard::CBombard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBombard::CBombard(const CBombard& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBombard::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CBombard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Bombard")))
		return E_FAIL;


	m_wstrEffectName = L"Bombard";

	m_pLight_Projectile = Get_PartObject<CEditEffect>("Bombard_Projectile");
	SAFE_ADDREF(m_pLight_Projectile);

	m_fDuration = 2.5f;

	return S_OK;
}

void CBombard::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pLight_Projectile->Get_WorldPostion());
}

void CBombard::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	CTransform* pPJTransform = m_pLight_Projectile->Get_Component<CTransform>();

	pPJTransform->Translation(m_vCameraLook * 2.f);



	if (true == m_pGameInstance->SphereCast(0.125f, XMLoadFloat4(&m_vStartPos), m_vCameraLook, XMVectorGetX(XMVector3Length(m_vCameraLook * 2.f))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eSTATIC, hitBuffer))
	{
		OnCollision();
	}

}

void CBombard::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pLight_Projectile->Get_WorldPostion());

}

HRESULT CBombard::Pre_Setting(CGameObject* pObject)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return E_FAIL;


	CPartObject* pShootPt = Get_PartObject<CEditEffect>("Bombard_Shoot_Pt");
	CPartObject* pCircle0 = Get_PartObject<CEditEffect>("Bombard_Circle0");

	m_vCameraLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);


	pShootPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_pLight_Projectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	pCircle0->Set_Visible(true);
	pShootPt->Set_Visible(true);

	m_pLight_Projectile->Set_Visible(true);


	return S_OK;
}

HRESULT CBombard::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBombard::Ready_Child()
{
	return S_OK;
}

CBombard* CBombard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBombard* pInstance = new CBombard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBombard");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBombard::Clone(void* pArg, CGameObject* pOwner)
{
	CBombard* pInstance = new CBombard(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBombard");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBombard::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_isCollisionEnter == true)
		return;

	m_isCollisionEnter = true;

	_vector vPos = XMVectorSet(hitBuffer.block.position.x, hitBuffer.block.position.y, hitBuffer.block.position.z, 1.f);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return;

	CPartObject* pShootPt = Get_PartObject<CEditEffect>("Bombard_Shoot_Pt");
	CPartObject* pCircle0 = Get_PartObject<CEditEffect>("Bombard_Circle0");

	pShootPt->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());


	m_pLight_Projectile->Set_Visible(false);

	//Get_PartObject<CEditEffect>("Bombard_PT_0")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());
	//Get_PartObject<CEditEffect>("Bombard_PT_1")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());
	//Get_PartObject<CEditEffect>("Bombard_PT_2")->Get_Component<CTransform>()->LookAt(m_pOwner->Get_WorldPostion());

	_vector vOwnerLook = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	Get_PartObject<CEditEffect>("Bombard_Smoke")->Get_Component<CTransform>()->Translation(vOwnerLook);

}

void CBombard::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLight_Projectile);

}
#ifdef _DEBUG
void CBombard::Describe_Entity()
{

}
#endif

HRESULT CBombard::Bind_ShaderResources()
{
	return S_OK;
}


