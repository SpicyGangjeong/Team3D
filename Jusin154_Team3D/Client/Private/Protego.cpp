#include "pch.h"
#include "Protego.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Player.h"
#include "EffectPool.h"
#include "Layer.h"

CProtego::CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CProtego::CProtego(const CProtego& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CProtego::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Protego")))
		return E_FAIL;

	return S_OK;

}

HRESULT CProtego::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_pSphere = Get_PartObject<CEffectParts>("ProtegoSphere");
	m_pSphereLay = Get_PartObject<CEffectParts>("ProtegoLay");

	SAFE_ADDREF(m_pSphere);
	SAFE_ADDREF(m_pSphereLay);

	m_wstrEffectName = L"Protego";


	m_fAmountSize = 0.1f;
	m_fSpeed = 5.f;

	m_fDuration = 6.f;

	return S_OK;
}

void CProtego::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CProtego::Update(_float fTimeDelta)
{
	if (m_bVisible == false) {
		dynamic_cast<CPlayer*>(m_pOwner)->Set_Shield(false);
		return;
	}



	m_pSphere->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pSphereLay->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	if (m_bVisible == false){
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}


	/* 시작 사이즈 러프 */
	if (m_fSizeAccTime > XM_PIDIV2)
		return;

	m_fSizeAccTime += fTimeDelta * m_fSpeed;

	_float fSize = 1 + sinf(m_fSizeAccTime) * m_fAmountSize;
	_float3 vSize = _float3(fSize, fSize, fSize);

	m_pSphere->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pSphereLay->Get_Component<CTransform>()->Set_Scale(vSize);
}

void CProtego::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false){
		return;
	}

	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position());
	__super::Late_Update(fTimeDelta);

}

HRESULT CProtego::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	m_pGameInstance->Attach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);

	m_pSphere->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pSphereLay->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	m_pSphere->Set_Visible(true);
	m_pSphereLay->Set_Visible(true);



	_float3 vSize = _float3(1.f, 1.f, 1.f);
	m_pSphere->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pSphereLay->Get_Component<CTransform>()->Set_Scale(vSize);



	m_fSizeAccTime = 0.f;

	return S_OK;
}

void CProtego::Set_Visible(_bool bVisible)
{
	m_bVisible = true;
	m_pGameInstance->Attach_Actor(*m_pRigidBody->Get_Actor(), CURRENT_LEVEL);
}

HRESULT CProtego::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}


	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::SKILL_PROTEGO);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_SHIELD"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}

		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	return S_OK;
}

HRESULT CProtego::Ready_Child()
{
	return S_OK;
}

CProtego* CProtego::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtego* pInstance = new CProtego(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CProtego");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CProtego::Clone(void* pArg, CGameObject* pOwner)
{
	CProtego* pInstance = new CProtego(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CProtego");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CProtego::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (!m_bVisible)
		return;

	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	dynamic_cast<CPlayer*>(m_pOwner)->Set_Shield(true);

	m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>()
		->Use_Skill(SKILL_TYPE::PROTEGO_HIT, m_pOwner, &CollisionDesc->vWorldPos);

	m_pOwner->OnCollision(pOther, &CollisionDesc);

	static_cast<CPlayer*>(m_pOwner)->Start_CameraShake(0.3f, 2.f);

	m_pRigidBody->Detach_Actor(CURRENT_LEVEL);
	m_bVisible = false;
}

void CProtego::Free()
{
	__super::Free();

	//if(m_pPhysHitBox != nullptr)
	//	if (m_pPhysHitBox->Get_Depth() == false)
	//		SAFE_RELEASE(m_pPhysHitBox);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pSphere);
	SAFE_RELEASE(m_pSphereLay);

}
#ifdef _DEBUG

void CProtego::Describe_Entity()
{
	GUI::Begin("PROTEGO");

	GUI::DragFloat("fAmountSize", &m_fAmountSize);
	GUI::DragFloat("fSpeed", &m_fSpeed);

	GUI::End();
}

#endif // _DEBUG

HRESULT CProtego::Bind_ShaderResources()
{
	return S_OK;
}


