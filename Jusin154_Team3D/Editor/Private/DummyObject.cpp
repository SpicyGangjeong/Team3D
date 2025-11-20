#include "pch.h"
#include "DummyObject.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "State_Idle.h"
#include "State_Walk.h"
#include "State_Dodge.h"
#include "State_Sprint.h"
#include "State_Jump.h"
#include "State_Skill.h"

CDummyObject::CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CDummyObject::CDummyObject(const CDummyObject& Prototype)
	: CUnit(Prototype)
{
}

HRESULT CDummyObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummyObject::Initialize(void* pArg)
{
	PARTS_OBJECT_DESC* pDesc = static_cast<PARTS_OBJECT_DESC*>(pArg);

	m_strModelPrototypeTag = pDesc->pModelPrototypeTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CDummyObject::Priority_Update(_float fTimeDelta)
{

}

void CDummyObject::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta,m_pTransformCom);
}

void CDummyObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CDummyObject::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
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

HRESULT CDummyObject::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyObject::Bind_ShaderResources()
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

CDummyObject* CDummyObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummyObject* pInstance = new CDummyObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummyObject");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CDummyObject::Clone(void* pArg, CGameObject* pOwner)
{
	CDummyObject* pInstance = new CDummyObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummyObject");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CDummyObject::Free()
{
	__super::Free();
}

void CDummyObject::Describe_Entity()
{
}
