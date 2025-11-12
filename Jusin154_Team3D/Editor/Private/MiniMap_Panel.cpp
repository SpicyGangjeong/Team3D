#include "pch.h"
#include "MiniMap_Panel.h"
#include "GameInstance.h"

CMiniMap_Panel::CMiniMap_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPanelObject(pDevice, pContext)
{
}

CMiniMap_Panel::CMiniMap_Panel(const CMiniMap_Panel& rhs)
    :CPanelObject(rhs)
{
}

HRESULT CMiniMap_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMiniMap_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 300.f;
	Desc.fY = 300.f;
	Desc.fSizeX = 100.f;
	Desc.fSizeY = 100.f;

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

void CMiniMap_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CMiniMap_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMiniMap_Panel::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CMiniMap_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
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

_vector CMiniMap_Panel::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

HRESULT CMiniMap_Panel::Bind_ShaderResources()
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

HRESULT CMiniMap_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Keyboard_0"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CMiniMap_Panel::Ready_Element(void* pArg)
{
	return S_OK;
}

CMiniMap_Panel* CMiniMap_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniMap_Panel* pInstance = new CMiniMap_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMiniMap_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniMap_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CMiniMap_Panel* pInstance = new CMiniMap_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMiniMap_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMiniMap_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMiniMap_Panel::Describe_Entity()
{
}
