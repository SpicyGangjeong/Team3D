#include "pch.h"
#include "ReparoObject.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Unit.h"
#include "PartObject.h"
#include "EffectParts.h"

CReparoObject::CReparoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CReparoObject::CReparoObject(const CReparoObject& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CReparoObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReparoObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pModelCom->Set_AnimationIndex(0);


	return S_OK;
}

void CReparoObject::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CReparoObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG


}

void CReparoObject::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CReparoObject::Render()
{
	if (!m_pModelCom) {
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CReparoObject::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerA_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CReparoObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
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

CReparoObject* CReparoObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReparoObject* pInstance = new CReparoObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CReparoObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CReparoObject::Clone(void* pArg, CGameObject* pOwner)
{
	CReparoObject* pInstance = new CReparoObject(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CReparoObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CReparoObject::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CReparoObject::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	
	GUI::End();
}

#endif // _DEBUG
