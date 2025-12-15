#include "pch.h"
#include "StunEffect.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"

CStunEffect::CStunEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CStunEffect::CStunEffect(const CStunEffect& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CStunEffect::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Stun")))
		return E_FAIL;

	return S_OK;

}

HRESULT CStunEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Stun";


	m_fDuration = 3.f;
	
	m_pStunEffect = Get_PartObject<CEffectParts>("StunEffect");
	SAFE_ADDREF(m_pStunEffect);


	m_Events.emplace(3.f, [&]() {
		
		m_BoneMat = nullptr;

		});

	return S_OK;
}

void CStunEffect::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CStunEffect::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	if (m_BoneMat == nullptr)
		return;

	_matrix BoneMat = XMLoadFloat4x4(m_BoneMat);

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}

	BoneMat *= m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	m_pStunEffect->Get_Component<CTransform>()->Set_State(STATE::POSITION, BoneMat.r[3]);

}

void CStunEffect::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CStunEffect::Pre_Setting(CGameObject* pObject, void* pArg)
{

	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	_string strBoneName = *static_cast<_string*>(pArg);

	m_BoneMat = m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr(strBoneName.c_str()) ; // 위치 넘기기

	_matrix BoneMat = XMLoadFloat4x4(m_BoneMat);

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}
	BoneMat *= m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	m_pStunEffect->Get_Component<CTransform>()->Set_State(STATE::POSITION, BoneMat.r[3]);



	m_pStunEffect->Set_Visible(true);


	return S_OK;
}

HRESULT CStunEffect::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStunEffect::Ready_Child()
{
	return S_OK;
}

CStunEffect* CStunEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStunEffect* pInstance = new CStunEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStunEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CStunEffect::Clone(void* pArg, CGameObject* pOwner)
{
	CStunEffect* pInstance = new CStunEffect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStunEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CStunEffect::OnCollision(CGameObject* pOther, void* pDesc)
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

void CStunEffect::Free()
{
	__super::Free();
	Safe_Release(m_pStunEffect);

}
#ifdef _DEBUG
void CStunEffect::Describe_Entity()
{

}
#endif

HRESULT CStunEffect::Bind_ShaderResources()
{
	return S_OK;
}


