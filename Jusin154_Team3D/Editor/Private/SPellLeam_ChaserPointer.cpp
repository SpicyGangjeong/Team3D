#include "pch.h"
#include "SPellLeam_ChaserPointer.h"
#include "GameInstance.h"
#include "SpellLearn_MovePointer.h"

CSPellLeam_ChaserPointer::CSPellLeam_ChaserPointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSPellLeam_ChaserPointer::CSPellLeam_ChaserPointer(const CSPellLeam_ChaserPointer& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSPellLeam_ChaserPointer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSPellLeam_ChaserPointer::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 476.f;
	Desc.fY = -253.f;
	Desc.fSizeX = 32.f;
	Desc.fSizeY = 32.f;
	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 3.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 140.f;
	Set_SpellLearn(0);
	Visible(false);
	return S_OK;
}

void CSPellLeam_ChaserPointer::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSPellLeam_ChaserPointer::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta * m_fAlphaTime;
		}

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta * m_fAlphaTime;;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}

	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fMouse{};
	fMouse.x = ptMouse.x - (g_iWinSizeX * 0.5f);
	fMouse.y = -ptMouse.y + (g_iWinSizeY * 0.5f);

	_float2 fFinalMouse{};
	fFinalMouse.x = fMouse.x - m_fX;
	fFinalMouse.y = fMouse.y - m_fY;

	if (m_pGameInstance->Key_Down(DIK_L))
	{
		m_bMoveStart = true;
	}

	if (m_bMoveStart == true)
	{
		m_fTime += fTimeDelta * m_fTimeMult;
		if (m_fTime >= 1.f)
		{
			m_bHover = true;
			Line(fTimeDelta);
		}
	}

	if (m_bHover == true)
	{
		_float Dir = XMVectorGetX(
				  XMVector3Length(
				 XMVectorSubtract(m_pPointer->Get_Current_Position(), m_fCurrent_Position)));
		
		if (Dir <= 16.f)
		{
			_int a = 10;
		}
	}

	m_fAngle = atan2(-fFinalMouse.x, fFinalMouse.y);
	__super::Update(fTimeDelta);
}

void CSPellLeam_ChaserPointer::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		if (m_bHover == true)
		{
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		}
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CSPellLeam_ChaserPointer::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
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

_vector CSPellLeam_ChaserPointer::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CSPellLeam_ChaserPointer::Set_SpellLearn(_int Index)
{
	m_bHover = false;
	m_bMoveStart = false;
	m_fTime = 0.f;
	m_iLineIndex = _int(static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).Lines.size());
	m_MoveLine = static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).Lines;
	m_iCurrentLine = 0;
	m_fX = static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).fStartPosition.x;
	m_fY = static_cast<CUIObject*>(m_pOwner)->Get_Learninfo(Index).fStartPosition.y;
}

void CSPellLeam_ChaserPointer::Set_Pointer(CSpellLearn_MovePointer* Pointer)
{
	m_pPointer = Pointer;
}

void CSPellLeam_ChaserPointer::Line(_float fTime)
{
	m_vLerp_Position = m_MoveLine[m_iCurrentLine];

	if (Start_Lerp(fTime) == true)
	{
		if (m_iCurrentLine < m_iLineIndex - 1)
		{
			++m_iCurrentLine;
		}
		else
		{
			m_bMoveStart = false;
		}
	}

}

HRESULT CSPellLeam_ChaserPointer::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
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

HRESULT CSPellLeam_ChaserPointer::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_SpellMinigame_SpeedBurstDropBack"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSPellLeam_ChaserPointer* CSPellLeam_ChaserPointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSPellLeam_ChaserPointer* pInstance = new CSPellLeam_ChaserPointer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSPellLeam_ChaserPointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSPellLeam_ChaserPointer::Clone(void* pArg, CGameObject* pOwner)
{
	CSPellLeam_ChaserPointer* pInstance = new CSPellLeam_ChaserPointer(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSPellLeam_ChaserPointer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSPellLeam_ChaserPointer::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSPellLeam_ChaserPointer::Describe_Entity()
{
}
