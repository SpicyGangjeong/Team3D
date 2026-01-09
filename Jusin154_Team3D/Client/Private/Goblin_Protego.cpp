#include "pch.h"
#include "Goblin_Protego.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "MapElement_Interactable.h"


CGoblin_Protego::CGoblin_Protego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CGoblin_Protego::CGoblin_Protego(const CGoblin_Protego& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CGoblin_Protego::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/GoblinProtego")))
		return E_FAIL;

	return S_OK;

}

HRESULT CGoblin_Protego::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_pSphere = Get_PartObject<CEffectParts>("ProtegoSphere");
	m_pBottom = Get_PartObject<CEffectParts>("ProtegoBottom");
	m_pCircle = Get_PartObject<CEffectParts>("ProtegoCircle");

	SAFE_ADDREF(m_pSphere);
	SAFE_ADDREF(m_pBottom);
	SAFE_ADDREF(m_pCircle);

	m_wstrEffectName = L"Goblin_Protego";


	m_fAmountSize = 0.1f;
	m_fSpeed = 5.f;

	m_fDuration = 3.f;

	return S_OK;
}

void CGoblin_Protego::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CGoblin_Protego::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pSphere->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pBottom->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pCircle-> Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	if (m_bVisible == false) {
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}


	/* 시작 사이즈 러프 */
	if (m_fSizeAccTime > XM_PIDIV2)
		return;

	m_fSizeAccTime += fTimeDelta * m_fSpeed;

	_float fSize = 1 + sinf(m_fSizeAccTime) * m_fAmountSize;
	_float3 vSize = _float3(fSize, fSize, fSize);

	m_pSphere->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pCircle->Get_Component<CTransform>()->Set_Scale(vSize);
}

void CGoblin_Protego::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;
	m_pTransformCom->Set_State(STATE::POSITION, m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position());
	__super::Late_Update(fTimeDelta);
}

HRESULT CGoblin_Protego::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	m_pSphere->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pBottom->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());
	m_pCircle->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pOwner->Get_WorldPostion());

	m_pSphere->Set_Visible(true);
	m_pCircle->Set_Visible(true);
	m_pBottom->Set_Visible(true);


	_float3 vSize = _float3(1.f, 1.f, 1.f);
	m_pSphere->Get_Component<CTransform>()->Set_Scale(vSize);
	m_pCircle->Get_Component<CTransform>()->Set_Scale(vSize);

	m_fSizeAccTime = 0.f;

	return S_OK;
}

HRESULT CGoblin_Protego::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_PROTEGO);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_SHIELD"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Attach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	return S_OK;
}

HRESULT CGoblin_Protego::Ready_Child()
{
	return S_OK;
}

CGoblin_Protego* CGoblin_Protego::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Protego* pInstance = new CGoblin_Protego(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Protego");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CGoblin_Protego::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Protego* pInstance = new CGoblin_Protego(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Protego");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_Protego::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (!m_bVisible)
		return;

	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	CEffect_Container* pEffect_Container = dynamic_cast<CEffect_Container*>(pOther);

	if (pEffect_Container != nullptr)
	{
		_uint iSkillType = pEffect_Container->Get_SkillType();

		switch (iSkillType)
		{
		case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
			m_pOwner->OnCollision(pOther, CollisionDesc);
			m_bVisible = false;
			break;
		case ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA):
			m_pOwner->OnCollision(pOther, CollisionDesc);
			m_bVisible = false;
			break;
		case ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC):
			m_pOwner->OnCollision(pOther, CollisionDesc);
			m_bVisible = false;
			break;
		}
	}
	else
	{
		CMapElement_Interactable* pProps = dynamic_cast<CMapElement_Interactable*>(pOther);
		if (pProps != nullptr)
		{
			m_pOwner->OnCollision(pOther, CollisionDesc);
			m_bVisible = false;
		}
	}

}

void CGoblin_Protego::Free()
{
	__super::Free();

	//if(m_pPhysHitBox != nullptr)
	//	if (m_pPhysHitBox->Get_Depth() == false)
	//		SAFE_RELEASE(m_pPhysHitBox);

	SAFE_RELEASE(m_pSphere);
	SAFE_RELEASE(m_pBottom);
	SAFE_RELEASE(m_pCircle);
	SAFE_RELEASE(m_pRigidBody);

}
#ifdef _DEBUG

void CGoblin_Protego::Describe_Entity()
{
	GUI::Begin("PROTEGO", 0, IMGUI_GLOBAL_BEGIN_FLAG);

	GUI::DragFloat("fAmountSize" ,&m_fAmountSize);
	GUI::DragFloat("fSpeed" , &m_fSpeed);

	GUI::End();
}

#endif // _DEBUG

HRESULT CGoblin_Protego::Bind_ShaderResources()
{
	return S_OK;
}


