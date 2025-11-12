#include "pch.h"
#include "Dummy_PhysXPlayable.h"

#include "GameInstance.h"

CDummy_PhysXPlayable::CDummy_PhysXPlayable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):
	CGameObject(pDevice, pContext)
{
}

CDummy_PhysXPlayable::CDummy_PhysXPlayable(const CDummy_PhysXPlayable& rhs):
	CGameObject(rhs)
{
}

void CDummy_PhysXPlayable::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
}

void CDummy_PhysXPlayable::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}
	if (m_pGameInstance->Key_Pressing(DIK_A)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(-1.f, 0.f, 0.f, 0.f));
	}
	if (m_pGameInstance->Key_Pressing(DIK_S)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 0.f, -1.f, 0.f));
	}
	if (m_pGameInstance->Key_Pressing(DIK_D)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(1.f, 0.f, 0.f, 0.f));
	}
	if (m_pGameInstance->Key_Pressing(DIK_SPACE)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 10.f, 0.f, 0.f));
	}
	if (m_pGameInstance->Key_Pressing(DIK_C)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, -10.f, 0.f, 0.f));
	}
	//m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, -GRAVITY, 0.f, 0.f));
	m_pCharacter_Controller->Move(fTimeDelta);
}

void CDummy_PhysXPlayable::Late_Update(_float fTimeDelta)
{
	Describe_Entity();
}

HRESULT CDummy_PhysXPlayable::Render()
{
	return S_OK;
}

HRESULT CDummy_PhysXPlayable::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXPlayable::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_PhysXPlayable::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(nullptr))) {
		return E_FAIL;
	}
	{
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.1f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
		Desc.tCapsuleInfo.fRadius = 1.5f;
		Desc.tCapsuleInfo.fHeight = 3.0f;
		Desc.tCapsuleInfo.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CDummy_PhysXPlayable::Bind_ShaderResources()
{
	return S_OK;
}

CDummy_PhysXPlayable* CDummy_PhysXPlayable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXPlayable* pInstance = new CDummy_PhysXPlayable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXPlayable");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXPlayable::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXPlayable* pInstance = new CDummy_PhysXPlayable(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXPlayable");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXPlayable::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pCharacter_Controller);
}

void CDummy_PhysXPlayable::Describe_Entity()
{
	m_pCharacter_Controller->Describe_Entity();
}
