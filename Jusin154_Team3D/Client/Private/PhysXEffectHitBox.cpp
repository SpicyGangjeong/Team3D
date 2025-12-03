#include "pch.h"
#include "PhysXEffectHitBox.h"
#include "GameInstance.h"


CPhysXEffectHitBox::CPhysXEffectHitBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPhysXEffectHitBox::CPhysXEffectHitBox(const CPhysXEffectHitBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPhysXEffectHitBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysXEffectHitBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CPhysXEffectHitBox::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
}

void CPhysXEffectHitBox::Update(_float fTimeDelta)
{
	m_pTransformCom->AccumulateMomentum(XMLoadFloat3(&m_vDeltaPos));
}

void CPhysXEffectHitBox::Late_Update(_float fTimeDelta)
{
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_Position());
	}
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CPhysXEffectHitBox::Render()
{
	//if (FAILED(Bind_ShaderResources())) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Bind_Material(0, m_pShaderCom))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Render(0))) {
	//	return E_FAIL;
	//}
#ifdef _DEBUG
	if (FAILED(m_pCharacter_Controller->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

void CPhysXEffectHitBox::Move(_float fTimeDelta)
{
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pCharacter_Controller->Move(fTimeDelta);
	}
}

HRESULT CPhysXEffectHitBox::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pDesc->vRotRPY.x, pDesc->vRotRPY.y, pDesc->vRotRPY.z);
	m_vDeltaPos = pDesc->vDeltaPos;
	m_vLifeTime = pDesc->vLifeTime;
	


	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = pDesc->iSubKind;
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::BOX;
		Desc.fContactOffset = 0.1f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
		Desc.fRadius = 0.5f;
		Desc.fHeight = 1.0f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_EffectHitBox_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_EffectHitBox_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		Desc.fWalkableSlope = 45.f;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}

		m_pCharacter_Controller->SetGravity(false);
	}
	m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CPhysXEffectHitBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

CPhysXEffectHitBox* CPhysXEffectHitBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhysXEffectHitBox* pInstance = new CPhysXEffectHitBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysXEffectHitBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CPhysXEffectHitBox::Clone(void* pArg, CGameObject* pOwner)
{
	CPhysXEffectHitBox* pInstance = new CPhysXEffectHitBox(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXEffectHitBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPhysXEffectHitBox::Free()
{
	if (nullptr == m_pGameInstance) {
		m_pCharacter_Controller->SetActive(false);
		PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
		PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
		m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
		m_pGameInstance->Detach_Actor(*m_pCharacter_Controller->Get_Actor());


	}
	__super::Free();
	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
	}
	SAFE_RELEASE(m_pCharacter_Controller);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}
#ifdef _DEBUG

void CPhysXEffectHitBox::Describe_Entity()
{
}

#endif // _DEBUG
