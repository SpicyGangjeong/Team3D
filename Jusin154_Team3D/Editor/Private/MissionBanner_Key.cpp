#include "pch.h"
#include "MissionBanner_Key.h"
#include "GameInstance.h"
#include "Mission_KeyHold.h"

CMissionBanner_Key::CMissionBanner_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject(pDevice, pContext)
{
}

CMissionBanner_Key::CMissionBanner_Key(const CMissionBanner_Key& rhs)
    :CUIObject(rhs)
{
}

HRESULT CMissionBanner_Key::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMissionBanner_Key::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 140.f;
	Desc.fY = 750.f;
	Desc.fSizeX = 192.f;
	Desc.fSizeY = 96.f;

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

void CMissionBanner_Key::Priority_Update(_float fTimeDelta)
{
}

void CMissionBanner_Key::Update(_float fTimeDelta)
{
}

void CMissionBanner_Key::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
}

HRESULT CMissionBanner_Key::Render()
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

_vector CMissionBanner_Key::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMissionBanner_Key::Bind_ShaderResources()
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

HRESULT CMissionBanner_Key::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("MissionBanner_Key"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMission_KeyHold>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, &m_pMission_KeyHold)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CMissionBanner_Key* CMissionBanner_Key::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMissionBanner_Key* pInstance = new CMissionBanner_Key(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMissionBanner_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMissionBanner_Key::Clone(void* pArg, CGameObject* pOwner)
{
	CMissionBanner_Key* pInstance = new CMissionBanner_Key(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMissionBanner_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMissionBanner_Key::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMissionBanner_Key::Describe_Entity()
{
}
