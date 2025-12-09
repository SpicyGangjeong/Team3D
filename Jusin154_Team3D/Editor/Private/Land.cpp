#include "pch.h"
#include "Land.h"

#include "GameInstance.h"

CLand::CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLand::CLand(const CLand& rhs)
	: CGameObject(rhs), m_fUsingSurfaceParams{rhs.m_fUsingSurfaceParams}
{
}

void CLand::Priority_Update(_float fTimeDelta)
{
}

void CLand::Update(_float fTimeDelta)
{
	if (m_bEdit)
		Describe_Entity();
}

void CLand::Late_Update(_float fTimeDelta)
{
	if(m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pModelCom->Get_Radius()))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	else
	{
		_uint i = 0;
	}
}

HRESULT CLand::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(17))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLand::Initialize(void* pArg)
{
	m_vScale = _float3(1.f, 1.f, 1.f);
	m_vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;




	return S_OK;
}

HRESULT CLand::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	LAND_DESC* pDesc = static_cast<LAND_DESC*>(pArg);

	m_bEdit = pDesc->bEdit;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strModelComTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLand::Bind_ShaderResources()
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

CLand* CLand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLand* pInstance = new CLand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLand::Clone(void* pArg, CGameObject* pOwner)
{
	CLand* pInstance = new CLand(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CLand::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CLand::Describe_Entity()
{
	GUI::Begin("Land Debug");

	XMStoreFloat4(&m_vPosition, m_pTransformCom->Get_State(STATE::POSITION));
	GUI::DragFloat3("LandPos", (_float*)(&m_vPosition));
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_vPosition));


	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	m_pTransformCom->Set_Scale(m_vScale);

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

	GUI::End();
}
