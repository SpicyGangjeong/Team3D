#include "pch.h"
#include "Loding_Canvas.h"
#include "GameInstance.h"
#include "UI_Manager.h"


CLoding_Canvas::CLoding_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CLoding_Canvas::CLoding_Canvas(const CLoding_Canvas& rhs)
	:CCanvasObject(rhs)
{
}

HRESULT CLoding_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoding_Canvas::Initialize(void* pArg)
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
	Visible(true);
	return S_OK;
}

void CLoding_Canvas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CLoding_Canvas::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLoding_Canvas::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CLoding_Canvas::Render()
{
	return S_OK;
}

_vector CLoding_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLoding_Canvas::Bind_ShaderResources()
{

	return S_OK;
}

HRESULT CLoding_Canvas::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoding_Canvas::Ready_Panel(void* pArg)
{
	return S_OK;
}

void CLoding_Canvas::Clear_Penel()
{
}

CLoding_Canvas* CLoding_Canvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoding_Canvas* pInstance = new CLoding_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoding_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLoding_Canvas::Clone(void* pArg, CGameObject* pOwner)
{
	CLoding_Canvas* pInstance = new CLoding_Canvas(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoding_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLoding_Canvas::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CLoding_Canvas::Describe_Entity()
{
}
