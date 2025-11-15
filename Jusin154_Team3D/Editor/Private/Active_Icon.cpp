#include "pch.h"
#include "Active_Icon.h"
#include "GameInstance.h"

CActive_Icon::CActive_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CActive_Icon::CActive_Icon(const CActive_Icon& rhs)
	:CElementObject(rhs)
{
}

HRESULT CActive_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActive_Icon::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -230.f;
	Desc.fY = 160.f;
	Desc.fSizeX = 35.f;
	Desc.fSizeY = 35.f;

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
	m_fAlphaTime = 3.f;
	m_eType = QUESTYPE::MAIN;
	return S_OK;
}

void CActive_Icon::QuestType(QUESTYPE eType)
{
	m_eType = eType;
}

void CActive_Icon::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CActive_Icon::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	m_fTime += fTimeDelta * m_fTimeMult;

	m_fOwnerAlpha = static_cast<CUIObject*>(m_pOwner)->Get_Alpha();
	m_fCanvasAlpha = static_cast<CUIObject*>(m_pOwner)->Get_OwnerAlpha();
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

	if (m_bLerpOn == true)
	{
		Start_Lerp(fTimeDelta);
	}

	if (m_bLerpOff == true)
	{
		Reset_Pos(fTimeDelta);
	}

	__super::Update(fTimeDelta);
}

void CActive_Icon::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		}
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CActive_Icon::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::QUESTYPE)))) {
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

_vector CActive_Icon::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

HRESULT CActive_Icon::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iQuestType", &m_eType, sizeof(_uint))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CActive_Icon::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("ActiveMission_Icon"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CActive_Icon* CActive_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActive_Icon* pInstance = new CActive_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CActive_Icon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CActive_Icon::Clone(void* pArg, CGameObject* pOwner)
{
	CActive_Icon* pInstance = new CActive_Icon(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CActive_Icon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CActive_Icon::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CActive_Icon::Describe_Entity()
{
}
