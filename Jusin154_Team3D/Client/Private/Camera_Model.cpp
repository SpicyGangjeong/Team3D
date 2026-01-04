#include "pch.h"
#include "Camera_Model.h"
#include "Shader.h"
#include "Layer.h"
#include "Player.h"


CCamera_Model::CCamera_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Model::CCamera_Model(const CCamera_Model& rhs)
	: CCamera(rhs)
{
}

void CCamera_Model::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_MODEL, false);
#endif // _DEBUG

	if (FAILED(m_pGameInstance->IsBinded_Camera(CAMERA_MODEL))) {
		return;
	}
	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	_float3 vCamPos = {};
	XMStoreFloat3(&vCamPos, m_pTransformCom->Get_State(STATE::POSITION));
#ifdef _DEBUG
	GUI::Text("Cam_Model Coord %.2f, %.2f, %.2f", vCamPos.x, vCamPos.y, vCamPos.z);
#endif // _DEBUG

	__super::Bind_Matrices();
}

void CCamera_Model::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CCamera_Model::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
}

HRESULT CCamera_Model::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);
		

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CCamera_Model::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Model::Initialize(void* pArg)
{
	CAMERA_MODEL_DESC* pDesc = static_cast<CAMERA_MODEL_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	m_matInitial = *m_pTransformCom->Get_WorldMatrixPtr();
	m_bActive = true;

	return S_OK;
}

HRESULT CCamera_Model::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Camera_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}


	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CCamera_Model::Bind_ShaderResources()
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

void CCamera_Model::Set_InitialPos()
{
	m_pTransformCom->Set_WorldMatrix(m_matInitial);
}

CCamera_Model* CCamera_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Model* pInstance = new CCamera_Model(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Model");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Model::Clone(void* pArg, CGameObject* pOwner)
{
	CCamera_Model* pInstance = new CCamera_Model(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Model");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCamera_Model::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}
#ifdef _DEBUG

void CCamera_Model::Describe_Entity()
{
	_float3 vPosition = {};
	GUI::Begin("CameraAnim List");
	for (_int i = 0; i < m_pModelCom->Get_AnimSize(); i++)
	{
		if (GUI::Button(m_pModelCom->Get_AnimList(i)))
		{
			m_pModelCom->Set_AnimationIndex(i);
		}
	}
	

	GUI::End();
}

#endif // _DEBUG
