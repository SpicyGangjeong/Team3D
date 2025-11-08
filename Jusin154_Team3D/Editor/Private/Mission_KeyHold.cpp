#include "pch.h"
#include "Mission_KeyHold.h"
#include "GameInstance.h"

CMission_KeyHold::CMission_KeyHold(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CMission_KeyHold::CMission_KeyHold(const CMission_KeyHold& rhs)
    :CUIObject(rhs)
{
}

HRESULT CMission_KeyHold::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMission_KeyHold::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 90.f;
	Desc.fY = 747.f;
	Desc.fSizeX = 104.f;
	Desc.fSizeY = 104.f;

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

void CMission_KeyHold::Priority_Update(_float fTimeDelta)
{
}

void CMission_KeyHold::Update(_float fTimeDelta)
{
}

void CMission_KeyHold::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
}

HRESULT CMission_KeyHold::Render()
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

_vector CMission_KeyHold::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMission_KeyHold::Bind_ShaderResources()
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

HRESULT CMission_KeyHold::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Finishi_Rect"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMission_KeyHold* CMission_KeyHold::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMission_KeyHold* pInstance = new CMission_KeyHold(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMission_KeyHold");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMission_KeyHold::Clone(void* pArg, CGameObject* pOwner)
{
	CMission_KeyHold* pInstance = new CMission_KeyHold(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMission_KeyHold");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMission_KeyHold::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMission_KeyHold::Describe_Entity()
{
}
