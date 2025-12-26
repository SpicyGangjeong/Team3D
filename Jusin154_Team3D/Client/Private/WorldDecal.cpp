#include "pch.h"
#include "WorldDecal.h"

#include "GameInstance.h"

CWorldDecal::CWorldDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWorldDecal::CWorldDecal(const CWorldDecal& rhs)
	: CGameObject(rhs)
	, m_fWinSizeX{ rhs.m_fWinSizeX }
	, m_fWinSizeY{ rhs.m_fWinSizeY }
{
}

HRESULT CWorldDecal::Initialize_Prototype()
{
	m_fWinSizeX = g_iWinSizeX;
	m_fWinSizeY = g_iWinSizeY;

	return S_OK;
}

HRESULT CWorldDecal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	
	WORLD_DECAL_DESC* pDesc = static_cast<WORLD_DECAL_DESC*>(pArg);
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

	m_fUVTiling = pDesc->fUVTiling;
	m_vUVSpeed = pDesc->vUVSpeed;
	m_vMaskRed = pDesc->vMaskRed;
	m_vMaskGreen = pDesc->vMaskGreen;
	m_vMaskBlue = pDesc->vMaskBlue;

	_float fBoxRadius = 0.86f; // 1,1,1 기준
	_float fMaxScale = max(pDesc->vScale.x, max(pDesc->vScale.y, pDesc->vScale.z)) * fBoxRadius;

	m_pTransformCom->Set_Radius(fMaxScale);

	XMStoreFloat4x4(&m_WorldMatrixInv, m_pTransformCom->Get_WorldMatrixInv());

	return S_OK;
}

void CWorldDecal::Priority_Update(_float fTimeDelta)
{

}

void CWorldDecal::Update(_float fTimeDelta)
{
	m_fTimeAcc.x += fTimeDelta * m_vUVSpeed.x;
	m_fTimeAcc.y += fTimeDelta * m_vUVSpeed.y;
}

void CWorldDecal::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::DECAL, this);
	}
}

HRESULT CWorldDecal::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pShaderCom->Bind_SRV("g_MaskingTexture", m_pMaskTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_SurfaceParamsTexture", m_pSurfaceTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_NoiseTexture", m_pFadeTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_SRV("g_DiffsueMaskTexture", m_pDiffuseMaskTextureCom->Get_SRV(0)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DECAL)))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CWorldDecal::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_MSK"),
		reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_Noraml"),
		reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Base_MRO"),
		reinterpret_cast<CComponent**>(&m_pSurfaceTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_Fade"),
		reinterpret_cast<CComponent**>(&m_pFadeTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Decal_DiffuseMask"),
		reinterpret_cast<CComponent**>(&m_pDiffuseMaskTextureCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_DecalBox"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldDecal::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrixInv", &m_WorldMatrixInv))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWinSizeX", &m_fWinSizeX, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWinSizeY", &m_fWinSizeY, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTime", &m_fTimeAcc, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVTiling", &m_fUVTiling, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColorRed", &m_vMaskRed, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColorGreen", &m_vMaskGreen, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskColorBlue", &m_vMaskBlue, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_RenderTarget(TEXT("Target_NormalCopy"), m_pShaderCom, "g_NormalMapTexture"))) {
		return E_FAIL;
	}

	return S_OK;
}

CWorldDecal* CWorldDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWorldDecal* pInstance = new CWorldDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWorldDecal");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CWorldDecal::Clone(void* pArg, CGameObject* pOwner)
{
	CWorldDecal* pInstance = new CWorldDecal(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWorldDecal");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CWorldDecal::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuseMaskTextureCom);
	SAFE_RELEASE(m_pFadeTextureCom);
	SAFE_RELEASE(m_pMaskTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pSurfaceTextureCom);
	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}

#ifdef _DEBUG
void CWorldDecal::Describe_Entity()
{
}
#endif // _DEBUG
