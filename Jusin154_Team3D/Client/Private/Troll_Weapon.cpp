#include "pch.h"
#include "Troll_Weapon.h"

#include "GameInstance.h"
#include "TrailObject.h"
#include "EffectParts.h"

CTroll_Weapon::CTroll_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CTroll_Weapon::CTroll_Weapon(const CTroll_Weapon& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CTroll_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTroll_Weapon::Initialize(void* pArg)
{
	TROLLWEAPON_DESC* pDesc = static_cast<TROLLWEAPON_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CTroll_Weapon::Priority_Update(_float fTimeDelta)
{	
	m_pModelCom->Combined_BoneMatrix();
	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CTroll_Weapon::Update(_float fTimeDelta)
{


}

void CTroll_Weapon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CTroll_Weapon::Render()
{
	if (!m_pModelCom)
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

HRESULT CTroll_Weapon::Ready_Components()
{
	__super::Ready_Components(nullptr);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Troll_Weapon_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTroll_Weapon::Bind_ShaderResources()
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


CTroll_Weapon* CTroll_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Weapon* pInstance = new CTroll_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Weapon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTroll_Weapon::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Weapon* pInstance = new CTroll_Weapon(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Weapon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Weapon::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CTroll_Weapon::Describe_Entity()
{
}

#endif // _DEBUG
