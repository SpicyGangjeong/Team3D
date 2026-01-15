#include "pch.h"
#include "Barral_Splesh.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CBarral_Splesh::CBarral_Splesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CBarral_Splesh::CBarral_Splesh(const CBarral_Splesh& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CBarral_Splesh::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Barral")))
		return E_FAIL;

	return S_OK;

}

HRESULT CBarral_Splesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"Barral_Splesh";


	m_fDuration = 5.f;

	return S_OK;
}

void CBarral_Splesh::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CBarral_Splesh::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CBarral_Splesh::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CBarral_Splesh::Pre_Setting(CGameObject* pObject, void* pArg)
{

	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CEffectParts* pWood_PT0 = Get_PartObject<CEffectParts>("Wood_PT0");
	CEffectParts* pWood_PT1 = Get_PartObject<CEffectParts>("Wood_PT1");
	CEffectParts* pWood_PT2 = Get_PartObject<CEffectParts>("Wood_PT2");
	CEffectParts* pWater_Splesh = Get_PartObject<CEffectParts>("Water_Splesh");

	
	_vector vPos = m_pOwner->Get_WorldPostion();

	pWood_PT0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pWood_PT1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pWood_PT2->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pWater_Splesh->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);


	pWood_PT0->Set_Visible(true);
	pWood_PT1->Set_Visible(true);
	pWood_PT2->Set_Visible(true);
	pWater_Splesh->Set_Visible(true);


	m_pGameInstance->Sound_Play(SOUND::SD_KIND::BARREL0, SD_CHANNEL_GROUP::EFFECT, false, 0.8f);


	return S_OK;
}

HRESULT CBarral_Splesh::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBarral_Splesh::Ready_Child()
{
	return S_OK;
}

CBarral_Splesh* CBarral_Splesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBarral_Splesh* pInstance = new CBarral_Splesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBarral_Splesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CBarral_Splesh::Clone(void* pArg, CGameObject* pOwner)
{
	CBarral_Splesh* pInstance = new CBarral_Splesh(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBarral_Splesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBarral_Splesh::OnCollision(CGameObject* pOther, void* pDesc)
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

void CBarral_Splesh::Free()
{
	__super::Free();

}
#ifdef _DEBUG
void CBarral_Splesh::Describe_Entity()
{

}
#endif

HRESULT CBarral_Splesh::Bind_ShaderResources()
{
	return S_OK;
}


