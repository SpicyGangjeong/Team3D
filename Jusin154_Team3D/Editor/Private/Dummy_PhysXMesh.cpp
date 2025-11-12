#include "pch.h"
#include "Dummy_PhysXMesh.h"

#include "GameInstance.h"


CDummy_PhysXMesh::CDummy_PhysXMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_PhysXMesh::CDummy_PhysXMesh(const CDummy_PhysXMesh& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_PhysXMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXMesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXMesh::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F2)) {
		CModel* pModel = Get_Component<CModel>();
		_uint iNumMesh = pModel->Get_NumMeshes();
		pModel->Save_PhysXTriMeshes("../Bin/Resources/Models/River/River.bin");

		CRigidBody::RIGIDBODY_DESC Desc{};
		Desc.eType = ACTOR::TRIANGLEMESH;
		for (_uint i = 0; i < iNumMesh; ++i)
		{ // RIGID_BODY
			Desc.tRigidStaticDesc.szMeshName = pModel->Get_MeshName(i);

			Add_Asset_Component(g_iStaticLevel, CMyTools::ToWstring(pModel->Get_MeshName(i)).c_str(), nullptr, &Desc);
		}
	}
	if (m_pGameInstance->Key_Down(DIK_F3)) {
		CModel* pModel = Get_Component<CModel>();
		_uint iNumMesh = pModel->Get_NumMeshes();
		CRigidBody::RIGIDBODY_DESC Desc{};
		Desc.eType = ACTOR::TRIANGLEMESH;
		for (_uint i = 0; i < iNumMesh; ++i)
		{ // RIGID_BODY
			Desc.tRigidStaticDesc.szMeshName = pModel->Get_MeshName(i);

			Add_Asset_Component(g_iStaticLevel, CMyTools::ToWstring(pModel->Get_MeshName(i)).c_str(), nullptr, &Desc);
		}
	}
}

void CDummy_PhysXMesh::Update(_float fTimeDelta)
{
}

void CDummy_PhysXMesh::Late_Update(_float fTimeDelta)
{
	_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this, *vPos, m_pTransformCom->Get_Radius());
}

HRESULT CDummy_PhysXMesh::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CDummy_PhysXMesh::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	BOXSTARTPOS_DESC* pBox = static_cast<BOXSTARTPOS_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pBox->vPos), 1.f));
	m_pTransformCom->Rotation(pBox->vRotRPY.x, pBox->vRotRPY.y, pBox->vRotRPY.z);

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_River_Col_Model"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(20.f, 20.f, 20.f, 1.f));
	return S_OK;
}

HRESULT CDummy_PhysXMesh::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
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

CDummy_PhysXMesh* CDummy_PhysXMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXMesh* pInstance = new CDummy_PhysXMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXMesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXMesh::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXMesh* pInstance = new CDummy_PhysXMesh(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXMesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXMesh::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}

void CDummy_PhysXMesh::Describe_Entity()
{
}
