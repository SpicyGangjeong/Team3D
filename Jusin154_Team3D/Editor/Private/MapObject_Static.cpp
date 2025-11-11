#include "pch.h"
#include "MapObject_Static.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "Terrain.h"
#include "VIBuffer_Terrain.h"

CMapObject_Static::CMapObject_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapObject_Static::CMapObject_Static(const CMapObject_Static& Prototype)
	: CMapObject(Prototype)
{
}

void CMapObject_Static::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapObject_Static::Update(_float fTimeDelta)
{

}

void CMapObject_Static::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_bSelected)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
		m_pTransformCom->Set_Scale(m_vScale);
		m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	}
#endif 

	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this, vPos, 20.f);
}

HRESULT CMapObject_Static::Render()
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

HRESULT CMapObject_Static::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject_Static::Initialize(void* pArg)
{
	MAPOBJECT_STATIC_DESC* pDesc = static_cast<MAPOBJECT_STATIC_DESC*>(pArg);

	m_strModelPrototypeTag = pDesc->pModelPrototypeTag;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = _float3(0.f, 0.f, 0.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);
	m_vScale = _float3(1.f, 1.f, 1.f);
#endif // _DEBUG

	return S_OK;
}

_wstring CMapObject_Static::Get_PrototypeTag(_uint iLodIndex)
{
	return m_strModelPrototypeTag;
}

HRESULT CMapObject_Static::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject_Static::Bind_ShaderResources()
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
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_bRimLight", &m_bRimLight, sizeof(_bool)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimStrength", &m_fRimLightStrength, sizeof(_float)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimLightPower, sizeof(_float)))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimLightColor, sizeof(_float3)))) {
	//	return E_FAIL;
	//}
	return S_OK;
}

CMapObject_Static* CMapObject_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject_Static* pInstance = new CMapObject_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapObject_Static");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Static::Clone(void* pArg, CGameObject* pOwner)
{
	CMapObject_Static* pInstance = new CMapObject_Static(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject_Static");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapObject_Static::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

void CMapObject_Static::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;

	//ImGui::Text(CMyTools::ToString(m_strModelPrototypeTag).c_str());
	ImGui::Text("----- Transfrom ----");
	ImGui::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	ImGui::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	ImGui::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		CTerrain* pTerrain = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Terrain"))->Get_Object<CTerrain>();
		if (nullptr != pTerrain)
		{
			pTerrain->Get_Component<CVIBuffer_Terrain>()->Picking(pTerrain->Get_Component<CTransform>(), m_vPosition);
		}

	}

	ImGui::Text("----- Rotation ----");
	ImGui::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
	ImGui::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
	ImGui::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

	ImGui::Text("----- Scale ----");
	ImGui::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	ImGui::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	ImGui::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);


	if (ImGui::Button("Delete"))
		m_bDead = true;

	m_bSelected = true;
}