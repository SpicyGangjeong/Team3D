#include "pch.h"
#include "Damage_Font.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CDamage_Font::CDamage_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CDamage_Font::CDamage_Font(const CDamage_Font& rhs)
	:CGameObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CDamage_Font::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDamage_Font::Initialize(void* pArg)
{
	m_pInfoInstance->Add_Event(TEXT("Monster_Hit"), [this](void* p) {this->Add_Damage(p); });
	m_vWhite_Color = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	return S_OK;
}

void CDamage_Font::Priority_Update(_float fTimeDelta)
{
}

void CDamage_Font::Update(_float fTimeDelta)
{
	if (!m_DamageInfos.empty())
	{
		for (auto iter = m_DamageInfos.begin(); iter != m_DamageInfos.end();)
		{
			if (iter->fTime <= 0.f)
				iter = m_DamageInfos.erase(iter);
			else
			{
				iter->fTime -= fTimeDelta;
				iter->fAlpha -= fTimeDelta;
				++iter;
			}
		}
	}
}

void CDamage_Font::Late_Update(_float fTimeDelta)
{
	if (!m_DamageInfos.empty())
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI_OVERLAY, this);
	}
}

HRESULT CDamage_Font::Render()
{
	if (!m_DamageInfos.empty())
	{

		for (auto iter = m_DamageInfos.begin(); iter != m_DamageInfos.end(); ++iter)
		{
			if (iter->fAlpha > 0.f)
			{
				m_pGameInstance->Perspective_Render_Text(
					m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
					m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
					TEXT("Font_size30"),
					iter->pDamage.c_str(),
					iter->vTarget_Pos,
					XMVectorSet(
						iter->vColor.m128_f32[0] * iter->fAlpha,
						iter->vColor.m128_f32[1] * iter->fAlpha,
						iter->vColor.m128_f32[2] * iter->fAlpha,
						iter->fAlpha)
				);
			}
		}
	}
	return S_OK;
}

_vector CDamage_Font::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CDamage_Font::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CDamage_Font::Ready_Components(void* pArg)
{
	return S_OK;
}

void CDamage_Font::Add_Damage(void* pArg)
{
	DAMAGE_INFO pInfo{};
	pInfo = *reinterpret_cast<DAMAGE_INFO*>(pArg);
	DAMAGE_FONT_INFO Desc{};

	Desc.vTarget_Pos = XMVectorAdd(pInfo.vTarget_Pos , XMVectorSet(0.f,1.2f,0.f,0.f));
	Desc.pDamage = to_wstring(static_cast<_int>(pInfo.fDamage));
	if (pInfo.eType == DAMAGE_TYPE::NOMAL)
	{
		Desc.vColor = m_vWhite_Color;
	}
	else
	{
		Desc.vColor = XMVectorSet((215.f / 255.f), (185.f / 255.f), (95.f / 255.f), 1.f);
	}

	m_DamageInfos.push_back(Desc);
}

CDamage_Font* CDamage_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDamage_Font* pInstance = new CDamage_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDamage_Font");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDamage_Font::Clone(void* pArg, CGameObject* pOwner)
{
	CDamage_Font* pInstance = new CDamage_Font(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDamage_Font");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDamage_Font::Free()
{
	__super::Free();

	m_DamageInfos.clear();
}

#ifdef _DEBUG
void CDamage_Font::Describe_Entity()
{
}
#endif // _DEBUG
