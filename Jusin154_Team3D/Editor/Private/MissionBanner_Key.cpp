#include "pch.h"
#include "MissionBanner_Key.h"
#include "GameInstance.h"

CMissionBanner_Key::CMissionBanner_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CMissionBanner_Key::CMissionBanner_Key(const CMissionBanner_Key& rhs)
	:CElementObject(rhs)
{
}

HRESULT CMissionBanner_Key::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMissionBanner_Key::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -255.f;
	Desc.fY = 160.f;
	Desc.fSizeX = 160.f;
	Desc.fSizeY = 80.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 1.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 3.f;
	m_fPI = AI_MATH_TWO_PI_F;
	Compute_UV();
	Compute_Alphabat('V');
	m_eType = QUESTTYPE::MAIN;
	return S_OK;
}

void CMissionBanner_Key::QuestType(QUESTTYPE eType)
{
	m_eType = eType;
}

void CMissionBanner_Key::Compute_Alphabat(_tchar Alphabet)
{
	_uint Number = CMyTools::AlphabetToInt(Alphabet);

	_float2 fImage_Size = { 320.f, 384.f };

	_uint iXCount = 5;
	_uint iYCount = 6;

	_float frameWidth = 64.f;
	_float frameHeight = 64.f;

	_uint iframeX = Number % iXCount;
	_uint iframeY = Number / iXCount;

	_float2 UVStart;
	UVStart.x = iframeX * frameWidth / fImage_Size.x;
	UVStart.y = iframeY * frameHeight / fImage_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / fImage_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / fImage_Size.y);

	m_vUV = _float4(UVStart.x, UVStart.y, UVEnd.x, UVEnd.y);
}

void CMissionBanner_Key::Compute_UV()
{
	m_vKeyHold = _float4(-9.f, -9.f, 94.f, 94.f);
	m_vKey = _float4(18.f, 18.5f, 40.f, 40.f);
	m_vActive_Icon = _float4(80.f, 10.f, 36.f, 36.f);
}

void CMissionBanner_Key::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CMissionBanner_Key::Update(_float fTimeDelta)
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

	if (m_bKeyHold == false)
	{
		if (m_pGameInstance->Key_Pressing(DIK_V))
		{
			m_fTime += fTimeDelta * (1.f / m_fTimeMult);
		}
		else
		{
			m_fTime = 0.f;
			m_bKeyHold = false;
		}
	}

	if (m_pGameInstance->Key_Up(DIK_V))
	{
		m_bKeyHold = false;
	}

	if (m_fTime >= 1.f)
	{
		m_fTime = 0.f;
		m_bisHoldOn = !m_bisHoldOn;
		m_bKeyHold = true;
	}

	if (m_bisHoldOn)
	{
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Mission_On"));
	}
	else
	{
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Mission_Off"));
	}
	__super::Update(fTimeDelta);
}

void CMissionBanner_Key::Late_Update(_float fTimeDelta)
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

HRESULT CMissionBanner_Key::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::HOLD_ROTATION)))) {
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

_vector CMissionBanner_Key::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CMissionBanner_Key::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom3->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPI", &m_fPI, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vKeyHold, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos2", &m_vKey, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos3", &m_vActive_Icon, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iQuestType", &m_eType, sizeof(_uint))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMissionBanner_Key::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_MissionBanner_Key"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Finishi_Rect"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Atlas_Keyboard"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("ActiveMission_Icon"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CMissionBanner_Key* CMissionBanner_Key::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMissionBanner_Key* pInstance = new CMissionBanner_Key(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMissionBanner_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMissionBanner_Key::Clone(void* pArg, CGameObject* pOwner)
{
	CMissionBanner_Key* pInstance = new CMissionBanner_Key(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMissionBanner_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMissionBanner_Key::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMissionBanner_Key::Describe_Entity()
{
}
