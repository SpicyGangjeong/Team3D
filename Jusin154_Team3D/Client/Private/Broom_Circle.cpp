#include "pch.h"
#include "Broom_Circle.h"
#include "GameInstance.h"

CBroom_Circle::CBroom_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CBroom_Circle::CBroom_Circle(const CBroom_Circle& rhs)
	:CElementObject(rhs)
{
}

HRESULT CBroom_Circle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Circle::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 120.f;
	Desc.fSizeX = 160.f;
	Desc.fSizeY = 160.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 150.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 1.f;
	m_iCount = 3;
	m_fFontX = 918.f;
	m_fFontY = 345.f;
	m_fFontAlpha = 1.f;
	m_fAngle = XMConvertToRadians(90.f);
	m_bTimer = false;
	return S_OK;
}

void CBroom_Circle::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CBroom_Circle::Update(_float fTimeDelta)
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

	//if (m_pGameInstance->Key_Down(DIK_K))
	//{
	//	Race_Start();
	//}

	if (m_iPerCount != m_iCount)
	{
		m_iPerCount = m_iCount;
		Start_Size_Lerp(fTimeDelta * m_fTimeMult);
	}


	if (m_fFontAlpha >= 0.f)
	{
		m_fFontAlpha -= fTimeDelta;
	}
	else
	{
		m_fFontAlpha = 0.f;
	}

	if (m_bHover == true)
	{
		if (m_fDissolve <= 1.f)
			m_fDissolve += fTimeDelta;
		else if (m_fDissolve >= 1.f)
		{
			m_bHover = false;
			Visible(false);
		}
	}

	__super::Update(fTimeDelta);
}

void CBroom_Circle::Late_Update(_float fTimeDelta)
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

HRESULT CBroom_Circle::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::BROOMCIRCLE)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	_wstring Count;
	_vector Color{};
	if (m_iCount > 0.f)
	{
		Count = to_wstring(m_iCount);
		Color = XMVectorSet(1.f * m_fFontAlpha, 1.f * m_fFontAlpha, 1.f * m_fFontAlpha, m_fFontAlpha);
	}

	else if (m_iCount == 0)
	{
		Count = TEXT("GO");
		Color = XMVectorSet(0.f, 1.f * m_fFontAlpha, 0.f, m_fFontAlpha);
	}

	_float OffSet = (m_pGameInstance->FontSizeX(TEXT("UI_size70"), Count.c_str()) - 61.f) * 0.5f;
	m_pGameInstance->Render_Text(TEXT("UI_size70"), Count.c_str(), _float2(m_fFontX + m_fX - OffSet, m_fFontY - m_fY), Color);

	return S_OK;
}

_vector CBroom_Circle::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CBroom_Circle::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fDissolve, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iHover", &m_bHover, sizeof(_int))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_fAngle, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBroom_Circle::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_AccioBallFrame_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_VFX_T_Repairo_Wisp_04_D"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CBroom_Circle::Start_Size_Lerp(_float fTimeDelta)
{
	m_fFontAlpha = 1.f;
	if (m_bCountDown == true)
	{
		m_bCountDown = false;
	}

	if (m_fSizeX >= 150.f && m_bSizeDown == false)
	{
		m_fSizeX -= fTimeDelta;
		m_fSizeY -= fTimeDelta;
	}

	if (m_fSizeX <= 150.f)
	{
		m_bSizeDown = true;
	}

	if (m_bSizeDown == true)
	{
		m_fSizeX += fTimeDelta;
		m_fSizeY += fTimeDelta;
	}

	if (m_fSizeX >= 180.f && m_bSizeUp == false)
	{
		m_bSizeDown = false;
		m_bSizeUp = true;
	}

	if (m_bSizeUp == true)
	{
		if (m_fSizeX >= 160.f)
		{
			m_fSizeX -= fTimeDelta;
			m_fSizeY -= fTimeDelta;
		}
		else if (m_fSizeX <= 160.f/* && m_bCountDown == false*/)
		{
			m_fSizeX = 160.f;
			m_fSizeY = 160.f;
			m_bSizeUp = false;
			/*m_fTime = 0.f;*/
			//m_bCountDown = true;
		}
	}

	if (m_iCount <= 0)
	{
		//m_bTimer = false;
		m_bHover = true;
		//static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("BroomEnd"), &m_bHover);
		//Visible(false);
	}

}

void CBroom_Circle::Race_Start(_int iCount)
{
	m_iCount = iCount;
	//m_bTimer = true;
	m_bCountDown = true;
	m_bHover = false;
	m_fTime = 0.f;
	m_fDissolve = 0.f;
	m_fFontAlpha = 1.f;
}

void CBroom_Circle::Set_Count(_int Count)
{
	m_iCount = Count;
}

CBroom_Circle* CBroom_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Circle* pInstance = new CBroom_Circle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_Circle");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_Circle::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_Circle* pInstance = new CBroom_Circle(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Circle");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_Circle::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CBroom_Circle::Describe_Entity()
{
}
#endif // _DEBUG
