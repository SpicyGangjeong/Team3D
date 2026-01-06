#include "pch.h"
#include "UI_Manager.h"
#include "Quest_Canvas.h"
#include "GameInstance.h"
#include "Quest_Panel.h"
#include "InfoInstance.h"

CQuest_Canvas::CQuest_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CQuest_Canvas::CQuest_Canvas(const CQuest_Canvas& rhs)
	:CCanvasObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CQuest_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Canvas::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Panel(pArg)))
	{
		return E_FAIL;
	}
	Visible(false);
	m_pInfoInstance->Set_AcceptQuest(0);
	m_pInfoInstance->Set_AcceptQuest(1);
	m_pInfoInstance->Set_AcceptQuest(2);
	return S_OK;
}

void CQuest_Canvas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CQuest_Canvas::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CQuest_Canvas::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CQuest_Canvas::Render()
{
	return S_OK;
}

_vector CQuest_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_Canvas::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CQuest_Canvas::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CQuest_Canvas::Ready_Panel(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Panel>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CQuest_Panel**>(&m_pQuest_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Quest_Panel"), m_pQuest_Panel);

	return S_OK;
}

void CQuest_Canvas::Clear_Penel()
{
}

void CQuest_Canvas::Visible(_bool bVisible)
{
	_int Index = 0;
	m_bVisible = bVisible;
	if (m_bVisible == false)
		Function_Callback(TEXT("QuestPanelClose"), &Index);
}

CQuest_Canvas* CQuest_Canvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Canvas* pInstance = new CQuest_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Canvas::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Canvas* pInstance = new CQuest_Canvas(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Canvas::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CQuest_Canvas::Describe_Entity()
{
}
#endif // _DEBUG
