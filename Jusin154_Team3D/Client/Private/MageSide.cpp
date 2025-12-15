#include "pch.h"
#include "MageSide.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "TrailObject.h"

CMageSide::CMageSide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CMageSide::CMageSide(const CMageSide& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CMageSide::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/GoblinMage/MageSide")))
		return E_FAIL;

	return S_OK;

}

HRESULT CMageSide::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"MageSide";

	m_pMage_PT = Get_PartObject<CEffectParts>("Mage_PT0");
	m_pMage_Trail = Get_PartObject<CTrailObject>("Mage_Trail");

	SAFE_ADDREF(m_pMage_PT);
	SAFE_ADDREF(m_pMage_Trail);

	m_fDuration = 5.f;

	m_Events.emplace(3.f, [&]() {
		m_isParticleEnd = true;
		m_pMage_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));

		});

	return S_OK;
}

void CMageSide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CMageSide::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	_matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("RightHand"));

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}


	
	_matrix WorldMat = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	_matrix CombinedMat = BoneMat * WorldMat;


	if(m_isParticleEnd == false)
		m_pMage_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, CombinedMat.r[3]);
	m_pMage_Trail->Trail_Update(CombinedMat, fTimeDelta);

}

void CMageSide::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CMageSide::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("RightHand"));
	_matrix WorldMat = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	_matrix CombinedMat = BoneMat * WorldMat;
	
	m_pMage_Trail->Get_Component<CTrail>()->Reset_Trail();

	m_pMage_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, CombinedMat.r[3]);

	m_pMage_PT->Set_Visible(true);
	m_pMage_Trail->Set_Visible(true);

	m_isParticleEnd = false;

	return S_OK;
}

HRESULT CMageSide::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMageSide::Ready_Child()
{
	return S_OK;
}

CMageSide* CMageSide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMageSide* pInstance = new CMageSide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMageSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CMageSide::Clone(void* pArg, CGameObject* pOwner)
{
	CMageSide* pInstance = new CMageSide(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMageSide");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMageSide::OnCollision(CGameObject* pOther, void* pDesc)
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

void CMageSide::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pMage_PT);
	SAFE_RELEASE(m_pMage_Trail);

}
#ifdef _DEBUG
void CMageSide::Describe_Entity()
{

}
#endif

HRESULT CMageSide::Bind_ShaderResources()
{
	return S_OK;
}


