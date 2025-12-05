#include "pch.h"
#include "Decendo.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "TrailObject.h"

#include "Wand.h"
#include "Player.h"


CDecendo::CDecendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CDecendo::CDecendo(const CDecendo& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CDecendo::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CDecendo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Decendo")))
		return E_FAIL;


	m_wstrEffectName = L"Decendo";

	m_pProjectile_Blur = Get_PartObject<CEditEffect>("Decendo_Proj_Blur");
	m_pProjectile = Get_PartObject<CEditEffect>("Decendo_Proj");

	SAFE_ADDREF(m_pProjectile_Blur);
	SAFE_ADDREF(m_pProjectile);

	m_fDuration = 3.5f;

	return S_OK;
}

void CDecendo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pProjectile->Get_WorldPostion());
}

void CDecendo::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	m_pProjectile_Blur->Get_Component<CTransform>()->Translation(m_vCameraLook * 0.5f);
	m_pProjectile->Get_Component<CTransform>()->Translation(m_vCameraLook * 0.5f);


	if (m_fRotateAccTime > XM_2PI)
		return;

	m_fRotateAccTime += fTimeDelta * 7.5f;

	m_pProjectile_Blur->Get_Component<CTransform>()->Translation(m_vRotateUp * 0.2f * sinf(m_fRotateAccTime));
	m_pProjectile->Get_Component<CTransform>()->Translation(m_vRotateUp * 0.2f * sinf(m_fRotateAccTime));


	//TODO : 다시 위치 잡기 
	if (true == m_pGameInstance->SphereCast(0.0625, XMLoadFloat4(&m_vStartPos), m_vCameraLook, XMVectorGetX(XMVector3Length(m_vCameraLook * 2.f))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC | PSX::PxQueryFlag::eSTATIC, m_Hitbuffer))
	{
		OnCollision();
	}

}

void CDecendo::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	Get_PartObject<CTrailObject>()->Trail_Update(m_pProjectile->Get_Component<CTransform>()->Get_XMWorldMatrix(), fTimeDelta);

	XMStoreFloat4(&m_vEndPos, m_pProjectile->Get_WorldPostion());

	__super::Late_Update(fTimeDelta);
}

HRESULT CDecendo::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return E_FAIL;


	CPartObject* pCircle0 = Get_PartObject<CEditEffect>();

	CPartObject* pWandLight = Get_PartObject<CEditEffect>("Decendo_Wand_Light");



	/* 초기 객체 위치 초기화 */
	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	m_pProjectile->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	m_pProjectile_Blur->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	/* 초기 객체 비지블 */
	pCircle0->Set_Visible(true);
	pWandLight->Set_Visible(true);

	m_pProjectile->Set_Visible(true);
	m_pProjectile_Blur->Set_Visible(true);


	/*트레일 초기화 */
	Get_PartObject<CTrailObject>()->Set_Visible(true);
	Get_PartObject<CTrailObject>()->Get_Component<CTrail>()->Reset_Trail();


	//나아가는 벡터와 한점을 가져와 수직인 평면상에 하나의 점으로  DIR 을 만듬
	m_vCameraLook = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);


	_vector vQuaternion = XMQuaternionRotationAxis(m_vCameraLook, m_pGameInstance->Random_Float(0.f, XM_PIDIV2));

	m_vRotateUp = XMVector3Rotate(vUp, vQuaternion);
	m_vRotateUp = XMVector3Normalize(m_vRotateUp);

	m_fRotateAccTime = 0.f;

	return S_OK;
}

HRESULT CDecendo::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDecendo::Ready_Child()
{
	return S_OK;
}

CDecendo* CDecendo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecendo* pInstance = new CDecendo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDecendo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CDecendo::Clone(void* pArg, CGameObject* pOwner)
{
	CDecendo* pInstance = new CDecendo(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDecendo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDecendo::OnCollision(CGameObject* pOther, void* pDesc)
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



	m_pProjectile_Blur->Set_Visible(false);
	m_pProjectile->Set_Visible(false);


	CWand* pWand = static_cast<CPlayer*>(m_pOwner)->Get_PartObject<CWand>();

	if (pWand == nullptr)
		return;

	CPartObject* pCircle0 = Get_PartObject<CEditEffect>();
	CPartObject* pWandLight = Get_PartObject<CEditEffect>("Decendo_Wand_Light");


	pCircle0->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pWand->Get_WorldPostion());

	_vector vPlayerPos = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);



	Get_PartObject<CEditEffect>("Decendo_Down")->Get_Component<CTransform>()->LookAt(vPlayerPos);
	Get_PartObject<CEditEffect>("Decendo_Smoke")->Get_Component<CTransform>()->LookAt(vPlayerPos);



	Get_PartObject<CTrailObject>()->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	Get_PartObject<CTrailObject>()->Set_Visible(false);

}

void CDecendo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pProjectile_Blur);
	SAFE_RELEASE(m_pProjectile);

}
#ifdef _DEBUG
void CDecendo::Describe_Entity()
{

}
#endif
HRESULT CDecendo::Bind_ShaderResources()
{
	return S_OK;
}


