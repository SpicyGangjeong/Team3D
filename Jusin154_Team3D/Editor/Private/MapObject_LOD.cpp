#include "pch.h"
#include "MapObject_LOD.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Layer.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#include "MapObject_Manager.h"
#include "Character_Controller.h"
#include "Bounding_Sphere.h"
#include "Collider.h"

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

	//m_Type = static_cast<MAPOBJECT_RENDER_TYPE>(pDesc->iRenderType);
	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}
	
	if (_wstring::npos != m_ModelPrototypeTags.front().find(L"Glass"))
		m_iShaderPass = 12;
	else
		m_iShaderPass = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);

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

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_XMWorldMatrix() * m_pParentTransformCom->Get_XMWorldMatrix());

	_float3 vOffset = m_pModelComs[0]->Get_RadiusOffset();
	_matrix ColliderMatrix = XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
	XMStoreFloat4(&m_vExtentPosition, ColliderMatrix.r[3]);


	
	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);


	//m_pColliderCom->Update(ColliderMatrix);

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
	/*if (0 == m_iLodIndex)
		m_bSelected = true;*/

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
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

	_float3 vOffset = m_pModelComs[0]->Get_RadiusOffset();
	_matrix ColliderMatrix = XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
	XMStoreFloat4(&m_vExtentPosition, ColliderMatrix.r[3]);

	_float fRaius = m_pModelComs[0]->Get_Radius();
	if (m_pGameInstance->isIn_WorldFrustum(XMLoadFloat4(&m_vExtentPosition), fRaius)) {
		
		_vector		vCamPosition = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

		m_fCamDepth = XMVectorGetX(XMVector3LengthSq(vCamPosition - XMLoadFloat4(&m_vExtentPosition)));

		m_iLodIndex = min(m_iMaxLodLevel, (_uint)(m_fCamDepth / (fRaius * fRaius + 3600.f)));

		if (m_pGameInstance->Key_Pressing(DIK_Z))
			m_iLodIndex = 0;

		switch (m_Type)
		{
		case Editor::MAPOBJECT_RENDER_TYPE::NORMAL:
			m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
			break;

		case Editor::MAPOBJECT_RENDER_TYPE::DECAL:
			m_pGameInstance->Add_RenderGroup(RENDER::DECAL, this);
			break;
		case Editor::MAPOBJECT_RENDER_TYPE::GLASS:
			break;
		default:
			break;
		}
	}
}

HRESULT CMapObject_LOD::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
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
			if (FAILED(m_pShaderCom->Begin(m_iShaderPass))) {
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

	SAFE_RELEASE(m_pShaderCom);

	for(auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
	m_pModelComs.clear();

	m_ModelPrototypeTags.clear();
	m_ModelPathIndices.clear();
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
			pTerrain->Get_Component<CVIBuffer_Terrain>()->Picking(pTerrain->Get_Component<CTransform>()->Get_XMWorldMatrix(), &m_vPosition);
		}

	}
	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			memcpy(&m_vPosition, &vPosition, sizeof(_float3));
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
			for (_uint iModelIndex = 0; iModelIndex < (_uint)m_ModelPathIndices.size(); ++iModelIndex) {
				filesystem::path filePath = pManager->Get_PrototypePath(m_ModelPathIndices[iModelIndex]);
				
				CModel* pModel = m_pModelComs[iModelIndex];
				_uint iNumMesh = pModel->Get_NumMeshes();
				pModel->Save_PhysXTriMeshes(filePath.string().c_str());

				CRigidBody_Static::RIGIDBODY_STATIC_DESC Desc{};

				for (_uint iMeshIndex = 0; iMeshIndex < iNumMesh; ++iMeshIndex)
				{ // RIGID_BODY
					_wstring dest = CMyTools::ToWstring(m_pModelComs[iModelIndex]->Get_MeshName(iMeshIndex) + to_string(iMeshIndex)).c_str();
					Desc.pMeshName = dest.c_str();
					Desc.iSubKind = 998;

					if (FAILED(Add_Asset_Component(g_iStaticLevel, Desc.pMeshName, nullptr, &Desc))) {
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
