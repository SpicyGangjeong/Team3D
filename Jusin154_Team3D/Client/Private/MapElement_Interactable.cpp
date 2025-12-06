#include "pch.h"
#include "MapElement_Interactable.h"

#include "GameInstance.h"

CMapElement_Interactable::CMapElement_Interactable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Interactable::CMapElement_Interactable(const CMapElement_Interactable& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Interactable::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Interactable::Initialize(void* pArg)
{
	ELEMENT_INTERACTABLE_DESC* pDesc = static_cast<ELEMENT_INTERACTABLE_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
	
	m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());

	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_HalfGeometryInfo(pDesc->vBoxSize);
	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Move_LocalPos(_float4(0.f, 0.f, 0.f, 0.f), pDesc->vBoxLocalPosition);

	return S_OK;
}

void CMapElement_Interactable::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Interactable::Update(_float fTimeDelta)
{
}

void CMapElement_Interactable::Late_Update(_float fTimeDelta)
{

	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Interactable::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapElement_Interactable::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	ELEMENT_INTERACTABLE_DESC* pPhysXDummyDesc = static_cast<ELEMENT_INTERACTABLE_DESC*>(pArg);

	// RIGID_BODY
	CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);//pPhysXDummyDesc->iSubKind;
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX_KIN"), (CComponent**)&m_pRigidBody, &Desc))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMapElement_Interactable::Bind_ShaderResources()
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

CMapElement_Interactable* CMapElement_Interactable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Interactable* pInstance = new CMapElement_Interactable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Interactable");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Interactable::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Interactable* pInstance = new CMapElement_Interactable(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Interactable");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Interactable::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Interactable::Describe_Entity()
{

}
#endif // _DEBUG


