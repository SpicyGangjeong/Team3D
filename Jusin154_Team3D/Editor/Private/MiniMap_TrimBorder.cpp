#include "pch.h"
#include "MiniMap_TrimBorder.h"
#include "GameInstance.h"

CMiniMap_TrimBorder::CMiniMap_TrimBorder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject(pDevice, pContext)
{
}

CMiniMap_TrimBorder::CMiniMap_TrimBorder(const CMiniMap_TrimBorder& rhs)
	:CUIObject(rhs)
{
}

HRESULT CMiniMap_TrimBorder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniMap_TrimBorder::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 140.f;
	Desc.fY = 920.f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 256.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMiniMap_TrimBorder::Priority_Update(_float fTimeDelta)
{
}

void CMiniMap_TrimBorder::Update(_float fTimeDelta)
{
}

void CMiniMap_TrimBorder::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
}

HRESULT CMiniMap_TrimBorder::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ALPHABLEND)))) {
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

_vector CMiniMap_TrimBorder::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMiniMap_TrimBorder::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMiniMap_TrimBorder::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("HUD_MiniMap_TrimBorder"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMiniMap_TrimBorder* CMiniMap_TrimBorder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniMap_TrimBorder* pInstance = new CMiniMap_TrimBorder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMiniMap_TrimBorder");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniMap_TrimBorder::Clone(void* pArg, CGameObject* pOwner)
{
	CMiniMap_TrimBorder* pInstance = new CMiniMap_TrimBorder(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMiniMap_TrimBorder");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMiniMap_TrimBorder::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMiniMap_TrimBorder::Describe_Entity()
{
}
