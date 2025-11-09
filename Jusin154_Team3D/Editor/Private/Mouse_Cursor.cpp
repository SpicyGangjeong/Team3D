#include "pch.h"
#include "Mouse_Cursor.h"
#include "GameInstance.h"

CMouse_Cursor::CMouse_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject(pDevice, pContext)
{
}

CMouse_Cursor::CMouse_Cursor(const CMouse_Cursor& rhs)
	:CUIObject(rhs)
{
}

HRESULT CMouse_Cursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMouse_Cursor::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 74.f;
	Desc.fSizeY = 74.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	// 메뉴가 실행이 되면 마우스가 나타나고 다른 UI위에 올라가면 살짝 작아진다.
	return S_OK;
}

void CMouse_Cursor::Priority_Update(_float fTimeDelta)
{
}

void CMouse_Cursor::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	ShowCursor(false);
}

void CMouse_Cursor::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		POINT m_pPt{};
		GetCursorPos(&m_pPt);
		ScreenToClient(g_hWnd, &m_pPt);
		_vector MousePos = XMVectorSet(static_cast<_float>(m_pPt.x - m_fWinSizeX * 0.5), static_cast<_float>(-m_pPt.y + m_fWinSizeY * 0.5), 0.f, 1.f);
		m_pTransformCom->Set_State(STATE::POSITION, MousePos);

		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
}

HRESULT CMouse_Cursor::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::CURSOR)))) {
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

_vector CMouse_Cursor::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMouse_Cursor::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
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
	return S_OK;
}

HRESULT CMouse_Cursor::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Cursor"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMouse_Cursor* CMouse_Cursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMouse_Cursor* pInstance = new CMouse_Cursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMouse_Cursor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMouse_Cursor::Clone(void* pArg, CGameObject* pOwner)
{
	CMouse_Cursor* pInstance = new CMouse_Cursor(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMouse_Cursor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMouse_Cursor::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMouse_Cursor::Describe_Entity()
{
}
