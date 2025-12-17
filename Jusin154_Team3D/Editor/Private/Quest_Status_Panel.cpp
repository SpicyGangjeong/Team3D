#include "pch.h"
#include "Quest_Status_Panel.h"
#include "GameInstance.h"
#include "Quest_Status.h"

CQuest_Status_Panel::CQuest_Status_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CQuest_Status_Panel::CQuest_Status_Panel(const CQuest_Status_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CQuest_Status_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Status_Panel::Initialize(void* pArg)
{

	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -860.f;
	Desc.fY = 120.f;
	Desc.fSizeX = 175.f;
	Desc.fSizeY = 680.f;
	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

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

	m_fCanvasAlpha = 1.f;
	m_fSortZ = 0.04f;
	Visible(true);
	Set_Status();
	Add_Function(TEXT("Click"), [this](void* p) {this->Slot_Hover(*reinterpret_cast<_int*>(p)); });
	return S_OK;
}

void CQuest_Status_Panel::Set_Status()
{
	static_cast<CQuest_Status*>(m_pQuest_Status1)->Set_Status(0, 180.f, TEXT("전 체"));
	static_cast<CQuest_Status*>(m_pQuest_Status2)->Set_Status(1, 0.f, TEXT("진행중"));
	static_cast<CQuest_Status*>(m_pQuest_Status3)->Set_Status(2, -180.f, TEXT("완 료"));
	static_cast<CQuest_Status*>(m_pQuest_Status1)->Set_Click(true);
	m_bClick[0] = true;
	m_bClick[1] = false;
	m_bClick[2] = false;
}

void CQuest_Status_Panel::Slot_Hover(_int Index)
{
	for (_int i = 0; i < 3; ++i)
	{
		if (i == Index)
		{
			m_bClick[i] = true;
		}
		else
		{
			m_bClick[i] = false;
		}
	}
	static_cast<CQuest_Status*>(m_pQuest_Status1)->Set_Click(m_bClick[0]);
	static_cast<CQuest_Status*>(m_pQuest_Status2)->Set_Click(m_bClick[1]);
	static_cast<CQuest_Status*>(m_pQuest_Status3)->Set_Click(m_bClick[2]);
	static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("QuestPanelUpdate"), &Index);
}

void CQuest_Status_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CQuest_Status_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Update(fTimeDelta);
}

void CQuest_Status_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CQuest_Status_Panel::Render()
{
	return S_OK;
}

_vector CQuest_Status_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_Status_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CQuest_Status_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuest_Status_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Status>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Status**>(&m_pQuest_Status1))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Status1"), m_pQuest_Status1);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Status>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Status**>(&m_pQuest_Status2))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Status2"), m_pQuest_Status2);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Status>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Status**>(&m_pQuest_Status3))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Status3"), m_pQuest_Status3);

	return S_OK;
}

CQuest_Status_Panel* CQuest_Status_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Status_Panel* pInstance = new CQuest_Status_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Status_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Status_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Status_Panel* pInstance = new CQuest_Status_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Status_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Status_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CQuest_Status_Panel::Describe_Entity()
{
}
