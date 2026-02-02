#include "pch.h"
#include "Quest_Slot.h"
#include "GameInstance.h"
#include "Quest_Panel.h"
#include "InfoInstance.h"

CQuest_Slot::CQuest_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CQuest_Slot::CQuest_Slot(const CQuest_Slot& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CQuest_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Slot::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 64.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fAlpha = 1.f;
	m_fTimeMult = 3.f;
	m_vNine_Slice = _float4(70.f, 186.f, 0.f, 64.f);
	m_fAlphaTime = 1.f;
	m_fFontX = 300.f;
	m_fFontY = 330.f;
	m_fOffSetX = 0.f;
	m_fOffSetY = 85;
	m_iCols = 1;
	m_fPositionX = -475.f;
	m_fPositionY = 330.f;
	SizeUpX(495.f);
	SizeUpY(80.f);
	m_pVIBufferCom->Set_Cloned(true);
	m_pVIBufferCom->Set_Pos(m_fPositionX, m_fPositionY, m_fOffSetX, m_fOffSetY, m_iCols);
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
	m_iQuestCount = m_pInfoInstance->Get_Quest_Count(ENUM_CLASS(QUESTSTATE::NONE));
	Set_QuestType(ENUM_CLASS(QUESTSTATE::NONE));
	Visible(true);
	return S_OK;
}

void CQuest_Slot::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CQuest_Slot::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
			m_fAlpha += fTimeDelta * m_fAlphaTime;

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}
	Hover();

	static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("QuestListHover"), &m_Info);

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CQuest_Slot::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible)
	{
		m_pVIBufferCom->Set_Hover(m_iIndex);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CQuest_Slot::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIINTANCE::QUEST_SLOT))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	for (_int i = 0; i < m_iQuestCount; ++i)
	{
		m_pGameInstance->Render_Text(m_Fonts[i].pFontSizeName.c_str(), m_Fonts[i].pQuestName.c_str(), m_Fonts[i].m_fFontSize);
	}
	return S_OK;
}

void CQuest_Slot::Font_Size()
{
	for (_int i = 0; i < m_iQuestCount; ++i)
	{
		m_fFontY = Get_SlotY(i);
		if (m_iIndex == -1)
		{
			m_Fonts[i].pFontSizeName = TEXT("Font_size20");
			m_Fonts[i].m_fFontSize = _float2(m_fFontX + m_fX, m_fFontY - m_fY);
		}
		else
		{
			if (i == m_iIndex)
			{
				m_Fonts[i].pFontSizeName = TEXT("Font_size30");
				m_Fonts[i].m_fFontSize = _float2(m_fFontX - 10.f + m_fX, m_fFontY - 10.f - m_fY);
			}
			else
			{
				m_Fonts[i].pFontSizeName = TEXT("Font_size20");
				m_Fonts[i].m_fFontSize = _float2(m_fFontX + m_fX, m_fFontY - m_fY);
			}
		}
	}
}

void CQuest_Slot::Font_Setting(_int Index)
{
	m_Fonts.clear();
	FontInfo Fonts;

	for (_int i = 0; i < Index; ++i)
	{
		m_fFontY = Get_SlotY(i);
		Fonts.pFontSizeName = TEXT("Font_size20");
		Fonts.pQuestName = m_pInfoInstance->Get_Quest(m_Info.iQuestCategory, i).pQuestName.c_str();
		Fonts.m_fFontSize = _float2(m_fFontX + m_fX, m_fFontY - m_fY);

		m_Fonts.push_back(Fonts);
	}
}

_int CQuest_Slot::Mousechack(POINT Mouse)
{
	for (_int i = 0; i < m_iQuestCount; ++i)
	{
		_float SlotY = m_fPositionY - (i * m_fOffSetY);

		RECT Box = {
			long(m_fPositionX - m_fSizeX * 0.5f),
			long(SlotY - m_fSizeY * 0.5f),
			long(m_fPositionX + m_fSizeX * 0.5f),
			long(SlotY + m_fSizeY * 0.5f)
		};

		if (PtInRect(&Box, Mouse))
		{
			return i;
		}
	}

	return -1;
}

_float CQuest_Slot::Get_SlotY(_int iIndex)
{
	return 190 + (iIndex * m_fOffSetY);
}

_vector CQuest_Slot::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CQuest_Slot::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_pVIBufferCom->Set_SizeX(m_fSizeX);
}

void CQuest_Slot::SizeUpY(_float fSizeY)
{
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_SizeY(m_fSizeY);
}

void CQuest_Slot::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
}

HRESULT CQuest_Slot::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNine_Slice", &m_vNine_Slice, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOrigin_Size", &m_fOrigin_Size, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CQuest_Slot::Ready_Components(void* pArg)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Quest_Slot"), (CComponent**)&m_pVIBufferCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_MenuTextButtonBorder"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIINSTANCE, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuest_Slot::Hover()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fMouse;
	fMouse.x = ptMouse.x - (g_iWinSizeX * 0.5f);
	fMouse.y = -(ptMouse.y - (g_iWinSizeY * 0.5f));

	POINT Mouse{};
	Mouse.x = _long(fMouse.x);
	Mouse.y = _long(fMouse.y);

	m_Info.iQuestIndex = m_iIndex = Mousechack(Mouse);
	
	if (m_iIndex != m_iPrevIndex)
	{
		//m_pVIBufferCom->Set_Hover(m_iIndex); 
		Font_Size();
		m_iPrevIndex = m_iIndex;
		m_pGameInstance->Sound_Play(SOUND::SD_KIND::SLOT_HOVER, SD_CHANNEL_GROUP::EFFECT, false, 1.f);
	}
}


void CQuest_Slot::Set_QuestType(_int Index)
{
	switch (Index)
	{
	case 0:
		m_Info.iQuestCategory = ENUM_CLASS(QUESTSTATE::NONE);
		m_iQuestCount = m_pInfoInstance->Get_Quest_Count(ENUM_CLASS(QUESTSTATE::NONE));
		m_pVIBufferCom->Set_Draw(-1);
		break;

	case 1:
		m_Info.iQuestCategory = ENUM_CLASS(QUESTSTATE::ACCEPTED);
		m_iQuestCount = m_pInfoInstance->Get_Quest_Count(ENUM_CLASS(QUESTSTATE::ACCEPTED));
		m_pVIBufferCom->Set_Draw(m_iQuestCount);
		break;

	case 2:
		m_Info.iQuestCategory = ENUM_CLASS(QUESTSTATE::CLEARED);
		m_iQuestCount = m_pInfoInstance->Get_Quest_Count(ENUM_CLASS(QUESTSTATE::CLEARED));
		m_pVIBufferCom->Set_Draw(m_iQuestCount);
		break;

	default:
		break;
	}
	Font_Setting(m_iQuestCount);
}

CQuest_Slot* CQuest_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Slot* pInstance = new CQuest_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Slot::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Slot* pInstance = new CQuest_Slot(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Slot::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}


#ifdef _DEBUG
void CQuest_Slot::Describe_Entity()
{
}
#endif // _DEBUG
