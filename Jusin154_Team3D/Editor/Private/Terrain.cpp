#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "VIBuffer_Terrain.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vRotation = _float3{ 0.f, 0.f, 0.f };
	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-128.f, -100.f, -128.f, 1.f));

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{
	GUI::Begin("Picking Position");
	if (m_pGameInstance->Mouse_Pressing(0))
	{
		if (m_pGameInstance->isPicking(&m_vPickingPosition))
		{
			m_pVIBufferCom->FitY(m_pTransformCom->Get_XMWorldMatrix(), m_vPickingPosition.y);
		}
	}
	
	GUI::InputFloat("Height Ratio", &m_fHeightRatio);
	max(0.01f, m_fHeightRatio);

	//m_pVIBufferCom->Change_HeigthRatio(m_fHeightRatio);
	GUI::DragFloat3("Picking", (_float*)(&m_vPickingPosition));
	_float4 vPos = {};
	_float3 vRotation = {};
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
	GUI::DragFloat3("Pos", (_float*)(&vPos));
	GUI::DragFloat3("Rota", (_float*)(&m_vRotation));

	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vPos));

	GUI::End();
}

void CTerrain::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Culling(XMMatrixIdentity());

}

void CTerrain::Late_Update(_float fTimeDelta)
{
	//if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//}
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::DEFAULT)))) {
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
		reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NORTEX,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("TerrainTest"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}

	return S_OK;
}

CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg, CGameObject* pOwner)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pTextureCom);
}

void CTerrain::Describe_Entity()
{

}
