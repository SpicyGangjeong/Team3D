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
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CDummy_PhysXPlayable::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//m_pShaderCom->Bind_SRV("g_NormalTexture", nullptr);
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
		//	return E_FAIL;
		//}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

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

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Steve_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//m_pModelCom->Change_AnimationIndex(0, true, 0.4f, true);

	return S_OK;

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

	SAFE_RELEASE(m_pCharacter_Controller);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

void CDummy_PhysXPlayable::Describe_Entity()
{
	m_pCharacter_Controller->Describe_Entity();
}
