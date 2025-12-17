#include "pch.h"
#include "MissionBanner_Border.h"
#include "GameInstance.h"
#include "Mission_Icon.h"


CMissionBanner_Border::CMissionBanner_Border(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CMissionBanner_Border::CMissionBanner_Border(const CMissionBanner_Border& rhs)
	:CElementObject(rhs)
{
}

HRESULT CMissionBanner_Border::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMissionBanner_Border::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -207.f;
	Desc.fY = -115.f;
	Desc.fSizeX = 256.f;
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

	m_fTimeMult = 15.f;
	m_fAlpha = 0.f;
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 100.f;
	m_fLerpX = m_fX;
	m_fLerpY = 100.f;
	m_fSortZ = 0.11f;
	m_vUVScale = _float2(1.f, 1.f);
	m_vNine_Slice = _float4(80, 200, 60, 90);
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Mission_On"), [this](void* p) {this->Set_FadeIn(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Mission_Off"), [this](void* p) {this->Set_FadeOut(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Mission_Off"), [this](void* p) {this->LerpOff(); });
	return S_OK;
}


void CMissionBanner_Border::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CMissionBanner_Border::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	m_fLerpTime = fTimeDelta * m_fTimeMult;

	if (m_bFadeIn == true)
	{

		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta * m_fAlphaTime;
		}

		if (m_fAlpha >= 0.5f)
		{
			LerpOn();
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
			m_fAlpha -= fTimeDelta * m_fAlphaTime;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		Lerp_PosY(100);
	}
	if (m_bLerpOn == true)
	{
		Size_LerpOn(m_fOrigin_Size.y + m_fLerpY, m_fLerpTime);
	}

	if (m_bLerpOff == true)
	{
		Size_LerpOff(m_fLerpTime);
	}

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CMissionBanner_Border::Late_Update(_float fTimeDelta)
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

HRESULT CMissionBanner_Border::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::NINECLICE)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}
	m_pMission_Icon->Render();
	return S_OK;
}

_vector CMissionBanner_Border::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CMissionBanner_Border::Size_LerpOn(_float LerpX, _float fTimeDelta)
{
	_float fDir = LerpX - m_fSizeY;
	_float move = m_fMoveSpeed * fTimeDelta;


	if (fDir <= move)
	{
		m_fSizeY = LerpX;
		m_bLerpOn = false;
	}

	else
	{
		m_fY -= move * 0.5f;
		m_fSizeY += move;
		static_cast<CUIObject*>(m_pMission_Icon)->MoveY(move * 0.5f);

	}
}
	
void CMissionBanner_Border::Size_LerpOff(_float fTimeDelta)
{
	_float fDir = m_fSizeY - m_fOrigin_Size.y;

	_float move = m_fMoveSpeed * fTimeDelta;

	if (fDir <= move)
	{
		m_fSizeY = m_fOrigin_Size.y;
		m_fLerpY;
		m_bLerpOn = false;
	}

	else
	{
		m_fSizeY -= move;
		m_fY += move * 0.5f;
		static_cast<CUIObject*>(m_pMission_Icon)->MoveY(-move * 0.5f);

	}
}

HRESULT CMissionBanner_Border::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaU", &m_vUVScale.x, sizeof(_float))))
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

HRESULT CMissionBanner_Border::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_MissionBanner_Border"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_ConjurationHUDPanelBack"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMission_Icon>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMission_Icon**>(&m_pMission_Icon))))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CMissionBanner_Border::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_fX += (fSizeX - m_vScale.x) * 0.5f;
	m_fLerpX += (fSizeX - m_vScale.x) * 0.5f;
	static_cast<CUIObject*>(m_pMission_Icon)->MoveX((fSizeX - m_vScale.x) * 0.5f);
}

void CMissionBanner_Border::Lerp_PosY(_float fSizeY)
{
	m_fLerpY = fSizeY;
}

CMissionBanner_Border* CMissionBanner_Border::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMissionBanner_Border* pInstance = new CMissionBanner_Border(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMissionBanner_Border");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMissionBanner_Border::Clone(void* pArg, CGameObject* pOwner)
{
	CMissionBanner_Border* pInstance = new CMissionBanner_Border(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMissionBanner_Border");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMissionBanner_Border::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMissionBanner_Border::Describe_Entity()
{
}
