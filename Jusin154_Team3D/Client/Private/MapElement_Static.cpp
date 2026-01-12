#include "pch.h"
#include "MapElement_Static.h"

#include "GameInstance.h"

#include "Layer.h"

CMapElement_Static::CMapElement_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Static::CMapElement_Static(const CMapElement_Static& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Static::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Static::Initialize(void* pArg)
{
	MAPELEMENT_DESC* pDesc = static_cast<MAPELEMENT_DESC*>(pArg);

	m_bVisible = pDesc->bVisible;
	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

	_float fMaxScale = max(max(pDesc->vScale.x, pDesc->vScale.y), pDesc->vScale.z);

	m_fRadius = m_pModelComs[0]->Get_Radius() * fMaxScale;

	_float3 vOffset = m_pModelComs[0]->Get_RadiusOffset();
	XMStoreFloat3(&m_vWorldCenterPosition, XMVector3TransformCoord(XMLoadFloat3(&vOffset), m_pTransformCom->Get_XMWorldMatrix()));

	ReadyForPhysX();
	ConvertToPhysX();

	return S_OK;
}

void CMapElement_Static::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Static::Update(_float fTimeDelta)
{
}

void CMapElement_Static::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(XMVectorSetW(XMLoadFloat3(&m_vWorldCenterPosition), 1.f), m_fRadius)) {
		if(m_bVisible)
			m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Static::Render()
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

	return S_OK;
}

HRESULT CMapElement_Static::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Static::Bind_ShaderResources()
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

void CMapElement_Static::ReadyForPhysX()
{
	if (true == m_bConverted) {
		return;
	}
	m_bConverted = true;
	_uint iLevel = NEXT_LEVEL;

	CModel* pModel = m_pModelComs[0];

	if (FAILED(pModel->Ready_PhysXMeshes(XMMatrixIdentity(), iLevel))) {
		assert(false);
	}
}

void CMapElement_Static::ConvertToPhysX()
{
	if (true == m_bReadyToCreatePhysX) {
		return;
	}
	m_bReadyToCreatePhysX = true;

	CModel* pModel = m_pModelComs[0];

	_uint iNumMeshes = pModel->Get_NumMeshes();

	for (_uint iIndex = 0; iIndex < iNumMeshes; ++iIndex)
	{
		_wstring wstrName = CMyTools::ToWstring(pModel->Get_MeshName(iIndex)) + to_wstring(iIndex);
		CRigidBody_Static* pRigidBody = { nullptr };
		CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc = {};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);
		Desc.pMeshName = wstrName.c_str();
		Desc.pWorldMatrix = m_pTransformCom->Get_WorldMatrixPtr();
		if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, wstrName, (CComponent**)&pRigidBody, &Desc))) {
			assert(false);
		}
		m_RigidBodies.push_back(pRigidBody);
	}
}

CMapElement_Static* CMapElement_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Static* pInstance = new CMapElement_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Static");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Static::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Static* pInstance = new CMapElement_Static(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Static");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Static::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);

	for (_uint i = 0; i < m_RigidBodies.size(); ++i) {
		SAFE_RELEASE(m_RigidBodies[i]);
	} m_RigidBodies.clear();

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Static::Describe_Entity()
{

}
#endif // _DEBUG
