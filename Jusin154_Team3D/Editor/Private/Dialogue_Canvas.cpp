#include "pch.h"
#include "Dialogue_Canvas.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Dialogue_Panel.h"

CDialogue_Canvas::CDialogue_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CDialogue_Canvas::CDialogue_Canvas(const CDialogue_Canvas& rhs)
	:CCanvasObject(rhs)
{
}

HRESULT CDialogue_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue_Canvas::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 1920.f;
	Desc.fSizeY = 1080.f;

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
	return S_OK;
}

void CDialogue_Canvas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CDialogue_Canvas::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CDialogue_Canvas::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CDialogue_Canvas::Render()
{
	return S_OK;
}

_vector CDialogue_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CDialogue_Canvas::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CDialogue_Canvas::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CDialogue_Canvas::Ready_Panel(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDialogue_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CDialogue_Panel**>(&m_pDialogue_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Dialogue_Panel"), m_pDialogue_Panel);

	return S_OK;
}

void CDialogue_Canvas::Clear_Penel()
{
}

CDialogue_Canvas* CDialogue_Canvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDialogue_Canvas* pInstance = new CDialogue_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialogue_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogue_Canvas::Clone(void* pArg, CGameObject* pOwner)
{
	CDialogue_Canvas* pInstance = new CDialogue_Canvas(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialogue_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDialogue_Canvas::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CDialogue_Canvas::Describe_Entity()
{
}
