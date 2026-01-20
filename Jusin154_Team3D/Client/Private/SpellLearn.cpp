#include "pch.h"
#include "SpellLearn.h"
#include "GameInstance.h"
#include "SpellLearn_MovePointer.h"
#include "SpellLearn_ChaserPointer.h"
#include "InfoInstance.h"

CSpellLearn::CSpellLearn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpellLearn::CSpellLearn(const CSpellLearn& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSpellLearn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 600.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 550.f;
	Desc.fSizeY = 550.f;
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
	m_fAlpha = 0.f;
	m_fAlphaTime = 5.f;
	m_iTrailIndex = 0;
	m_fSortZ = 0.02f;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("LearnClear"), [this](void* p) {this->Clear(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("OverrlayClear"), [this](void* p) {this->Set_FadeOut(); });
	Change_Image(0);
	Visible(false);
	return S_OK;
}

void CSpellLearn::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpellLearn::Update(_float fTimeDelta)
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

	if (m_bReset == true)
	{
		m_Trail.clear();
		m_ChaseTrail.clear();
		m_bReset = false;
	}

	if (m_bHover == false)
	{
		fill(m_Trail.begin(), m_Trail.end(), _float2(-9999.f, -9999.f));
		fill(m_ChaseTrail.begin(), m_ChaseTrail.end(), _float2(-9999.f, -9999.f));
		m_bReset = true;
	}

	if (m_pPointer != nullptr)
	{
		if (m_pPointer->Get_MoveStart() == true)
		{
			m_bHover = true;
			m_vPointerPosition = m_pPointer->Get_Position();
			m_vPointerScale = m_pPointer->Get_Current_Size();

			m_vChasePosition = m_pChasePointer->Get_Position();
			m_vChaseScale = m_pChasePointer->Get_Current_Size();

			m_fTime += fTimeDelta * m_fTimeMult;
			if (m_fTime >= 0.3f)
			{
				m_fTime = 0.f;
				m_Trail.push_back(m_vPointerPosition);
				m_iTrailIndex++;
				m_ChaseTrail.push_back(m_vChasePosition);
				m_iChaseiTrailIndex++;
			}
		}
	}


	__super::Update(fTimeDelta);
}

void CSpellLearn::Late_Update(_float fTimeDelta)
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

HRESULT CSpellLearn::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::SPELLLEARNCOLOR)))) {
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

_vector CSpellLearn::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpellLearn::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fItemImageSizes1", &m_vPointerScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPosition", &m_fCurrent_Position, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Trail", m_Trail.data(), sizeof(_float2) * _uint(m_Trail.size()))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Count", &m_iTrailIndex, sizeof(_int))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ChaseTrail", m_ChaseTrail.data(), sizeof(_float2) * _uint(m_Trail.size()))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ChaseCount", &m_iChaseiTrailIndex, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpellLearn::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Levioso"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_Goldleaf_Large"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpellLearn::Change_Image(_int SpellID)
{
	m_bHover = false;
	m_ChaseTrail.push_back(_float2(-9999.f, -9999.f));
	m_Trail.push_back(_float2(-9999.f, -9999.f));
	m_iTrailIndex = 0;
	m_iChaseiTrailIndex = 0;
	_wstring pName = TEXT("Prototype_Texture_");
	_wstring pImageName = pName + m_pInfoInstance->Get_SpellLearn(SpellID).pImageName;
	m_MoveLine = m_pInfoInstance->Get_SpellLearn(SpellID).Lines;
	if (m_pDiffuse_TextureCom)
	{
		Remove_Component<CTexture>();
		SAFE_RELEASE(m_pDiffuse_TextureCom);
	}
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pImageName, reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom))))
		return E_FAIL;

	Set_FadeIn();
	return S_OK;
}

void CSpellLearn::Set_Pointer(CSpellLearn_MovePointer* Pointer, CSpellLearn_ChaserPointer* Chase)
{
	m_pPointer = Pointer;
	m_pChasePointer = Chase;
}

void CSpellLearn::Clear()
{
	fill(m_ChaseTrail.begin(), m_ChaseTrail.end(), _float2(-9999.f, -9999.f));
}

CSpellLearn* CSpellLearn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn* pInstance = new CSpellLearn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn* pInstance = new CSpellLearn(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpellLearn::Describe_Entity()
{
}
#endif // _DEBUG
