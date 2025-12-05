#include "pch.h"
#include "Goblin_Dagger.h"

#include "GameInstance.h"
#include "Goblin.h"

CGoblin_Dagger::CGoblin_Dagger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CGoblin_Dagger::CGoblin_Dagger(const CGoblin_Dagger& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CGoblin_Dagger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Dagger::Initialize(void* pArg)
{
	GOBLINDAGGER_DESC* pDesc = static_cast<GOBLINDAGGER_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Dagger::Priority_Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();
	if (m_bAttach)
	{
		_matrix socketMatrix = {};

		socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}

		m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
		_float4 vTargetPos = static_cast<CGoblin*>(m_pOwner)->Get_TargetPos();
		m_pTransformCom->LookAt(XMLoadFloat4(&vTargetPos));
	}
	else
	{
		_matrix world = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
		m_pTransformCom->Set_WorldMatrix(world);
		m_pTransformCom->Go_Straight(fTimeDelta);
	}


#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CGoblin_Dagger::Update(_float fTimeDelta)
{
}

void CGoblin_Dagger::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGoblin_Dagger::Render()
{
	if (!m_pModelCom)
		return S_OK;

	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
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

HRESULT CGoblin_Dagger::Ready_Components()
{

	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 25.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Goblin_Dagger_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoblin_Dagger::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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


CGoblin_Dagger* CGoblin_Dagger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Dagger* pInstance = new CGoblin_Dagger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Dagger");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CGoblin_Dagger::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Dagger* pInstance = new CGoblin_Dagger(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Dagger");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGoblin_Dagger::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CGoblin_Dagger::Describe_Entity()
{
}

#endif // _DEBUG
