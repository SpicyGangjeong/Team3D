#include "pch.h"
#include "Quest_Info.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CQuest_Info::CQuest_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CQuest_Info::CQuest_Info(const CQuest_Info& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CQuest_Info::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Info::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 325.f;
	Desc.fY = 250.f;
	Desc.fSizeX = 128.f;
	Desc.fSizeY = 128.f;

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
	m_vNine_Slice = _float4(50.f, 75.f, 30.f, 96.f);
	m_fPreviewOffSet = 0.f;
	m_fOriginPerviewSize = 280.f;
	SizeUpX(960);
	SizeUpY(m_fOriginPerviewSize);
	m_fSortZ = 0.02f;
	m_fFontX = 530.f;
	m_fFontY = 500.f;
	m_iPerQuestIndex = -1;
	Visible(false);
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("QuestListHover"), [this](void* p) {this->Set_Hover(p); });
	return S_OK;
}

void CQuest_Info::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CQuest_Info::Update(_float fTimeDelta)
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
			m_bHover = true;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
		{
			m_fAlpha -= fTimeDelta * m_fAlphaTime;
			m_bHover = false;
		}

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}

	if (m_iCurrentIndex != m_iQuest_Index)
	{
		Y(m_fOriginPerviewSize + (m_pInfoInstance->Get_Quest(m_iCurrentQuest, m_iQuest_Index).ObjectiveInfo.size() * 50.f));
		m_iCurrentIndex = m_iQuest_Index;
	}

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CQuest_Info::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CQuest_Info::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::QUEST_INFO)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	if (m_iQuest_Index != -1)
	{
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_pInfoInstance->Get_Quest(m_iCurrentQuest, m_iQuest_Index).pQuestInfo.c_str(), _float2(m_fFontX + m_fX, m_fFontY - m_fY), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));

		for (_int i = 0; i < m_pInfoInstance->Get_Quest(m_iCurrentQuest, m_iQuest_Index).ObjectiveInfo.size(); ++i)
		{
			m_pGameInstance->Render_Text(TEXT("Font_size20"), m_pInfoInstance->Get_Quest(m_iCurrentQuest, m_iQuest_Index).ObjectiveInfo[i].pQuestInfo.c_str(), _float2(m_fFontX + m_fX, m_fFontY - m_fY + 100 + (50.f * i)), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));
		}
	}

	return S_OK;
}

_vector CQuest_Info::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_Info::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
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

HRESULT CQuest_Info::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_SpellWidgetPanelBack"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_TutorialMediaBorder"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuest_Info::Set_Hover(void* pArg)
{
	CURRENTQUESTSECETINFO* Desc = static_cast<CURRENTQUESTSECETINFO*>(pArg);

	m_iCurrentQuest = Desc->iQuestCategory;
	m_iQuest_Index = Desc->iQuestIndex;
	if (m_iQuest_Index == -1)
	{
		Visible(false);
		return;
	}
	Visible(true);
}

void CQuest_Info::Y(_float fSizeY)
{
	m_fSizeY = fSizeY;

	_float Delta = fSizeY - m_fOriginPerviewSize;

	m_fY = m_fOrigin_Position.y - (Delta * 0.5f);
}

CQuest_Info* CQuest_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Info* pInstance = new CQuest_Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Info");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Info::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Info* pInstance = new CQuest_Info(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Info");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Info::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CQuest_Info::Describe_Entity()
{
}
#endif // _DEBUG
