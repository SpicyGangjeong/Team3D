#include "pch.h"
#include "Item_Potion.h"

#include "GameInstance.h"

CItem_Potion::CItem_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CItem_Potion::CItem_Potion(const CItem_Potion& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CItem_Potion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Potion::Initialize(void* pArg)
{
	POTION_DESC* pDesc = static_cast<POTION_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	return S_OK;
}

void CItem_Potion::Priority_Update(_float fTimeDelta)
{

	
#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CItem_Potion::Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();

}

void CItem_Potion::Late_Update(_float fTimeDelta)
{
	if (m_bVisible)
	{
		m_fTimer += fTimeDelta;
		if (m_fTimer >= 1.7f) {
			m_fTimer = 0.f;
			m_bVisible = false;
		}

	}
	if (m_bAttach)
	{
		_matrix socketMatrix = {};

		socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

		for (int i = 0; i < 3; ++i) {
			socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
		}

		m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	}


	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CItem_Potion::Render()
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

void CItem_Potion::Set_Attach(_bool Attach)
{
	m_bAttach = Attach;
}


HRESULT CItem_Potion::Ready_Components()
{
	__super::Ready_Components(nullptr);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Potion_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Potion::Bind_ShaderResources()
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


CItem_Potion* CItem_Potion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Potion* pInstance = new CItem_Potion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_Potion");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Potion::Clone(void* pArg, CGameObject* pOwner)
{
	CItem_Potion* pInstance = new CItem_Potion(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_Potion");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CItem_Potion::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CItem_Potion::Describe_Entity()
{

}

#endif // _DEBUG
