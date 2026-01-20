#include "pch.h"
#include "MapObject_Collision.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapObject_Collision::CMapObject_Collision(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapPartObject(pDevice, pContext)
{
}

CMapObject_Collision::CMapObject_Collision(const CMapObject_Collision& Prototype)
	: CMapPartObject(Prototype)
{
}

void CMapObject_Collision::Priority_Update(_float fTimeDelta)
{

}

void CMapObject_Collision::Update(_float fTimeDelta)
{

}

void CMapObject_Collision::Late_Update(_float fTimeDelta)
{
	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(XMLoadFloat4((_float4*)&m_CombinedWorldMatrix.m[3][0]), m_fRadius));
}

HRESULT CMapObject_Collision::Render()
{
	return S_OK;
}

HRESULT CMapObject_Collision::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	_uint iMeshes = m_pModelComs[0]->Get_NumMeshes();

	for (_uint i = 0; i < iMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SHADOW)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[0]->Render(i))) {
			return E_FAIL;
		}if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
			return E_FAIL;
		}
		_uint iMeshes = m_pModelComs[0]->Get_NumMeshes();

		for (_uint i = 0; i < iMeshes; i++)
		{
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SHADOW)))) {
				return E_FAIL;
			}

			if (FAILED(m_pModelComs[0]->Render(i))) {
				return E_FAIL;
			}
		}

		return S_OK;
	}

	return S_OK;

	return S_OK;
}

HRESULT CMapObject_Collision::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject_Collision::Initialize(void* pArg)
{
	m_iMaxLodLevel = 0;

	MAPOBJECT_DESC* pDesc = static_cast<MAPOBJECT_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[iIndex]);
	}
	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}

	if (FAILED(Ready_Components())){
		return E_FAIL;
	}

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_XMWorldMatrix() * m_pParentTransformCom->Get_XMWorldMatrix());

	ReadyForPhysX();
	ConvertToPhysX();

	m_fRadius = m_pModelComs[0]->Get_Radius();

	return S_OK;
}

_wstring CMapObject_Collision::Get_PrototypeTag(_uint iLodIndex)
{
	if (m_iMaxLodLevel < iLodIndex){
		return m_ModelPrototypeTags[0];
	}

	return m_ModelPrototypeTags[iLodIndex];
}

void CMapObject_Collision::ReadyForPhysX()
{
	if (true == m_bConverted) {
		return;
	}
	m_bConverted = true;
	_uint iLevel = NEXT_LEVEL;

	CModel* pModel = m_pModelComs[0];
	_matrix parentMatrix = m_pParentTransformCom->Get_XMWorldMatrix();
	_matrix idenMatrix = XMMatrixIdentity();
	_bool bSkip = { false };
	for (_int i = 0; i < 4; ++i) {
		if (false == XMVector4NearEqual(idenMatrix.r[i], parentMatrix.r[i], XMVectorReplicate(FLT_EPSILON5))) {
			if (FAILED(pModel->Ready_PhysXMeshes(XMLoadFloat4x4(&m_CombinedWorldMatrix), iLevel))) {
				assert(false);
			}
			bSkip = true;
			break;
		}
	}
	if (false == bSkip) {
		if (FAILED(pModel->Ready_PhysXMeshes(m_pParentTransformCom->Get_XMWorldMatrix(), iLevel))) {
			assert(false);
		}
	}
}

void CMapObject_Collision::ConvertToPhysX()
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

HRESULT CMapObject_Collision::Ready_Components()
{
	__super::Ready_Components();

	for (_uint iIndexLOD = 0; iIndexLOD < m_iMaxLodLevel + 1; ++iIndexLOD)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, m_ModelPrototypeTags[iIndexLOD],
			reinterpret_cast<CComponent**>(&pModel)))){
			return E_FAIL;
		}

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMapObject_Collision::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
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

CMapObject_Collision* CMapObject_Collision::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject_Collision* pInstance = new CMapObject_Collision(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapObject_Static");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Collision::Clone(void* pArg, CGameObject* pOwner)
{
	CMapObject_Collision* pInstance = new CMapObject_Collision(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject_Collision");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapObject_Collision::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs){
		SAFE_RELEASE(pModel);
	} m_pModelComs.clear();

	for (_uint i = 0; i < m_RigidBodies.size(); ++i) {
		SAFE_RELEASE(m_RigidBodies[i]);
	} m_RigidBodies.clear();

	m_ModelPrototypeTags.clear();
	m_ModelPathIndices.clear();
}
#ifdef _DEBUG
void CMapObject_Collision::Describe_Entity()
{

}
#endif // _DEBUG
