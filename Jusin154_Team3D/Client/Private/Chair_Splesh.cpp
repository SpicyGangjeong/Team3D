#include "pch.h"
#include "Chair_Splesh.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CChair_Splesh::CChair_Splesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CChair_Splesh::CChair_Splesh(const CChair_Splesh& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CChair_Splesh::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CChair_Splesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Chair")))
		return E_FAIL;


	m_wstrEffectName = L"Chair_Splesh";


	m_fDuration = 5.f;

	return S_OK;
}

void CChair_Splesh::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CChair_Splesh::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CChair_Splesh::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CChair_Splesh::Pre_Setting(CGameObject* pObject, void* pArg)
{

	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CEffectParts* pHit = Get_PartObject<CEffectParts>("Hit");
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");
	CEffectParts* pWood_PT0 = Get_PartObject<CEffectParts>("Wood_PT0");
	CEffectParts* pWood_PT1 = Get_PartObject<CEffectParts>("Wood_PT1");

	
	_vector vPos = m_pOwner->Get_WorldPostion();

	pWood_PT0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pWood_PT1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pHit->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);


	pWood_PT0->Set_Visible(true);
	pWood_PT1->Set_Visible(true);
	pHit->Set_Visible(true);
	pSmoke->Set_Visible(true);


	return S_OK;
}

HRESULT CChair_Splesh::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CChair_Splesh::Ready_Child()
{
	return S_OK;
}

CChair_Splesh* CChair_Splesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChair_Splesh* pInstance = new CChair_Splesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBarral_Splesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CChair_Splesh::Clone(void* pArg, CGameObject* pOwner)
{
	CChair_Splesh* pInstance = new CChair_Splesh(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBarral_Splesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CChair_Splesh::OnCollision(CGameObject* pOther, void* pDesc)
{
	_int iIndex = CollisionCheck();

	if (iIndex < 0)
		return;

	if (m_isCollisionEnter == true)
		return;

	m_isCollisionEnter = true;

	_vector vPos = XMVectorSet(m_Hitbuffer.touches[iIndex].position.x, m_Hitbuffer.touches[iIndex].position.y, m_Hitbuffer.touches[iIndex].position.z, 1.f);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}


}

void CChair_Splesh::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CChair_Splesh::Describe_Entity()
{

}
#endif

HRESULT CChair_Splesh::Bind_ShaderResources()
{
	return S_OK;
}


