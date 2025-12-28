#include "pch.h"
#include "Ranrok_Breath.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Breath::CRanrok_Breath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Breath::CRanrok_Breath(const CRanrok_Breath& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Breath::Initialize_Prototype()
{


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokBreath")))
		return E_FAIL;
	return S_OK;

}

HRESULT CRanrok_Breath::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"RanrokBreath";


	m_pBreath = Get_PartObject<CEffectParts>("Breath");
	m_pBreath_Black = Get_PartObject<CEffectParts>("Breath_Black");
	m_pBreathSlog = Get_PartObject<CEffectParts>("BreathSlog");
	m_pMouthFire = Get_PartObject<CEffectParts>("MouthFire");

	SAFE_ADDREF(m_pBreath);
	SAFE_ADDREF(m_pBreath_Black);
	SAFE_ADDREF(m_pBreathSlog);
	SAFE_ADDREF(m_pMouthFire);

	m_fDuration = 5.f;


	m_Events.emplace(0.5f, [&]() {
		m_isStartRayCast = true;
		});


	m_Events.emplace(3.f, [&]() {

		// 파티클 제어
		m_isParticleEnd = true;

		CEffectParts* pRock_PT = Get_PartObject<CEffectParts>("Rock_PT");
		CEffectParts* pBottomFire = Get_PartObject<CEffectParts>("BottomFire");
		CEffectParts* pSplatterBottom0 = Get_PartObject<CEffectParts>("SplatterBottom0");

		m_pMouthFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));

		pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
		pBottomFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
		pSplatterBottom0->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));

		m_pBreath->Get_Component<CInstance_Model>()->Set_Loop(false);
		m_pBreath->Get_Effect_Info()->isDissolve = true;

		m_pBreath_Black->Get_Component<CInstance_Model>()->Set_Loop(false);
		m_pBreath_Black->Get_Effect_Info()->isDissolve = true;

		m_pBreathSlog->Set_Visible(false);

		});
	return S_OK;
}

void CRanrok_Breath::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


}

void CRanrok_Breath::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	_matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("tongue_01"));

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}

	_matrix WorldMat = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix() ;

	_matrix CombinedMat = BoneMat * WorldMat;



	m_pBreath->Get_Component<CTransform>()->Set_WorldMatrix(CombinedMat);
	m_pBreath_Black->Get_Component<CTransform>()->Set_WorldMatrix(CombinedMat);
	m_pBreathSlog->Get_Component<CTransform>()->Set_WorldMatrix(CombinedMat);

	if(m_isParticleEnd == false)
		m_pMouthFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, CombinedMat.r[3]);


	_vector vStartPos = CombinedMat.r[3];

	XMStoreFloat4(&m_vStartPos, vStartPos);

	if(m_isStartRayCast == true)
	{
		ON_COLLISION_INFO CollisionInfo = MonsterRayCast(XMLoadFloat4(&m_vStartPos), CombinedMat.r[2] * -1.f, 50.f, 50);
		if (m_bHit = true)
			OnCollision(this, &CollisionInfo);

	}


	
}

void CRanrok_Breath::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Breath::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	/* 초기 객체 비지블*/

	CEffectParts* pBlurMesh = Get_PartObject<CEffectParts>("BlurMesh");

	 m_pBreath->Set_Visible(true);
	 m_pBreath_Black->Set_Visible(true);
	 m_pBreathSlog->Set_Visible(true);
	 m_pMouthFire->Set_Visible(true);
	 pBlurMesh->Set_Visible(true);

	/* 초기 객체 위치 초기화*/

	 _matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("tongue_01"));

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}

	_matrix WorldMat = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix() ;

	_matrix CombinedMat = BoneMat * WorldMat;



	 m_pBreath->Get_Component<CTransform>()->Set_WorldMatrix(CombinedMat);
	 m_pBreath_Black->Get_Component<CTransform>()->Set_WorldMatrix(CombinedMat);
	 m_pBreathSlog->Get_Component<CTransform>()->Set_WorldMatrix(CombinedMat);

	 _vector vStartPos = CombinedMat.r[3];

	 XMStoreFloat4(&m_vStartPos, vStartPos);



	 m_pMouthFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, CombinedMat.r[3]);
	 pBlurMesh->Get_Component<CTransform>()->Set_State(STATE::POSITION, CombinedMat.r[3]);

	 /* 브레스 디졸브를 위해 초기값 설정*/
	 m_pBreath->Get_Component<CInstance_Model>()->Set_Loop(true);
	 m_pBreath->Get_Effect_Info()->isDissolve = false;

	 m_pBreath_Black->Get_Component<CInstance_Model>()->Set_Loop(true);
	 m_pBreath_Black->Get_Effect_Info()->isDissolve = false;

	 m_isParticleEnd = false;
	 m_isStartRayCast = false;
	return S_OK;
}

HRESULT CRanrok_Breath::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Breath::Ready_Child()
{
	return S_OK;
}

CRanrok_Breath* CRanrok_Breath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Breath* pInstance = new CRanrok_Breath(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Breath");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Breath::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Breath* pInstance = new CRanrok_Breath(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Breath");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Breath::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	if (XMVectorGetX(XMVector4Length(vPos)) <= 0.f) /* 000 일때 들어오지마 */
		return;

	_vector vPrePos = XMLoadFloat4(&m_vPreCollisionPos);
	_vector vDir = XMVector3Normalize(vPos - vPrePos);

	if (m_isParticleEnd == true)
		return;

	CEffectParts* pRock_PT = Get_PartObject<CEffectParts>("Rock_PT");
	CEffectParts* pBottomFire = Get_PartObject<CEffectParts>("BottomFire");
	CEffectParts* pSplatterBottom0 = Get_PartObject<CEffectParts>("SplatterBottom0");

	pRock_PT->Set_Visible(true);
	pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	pBottomFire->Set_Visible(true);
	pBottomFire->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	pSplatterBottom0->Set_Visible(true);
	pSplatterBottom0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos + vDir * 3.f);

	XMStoreFloat4(&m_vPreCollisionPos, vPos);
}

void CRanrok_Breath::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);

	SAFE_RELEASE(m_pBreath);
	SAFE_RELEASE(m_pBreath_Black);
	SAFE_RELEASE(m_pBreathSlog);
	SAFE_RELEASE(m_pMouthFire);
	
	//SAFE_RELEASE(m_pFireBall_Tail);
}
#ifdef _DEBUG
void CRanrok_Breath::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Breath::Bind_ShaderResources()
{
	return S_OK;
}


