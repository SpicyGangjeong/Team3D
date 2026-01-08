#include "pch.h"
#include "Ranrok_Prop.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "InfoInstance.h"
#include "Player.h"

CRanrok_Prop::CRanrok_Prop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Prop::CRanrok_Prop(const CRanrok_Prop& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Prop::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokProp")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_Prop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;



	m_pSphere = Get_PartObject<CEffectParts>("Sphere");
	m_pSphereLay = Get_PartObject<CEffectParts>("Sphere_Noise");
	m_pSphereHitLay = Get_PartObject<CEffectParts>("HitLay");

	m_pRing = Get_PartObject<CEffectParts>("Ring");

	SAFE_ADDREF(m_pSphere);
	SAFE_ADDREF(m_pSphereLay);
	SAFE_ADDREF(m_pRing);
	SAFE_ADDREF(m_pSphereHitLay);

	m_wstrEffectName = L"RanrokProp";


	m_fAmountSize = 0.1f;
	m_fSpeed = 5.f;

	m_fDuration = 10000.f;

	return S_OK;
}

void CRanrok_Prop::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Prop::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;


	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);



	/* 시작 사이즈 러프 */
	if (m_fSizeAccTime > XM_PIDIV2 && m_isSizeLerpEnd == false)
	{
		m_isSizeLerpEnd = true;

		CEffectParts* pGlowBall = Get_PartObject<CEffectParts>("GlowBall");
		CEffectParts* pCore = Get_PartObject<CEffectParts>("Core");

		pGlowBall->Set_Visible(true);
		pCore->Set_Visible(true);


		return;
	}


	m_fSizeAccTime += fTimeDelta * m_fSpeed;

	_float fSize = 1 + sinf(m_fSizeAccTime) * m_fAmountSize;
	_float3 vSize = _float3(fSize, fSize, fSize);

	m_pSphere->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pSphereLay->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pRing->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pSphereHitLay->Get_Component<CTransform>()->Set_Scale(vSize);

}

void CRanrok_Prop::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);

#if _DEBUG
	m_pRigidBody->Render();
#endif
}

HRESULT CRanrok_Prop::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	_vector vPos = pObject->Get_WorldPostion();

	_vector vRandomPos = XMVectorSet(m_pGameInstance->Real_Random_Float(-20.f, 20.f), m_pGameInstance->Real_Random_Float(5.f, 15.f), m_pGameInstance->Real_Random_Float(-20.f, 20.f), 0.f);
		
	vPos += vRandomPos;

	for (auto& pPart : m_PartObjects)
	{
		pPart.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	CEffectParts* pAppear_PT = Get_PartObject<CEffectParts>("Appear_PT");
	CEffectParts* pSpline = Get_PartObject<CEffectParts>("Spline");


	m_pSphere->Set_Visible(true);
	m_pSphereLay->Set_Visible(true);
	m_pRing->Set_Visible(true);
	pAppear_PT->Set_Visible(true);
	pSpline->Set_Visible(true);
	


	_float3 vSize = _float3(1.f, 1.f, 1.f);
	m_pSphere->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pSphereLay->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pRing->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pSphereHitLay->Get_Component<CTransform>()->Set_Scale(vSize);

	m_fSizeAccTime = 0.f;

	m_isSizeLerpEnd = false;

	m_fHp = 3.f;
	m_fDuration = 10000.f;

	return S_OK;
}

HRESULT CRanrok_Prop::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::RANROK_PROP);

		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_SHIELD"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}

		m_pGameInstance->Attach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	return S_OK;
}

HRESULT CRanrok_Prop::Ready_Child()
{
	return S_OK;
}

CRanrok_Prop* CRanrok_Prop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Prop* pInstance = new CRanrok_Prop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Prop");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Prop::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Prop* pInstance = new CRanrok_Prop(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Prop");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Prop::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = nullptr;

	if(pDesc != nullptr)
		CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);


	m_fHp--;


	if (m_fHp > 0)
	{
		CEffectParts* pHitLay = Get_PartObject<CEffectParts>("HitLay");
		CEffectParts* pHit_PT = Get_PartObject<CEffectParts>("Hit_PT");
		CEffectParts* pPulse_Purple = Get_PartObject<CEffectParts>("Pulse_Purple");
		

		pHitLay->Set_Visible(true);
		pHit_PT->Set_Visible(true);
		pPulse_Purple->Set_Visible(true);

		pHitLay->Get_Component<CInstance_Model>()->Instane_Buffer_ReStruct();
		pHit_PT->Get_Component<CInstance_Model>()->Instane_Buffer_ReStruct();
		pPulse_Purple->Get_Component<CInstance_Model>()->Instane_Buffer_ReStruct();

		if (CollisionDesc != nullptr)
		{
			_vector vHitPos = XMLoadFloat4(&CollisionDesc->vWorldPos);
			pHit_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);
		}

	}
	else
	{

		for (auto& pPart : m_PartObjects)
		{
			pPart.second->Set_Visible(false);
		}

		CEffectParts* pHitSplash0 = Get_PartObject<CEffectParts>("HitSplash0");
		CEffectParts* pHitSplash1 = Get_PartObject<CEffectParts>("HitSplash1");
		CEffectParts* pBroken_Goo = Get_PartObject<CEffectParts>("Broken_Goo");
		CEffectParts* pBroken_PT = Get_PartObject<CEffectParts>("Broken_PT");
		CEffectParts* pBroken_PT2 = Get_PartObject<CEffectParts>("Broken_PT2");
		

		pHitSplash0->Set_Visible(true);
		pHitSplash1->Set_Visible(true);
		pBroken_Goo->Set_Visible(true);
		pBroken_PT->Set_Visible(true);
		pBroken_PT2->Set_Visible(true);

		m_fDuration = 5.f;
		m_fAccTime = 0.f;

	}



}

void CRanrok_Prop::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pSphere);
	SAFE_RELEASE(m_pSphereLay);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pRing);
	SAFE_RELEASE(m_pSphereHitLay);

	SAFE_RELEASE(m_pRigidBody);
}
#ifdef _DEBUG

void CRanrok_Prop::Describe_Entity()
{


	if(GUI::Button("Hit"))
	{
		OnCollision();
	}

}

#endif // _DEBUG

HRESULT CRanrok_Prop::Bind_ShaderResources()
{
	return S_OK;
}


