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
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_Position());
}

void CDummy_PhysXPlayable::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_UPARROW)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 0.f, 1.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_LEFT)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(-1.f, 0.f, 0.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_DOWN)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 0.f, -1.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT)) {
		m_pTransformCom->AccumulateMomentum(XMVectorSet(1.f, 0.f, 0.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL)) {
		if (m_pGameInstance->Key_Pressing(DIK_SPACE)) {
			m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, 10.f, 0.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
		}
		if (m_pGameInstance->Key_Pressing(DIK_C)) {
			m_pTransformCom->AccumulateMomentum(XMVectorSet(0.f, -10.f, 0.f, 0.f) * m_pTransformCom->Get_Speed() * fTimeDelta);
		}
		if (m_pGameInstance->Key_Down(DIK_V)) {
			m_pGameInstance->Detach_Actor(*m_pCharacter_Controller->Get_Actor(), m_pGameInstance->Get_CurrentLevelID());
		}
	}
	else {
		if (m_pGameInstance->Key_Down(DIK_V)) {
			m_pGameInstance->Attach_Actor(*m_pCharacter_Controller->Get_Actor(), m_pGameInstance->Get_CurrentLevelID());
		}
	}
	m_pCharacter_Controller->Move(fTimeDelta);
}

void CDummy_PhysXPlayable::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXPlayable::Render()
{
	//if (FAILED(Bind_ShaderResources())) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Bind_Material(0, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
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

	m_pCallBack_Behavior->Initialize(m_pCharacter_Controller, m_pRigidBody);
	m_pCallBack_HitReport->Initialize(m_pCharacter_Controller, m_pRigidBody);

	return S_OK;
}

HRESULT CDummy_PhysXPlayable::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC TransformDesc{};
	TransformDesc.fRadius = 100.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.f);
	TransformDesc.fSpeedPerSec = 15.f;
	if (FAILED(__super::Ready_Components(&TransformDesc))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pDesc->vRotRPY.x, pDesc->vRotRPY.y, pDesc->vRotRPY.z);
	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = pDesc->iSubKind;
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.02f };
		Desc.fRadius = 0.5f;
		Desc.fHeight = 1.0f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Playable_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Playable_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		Desc.fWalkableSlope = 45.f;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
	}
	m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = pDesc->iSubKind;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}
	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CDummy_PhysXPlayable::Bind_ShaderResources()
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
	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
	}
	SAFE_RELEASE(m_pCharacter_Controller);
	SAFE_RELEASE(m_pRigidBody);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}
#ifdef _DEBUG

void CDummy_PhysXPlayable::Describe_Entity()
{
	m_pCharacter_Controller->Describe_Entity();
}

#endif // _DEBUG
