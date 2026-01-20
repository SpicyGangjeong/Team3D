#include "pch.h"
#include "Broom_Record.h"
#include "GameInstance.h"

CBroom_Record::CBroom_Record(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CBroom_Record::CBroom_Record(const CBroom_Record& rhs)
	:CElementObject(rhs)
{
}

HRESULT CBroom_Record::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Record::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 120.f;
	Desc.fSizeX = 500.f;
	Desc.fSizeY = 150.f;

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
	m_fAlphaTime = 10.f;
	m_fBaseTime = 5.f;
	Set_Font();
	Set_Best_Record(m_fBaseTime);
	return S_OK;
}

void CBroom_Record::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CBroom_Record::Update(_float fTimeDelta)
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

	m_fTime += fTimeDelta * m_fTimeMult;

	__super::Update(fTimeDelta);
}

void CBroom_Record::Late_Update(_float fTimeDelta)
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

HRESULT CBroom_Record::Render()
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

	if (m_bNewScore == false)
	{
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[0].c_str(), m_fFontPos[0]);
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[1].c_str(), m_fFontPos[1], XMLoadFloat4(&m_fFontColor[0]));
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[2].c_str(), m_fFontPos[2]);
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[3].c_str(), m_fFontPos[3]);
	}
	else
	{
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[4].c_str(), m_fFontPos[4], XMLoadFloat4(&m_fFontColor[1]));
		m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[1].c_str(), m_fFontPos[5]);
	}


	return S_OK;
}

_vector CBroom_Record::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CBroom_Record::Bind_ShaderResources()
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

HRESULT CBroom_Record::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_Broom_Record"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CBroom_Record::Set_Font()
{
	m_fFont[0] = TEXT("내 기록");
	m_fFont[1] = TEXT("0:00:00");
	m_fFont[2] = TEXT("최고 기록");
	m_fFont[3] = TEXT("0:00:00");
	m_fFont[4] = TEXT("새 최고 기록");

	m_fFontPos[0] = _float2(m_fX + 820.f, m_fY + 140.f);
	m_fFontPos[1] = _float2(m_fX + 810.f, m_fY + 180.f);
	m_fFontPos[2] = _float2(m_fX + 1005.f, m_fY + 140.f);
	m_fFontPos[3] = _float2(m_fX + 1010.f, m_fY + 180.f);
	m_fFontPos[4] = _float2(m_fX + 880, m_fY + 140.f);
	m_fFontPos[5] = _float2(m_fX + 900, m_fY + 180.f);

	m_fFontColor[0] = _float4(0.f, 1.f, 0.f, 1.f);
	m_fFontColor[1] = _float4((208.f / 255.f) * m_fAlpha, (177.f / 255.f) * m_fAlpha, (52.f / 255.f) * m_fAlpha, m_fAlpha);

}

void CBroom_Record::Set_Best_Record(_float fTime)
{

	wchar_t szBuf[32] = {};

	_int min = static_cast<_int>(fTime) / 60;
	_int sec = static_cast<_int>(fTime) % 60;
	_int ms = static_cast<_int>((fTime - static_cast<_int>(fTime)) * 100);

	swprintf_s(szBuf, L"%02d:%02d:%02d", min, sec, ms);

	m_fFont[3] = szBuf;
}

void CBroom_Record::Rece_Results()
{
	Visible(true);
}

void CBroom_Record::Finish(_float fTime)
{
	if (m_fBaseTime > fTime)
		m_bNewScore = true;
	else
		m_bNewScore = false;

	static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Score"), &m_bNewScore);

	wchar_t szBuf[32] = {};

	_int min = static_cast<_int>(fTime) / 60;
	_int sec = static_cast<_int>(fTime) % 60;
	_int ms = static_cast<_int>((fTime - static_cast<_int>(fTime)) * 100);

	swprintf_s(szBuf, L"%02d:%02d:%02d", min, sec, ms);
	m_fFont[1] = szBuf;
}

_bool CBroom_Record::NewScore()
{
	return m_bNewScore;
}

CBroom_Record* CBroom_Record::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Record* pInstance = new CBroom_Record(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_Record");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_Record::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_Record* pInstance = new CBroom_Record(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Record");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_Record::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CBroom_Record::Describe_Entity()
{
}
#endif // _DEBUG
