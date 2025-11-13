#include "pch.h"
#include "MapObject_LOD.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#include "MapObject_Manager.h"

CMapObject_LOD::CMapObject_LOD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapObject_LOD::CMapObject_LOD(const CMapObject_LOD& Prototype)
	: CMapObject(Prototype), m_iLodIndex{ 0 }
{
}

HRESULT CMapObject_LOD::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject_LOD::Initialize(void* pArg)
{
	MAPOBJECT_LOD_DESC* pDesc = static_cast<MAPOBJECT_LOD_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotation = pDesc->vRotation;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
#endif // _DEBUG

	return S_OK;
}

void CMapObject_LOD::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapObject_LOD::Update(_float fTimeDelta)
{

}

void CMapObject_LOD::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_bSelected)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		m_pTransformCom->Set_Scale(m_vScale);
		m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	}
#endif 

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_XMWorldMatrix() * m_pParentTransformCom->Get_XMWorldMatrix());

	_float4 vPos;
	//XMStoreFloat4(&vPos, Get_WorldPostion());
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this, vPos, 20.f);
}

HRESULT CMapObject_LOD::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0))) {
			return E_FAIL;
		}
		if (m_bSelected)
		{
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::MAPTOOL)))) {
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
				return E_FAIL;
			}
		}
		

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

_wstring CMapObject_LOD::Get_PrototypeTag(_uint iLodIndex)
{
	if (m_iMaxLodLevel < iLodIndex)
		return m_ModelPrototypeTags[0];

	return m_ModelPrototypeTags[iLodIndex];
}

HRESULT CMapObject_LOD::Add_LodModel(const _tchar* pModelPrototypeTag)
{
	CModel* pModel = { nullptr };

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pModelPrototypeTag,
		reinterpret_cast<CComponent**>(&pModel))))
		return E_FAIL;

	m_ModelPrototypeTags.push_back(pModelPrototypeTag);

	++m_iMaxLodLevel;
	m_pModelComs.push_back(pModel);

	return S_OK;
}

HRESULT CMapObject_LOD::Ready_Components()
{
	__super::Ready_Components();

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

	return S_OK;
}

HRESULT CMapObject_LOD::Bind_ShaderResources()
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

CMapObject_LOD* CMapObject_LOD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject_LOD* pInstance = new CMapObject_LOD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapObject_LOD");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_LOD::Clone(void* pArg, CGameObject * pOwner)
{
	CMapObject_LOD* pInstance = new CMapObject_LOD(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject_LOD");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapObject_LOD::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for(auto& pModel : m_pModelComs)
		Safe_Release(pModel);
	m_pModelComs.clear();
}

void CMapObject_LOD::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;

	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));
	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);
	GUI::Text("----- Transfrom ----");
	GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		CTerrain* pTerrain = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Terrain"))->Get_Object<CTerrain>();
		if (nullptr != pTerrain)
		{
			pTerrain->Get_Component<CVIBuffer_Terrain>()->Picking(pTerrain->Get_Component<CTransform>(), m_vPosition);
		}

	}

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);


	if (GUI::Button("Delete"))
		m_bDead = true;

	if (GUI::Button("Bake_StaticPhysXMesh")) {
		GUI::OpenPopup("BakeStaticPhysXMesh##popup");
	}

	if (GUI::BeginPopup("BakeStaticPhysXMesh##popup")) {
		if (GUI::Button("No")) {
			GUI::CloseCurrentPopup();
		}
		GUI::SameLine();
		if (GUI::Button("Bake!!!")) {
			CMapObject_Manager* pManager = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_MAPOBJECTMANAGER)->Get_Object<CMapObject_Manager>();
			for (_uint i = 0; i < (_uint)m_ModelPrototypeTags.size(); ++i) {
				filesystem::path filePath = pManager->Get_PrototypePath(m_ModelPathIndices[i]);
				
				CModel* pModel = m_pModelComs[i];
				_uint iNumMesh = pModel->Get_NumMeshes();
				pModel->Save_PhysXTriMeshes(filePath.string().c_str());


				CRigidBody::RIGIDBODY_DESC Desc{};
				Desc.eType = ACTOR::TRIANGLEMESH;

				for (_uint i = 0; i < iNumMesh; ++i)
				{ // RIGID_BODY
					Desc.tRigidStaticDesc.szMeshName = pModel->Get_MeshName(i);

					if (FAILED(Add_Asset_Component(g_iStaticLevel, CMyTools::ToWstring(pModel->Get_MeshName(i)).c_str(), nullptr, &Desc))) {
						assert(false);
					}
				}
			}
			GUI::CloseCurrentPopup();
		}
		GUI::EndPopup();
	}

	m_bSelected = true;
}