#include "pch.h"
#include "LodingWidget1.h"
#include "GameInstance.h"

CLodingWidget1::CLodingWidget1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CLodingWidget1::CLodingWidget1(const CLodingWidget1& rhs)
    :CUIObject(rhs)
{
}

HRESULT CLodingWidget1::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLodingWidget1::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 1800.f;
	Desc.fY = 850.f;
	Desc.fSizeX = 200.f;
	Desc.fSizeY = 200.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_iImageFrameX = 6;
	m_fFrame = 0.2f;

	return S_OK;
}

void CLodingWidget1::Priority_Update(_float fTimeDelta)
{
}

void CLodingWidget1::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta * 3.f;
}

void CLodingWidget1::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
}

HRESULT CLodingWidget1::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::LODING)))) {
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

_vector CLodingWidget1::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLodingWidget1::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrame", &m_fFrame, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iImageCount", &m_iImageFrameX, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLodingWidget1::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("LodingWidget1"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLodingWidget1* CLodingWidget1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLodingWidget1* pInstance = new CLodingWidget1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLodingWidget1");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLodingWidget1::Clone(void* pArg, CGameObject* pOwner)
{
	CLodingWidget1* pInstance = new CLodingWidget1(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLodingWidget1");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLodingWidget1::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CLodingWidget1::Describe_Entity()
{
}
