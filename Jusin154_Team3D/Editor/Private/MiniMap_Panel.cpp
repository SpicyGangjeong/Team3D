#include "pch.h"
#include "MiniMap_Panel.h"
#include "GameInstance.h"
#include "MiniMap_TrimBorder.h"
#include "NoMountIcon.h"

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

	Desc.fX = 170.f;
	Desc.fY = 920.f;
	Desc.fSizeX = 280.f;
	Desc.fSizeY = 280.f;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Element(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CMiniMap_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CMiniMap_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CMiniMap_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CMiniMap_Panel::Render()
{
	return S_OK;
}

_vector CMiniMap_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMiniMap_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CMiniMap_Panel::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CMiniMap_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMiniMap_TrimBorder>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CMiniMap_TrimBorder**>(&m_pMiniMap_TrimBorder))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("MiniMap_TrimBorder"), m_pMiniMap_TrimBorder);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CNoMountIcon>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CNoMountIcon**>(&m_pNoMountIcon))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("NoMountIcon"), m_pNoMountIcon);
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

	SAFE_RELEASE(m_pVIBufferCom);
}

void CMiniMap_Panel::Describe_Entity()
{
}
