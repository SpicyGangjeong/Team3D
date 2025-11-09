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

	return S_OK;
}

void CTerrain::Priority_Update(_float fTimeDelta)
{

}

void CTerrain::Update(_float fTimeDelta)
{
	GUI::Begin("Picking Position");
	if (m_pGameInstance->Mouse_Down(0))
		m_pVIBufferCom->Picking(m_pTransformCom, m_vPickingPosition);

	GUI::DragFloat3("Pos", (_float*)(&m_vPickingPosition));
	GUI::End();
}

void CTerrain::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this, vPos, 20.f);
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NORTEX::TERRAIN)))) {
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
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg, CGameObject* pOwner)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}

void CTerrain::Describe_Entity()
{

}