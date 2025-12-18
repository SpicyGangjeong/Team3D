#include "pch.h"
#include "Quest_Status.h"
#include "GameInstance.h"

CQuest_Status::CQuest_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CQuest_Status::CQuest_Status(const CQuest_Status& rhs)
	:CElementObject(rhs)
{
}

HRESULT CQuest_Status::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_Status::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 180.f;
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

	m_fAlpha = 1.f;
	m_fTimeMult = 3.f;
	m_fAlphaTime = 1.f;
	Compute_UI(0);
	m_vImageposi = _float4{ 32.f, 32.f, 64.f, 64.f };
	m_fFontX = -780.f;
	m_fFontY = 610.f;
	Visible(true);
	return S_OK;
}

void CQuest_Status::Priority_Update(_float fTimeDelta)
{

	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CQuest_Status::Update(_float fTimeDelta)
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
	if (m_bHover == true)
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			m_bClick = true;
			static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Click"), &m_iIndex);
		}
	}

	if (m_bClick == true)
	{
		m_fSizeX = 148.f;
		m_fSizeY = 148.f;
		m_vImageposi = _float4{ 42.f, 42.f, 64.f, 64.f };
	}
	else
	{
		m_fSizeX = m_fOrigin_Size.x;
		m_fSizeY = m_fOrigin_Size.y;
		m_vImageposi = _float4{ 32.f, 32.f, 64.f, 64.f };
		m_bClick = false;
	}
	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}


void CQuest_Status::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CQuest_Status::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::QUEST_STATUS))))
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
	_float OffSet = (m_pGameInstance->FontSizeX(TEXT("Font_size20"), m_pName.c_str()) - 30) * 0.5f;
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_pName.c_str(), _float2((m_fFontX - m_fX) - OffSet, m_fFontY - m_fY));

	return S_OK;
}

_vector CQuest_Status::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_Status::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImageposi, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iHover", &m_bHover, sizeof(_bool))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iClick", &m_bClick, sizeof(_bool))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuest_Status::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Buttom_Slot"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Quest_Atlas"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_SpellMiniGame_ButtonRing"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuest_Status::Compute_UI(_int Index)
{
	_float2 fImage_Size = { 192.f, 64.f };

	_uint iCountX = 3;
	_uint iCountY = 1;

	_float iImageX = 64.f;
	_float iImageY = 64.f;

	_uint iframeX = Index % iCountX;
	_uint iframeY = Index / iCountX;

	_float2 UVStart;
	UVStart.x = iframeX * iImageX / fImage_Size.x;
	UVStart.y = iframeY * iImageY / fImage_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (iImageX / fImage_Size.x);
	UVEnd.y = UVStart.y + (iImageY / fImage_Size.y);

	_float4 UV = _float4{ UVStart.x, UVStart.y, UVEnd.x, UVEnd.y };
	m_vUV = UV;
}

void CQuest_Status::Set_Status(_int Index, _float PosX, _float PosY, _wstring Name)
{
	m_iIndex = Index;
	Compute_UI(Index);
	MoveX(PosX);
	MoveY(PosY);
	m_pName = Name;
}

void CQuest_Status::Set_Click(_bool bClick)
{
	m_bClick = bClick;
}

void CQuest_Status::Hover()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	// 2. UI 중심 기준으로 변환 (현재 구조 유지)
	_float2 fMouse;
	fMouse.x = ptMouse.x - (g_iWinSizeX * 0.5f);
	fMouse.y = -ptMouse.y + (g_iWinSizeY * 0.5f);

	// 3. 이미지 중심 좌표
	_float2 center;
	center.x = m_fX + m_pOwner->Get_WorldPostion().m128_f32[0];
	center.y = m_fY + m_pOwner->Get_WorldPostion().m128_f32[1];

	// 4. 반지름 계산 (원형이면 가로 기준)
	float radius = m_fSizeX * 0.4f;

	// 5. 거리 제곱 계산
	float dx = fMouse.x - center.x;
	float dy = fMouse.y - center.y;
	float distSq = dx * dx + dy * dy;

	// 6. 원 안 체크
	if (distSq <= radius * radius)
	{
		m_bHover = true;
	}
	else
	{
		m_bHover = false;
	}
}

CQuest_Status* CQuest_Status::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Status* pInstance = new CQuest_Status(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Status");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Status::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Status* pInstance = new CQuest_Status(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Status");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Status::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CQuest_Status::Describe_Entity()
{
}
#endif // _DEBUG
