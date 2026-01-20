#include "pch.h"
#include "MapElement_Static.h"

#include "GameInstance.h"

#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

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
	MAPOBJECT_LOD_DESC* pDesc = static_cast<MAPOBJECT_LOD_DESC*>(pArg);

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

#ifdef _DEBUG
	m_isCollision = false;
	m_bSelected = false;
	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotation = pDesc->vRotation;

	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	
	_float fMaxScale = max(m_vScale.z, max(m_vScale.x, m_vScale.y));

	m_fRadius = m_pModelComs[0]->Get_Radius() * fMaxScale;
	_float3 vOffset = m_pModelComs[0]->Get_RadiusOffset();
	XMStoreFloat3(&m_vWorldCenterPosition, XMVector3TransformCoord(XMLoadFloat3(&vOffset), m_pTransformCom->Get_XMWorldMatrix()));
#endif // _DEBUG

	ReadyForPhysX();
	ConvertToPhysX();

	return S_OK;
}

void CMapElement_Static::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapElement_Static::Update(_float fTimeDelta)
{
}

void CMapElement_Static::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->IsIn_WorldFrustum(XMVectorSetW(XMLoadFloat3(&m_vWorldCenterPosition), 1.f), m_fRadius)) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//}
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
		if (m_bSelected || m_isCollision)
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

HRESULT CMapElement_Static::Ready_Components(void* pArg)
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
	_uint iLevel = NEXT_LEVEL;

	CModel* pModel = m_pModelComs[0];

	if (FAILED(pModel->Ready_PhysXMeshes(XMMatrixIdentity(), iLevel))) {
		assert(false);
	}
}

void CMapElement_Static::ConvertToPhysX()
{
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
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, wstrName, (CComponent**)&pRigidBody, &Desc))) {
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

void CMapElement_Static::Describe_Entity()
{
	if (m_bDead)
		return;

	for (_uint i = 0; i < m_RigidBodies.size(); ++i) {
		SAFE_RELEASE(m_RigidBodies[i]);
	} m_RigidBodies.clear();

	if (nullptr == m_pGameInstance)
		return;

	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));
	GUI::Checkbox("isCol", &m_isCollision);
	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);

	_float3 vMove = {};
	GUI::InputFloat("Right", &vMove.x, 0.05f, 0.1f);
	GUI::InputFloat("Up", &vMove.y, 0.05f, 0.1f);
	GUI::InputFloat("Look", &vMove.z, 0.05f, 0.1f);

	m_pTransformCom->Move_Right(vMove.x);
	m_pTransformCom->Move_Up(vMove.y);
	m_pTransformCom->Move_Look(vMove.z);

	XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));


	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			memcpy(&m_vPosition, &vPosition, sizeof(_float3));
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		}
	}

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 1.f, 15.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 1.f, 15.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 1.f, 15.f);

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.05f, 0.1f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.05f, 0.1f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.05f, 0.1f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	m_pTransformCom->Set_Scale(m_vScale);

	if (GUI::Button("Delete"))
		m_bDead = true;

	m_bSelected = true;
}
