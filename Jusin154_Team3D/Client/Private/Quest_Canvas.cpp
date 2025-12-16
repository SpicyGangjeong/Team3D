#include "pch.h"
#include "UI_Manager.h"
#include "Quest_Canvas.h"
#include "GameInstance.h"
#include "Quest_Panel.h"
#include "Quest_Status_Panel.h"
#include "Completed_Panel.h"
#include "InProgress_Panel.h"

CQuest_Canvas::CQuest_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CQuest_Canvas::CQuest_Canvas(const CQuest_Canvas& rhs)
	:CCanvasObject(rhs)
{
}

HRESULT CQuest_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Canvas::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = 540.f;
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
	m_fAlpha = 1.f;
	m_fOwnerAlpha = 1.f;
	m_fCanvasAlpha = 1.f;
	m_fSortZ = 0.05f;
	Visible(false);
	m_iIndex = -1;
	Add_Function(TEXT("QuestPanelUpdate"), [this](void* p) {this->Update_Panel(*reinterpret_cast<_int*>(p)); });
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
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CQuest_Canvas::Render()
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

_vector CQuest_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_Canvas::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOwnerAlpha", &m_fOwnerAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCanvasAlpha", &m_fCanvasAlpha, sizeof(_float))))
	{
		return E_FAIL;

	}
	return S_OK;
}

HRESULT CQuest_Canvas::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Quest_Widget"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuest_Canvas::Ready_Panel(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CQuest_Panel**>(&m_pQuest_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Quest_Panel"), m_pQuest_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCompleted_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CCompleted_Panel**>(&m_pCompleted_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Completed_Panel"), m_pCompleted_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInProgress_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CInProgress_Panel**>(&m_pInProgress_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("InProgress_Panel"), m_pInProgress_Panel);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Status_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CQuest_Status_Panel**>(&m_pQuest_Status_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Quest_Status_Panel"), m_pQuest_Status_Panel);

	return S_OK;
}

void CQuest_Canvas::Clear_Penel()
{
}

void CQuest_Canvas::Update_Panel(_int Index)
{
	if (m_iIndex == Index)
		return;

	m_iIndex = Index;
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
	static_cast<CPanelObject*>(m_pQuest_Panel)->Visible(m_bClick[0]);
	static_cast<CPanelObject*>(m_pCompleted_Panel)->Visible(m_bClick[1]);
	static_cast<CPanelObject*>(m_pInProgress_Panel)->Visible(m_bClick[2]);
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

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CQuest_Canvas::Describe_Entity()
{
}
#endif // _DEBUG
