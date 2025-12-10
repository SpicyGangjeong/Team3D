#include "pch.h"
#include "CDamageFont.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CDamageFont::CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CDamageFont::CDamageFont(const CDamageFont& rhs)
	:CGameObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CDamageFont::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDamageFont::Initialize(void* pArg)
{
	return S_OK;
}

void CDamageFont::Priority_Update(_float fTimeDelta)
{
}

void CDamageFont::Update(_float fTimeDelta)
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

				iter->vColor = XMVectorSet(
					iter->vColor.m128_f32[0] * iter->fAlpha,
					iter->vColor.m128_f32[1] * iter->fAlpha,
					iter->vColor.m128_f32[2] * iter->fAlpha,
					iter->fAlpha);

				++iter;
			}
		}
	}
}

void CDamageFont::Late_Update(_float fTimeDelta)
{
}

HRESULT CDamageFont::Render()
{
	if (!m_DamageInfos.empty())
	{
		for (auto iter = m_DamageInfos.begin(); iter != m_DamageInfos.end();)
		{
			m_pGameInstance->Perspective_Render_Text(
				m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW),
				m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ),
				TEXT("UI_size13"),
				iter->pDamage.c_str(),
				iter->vTarget_Pos,
				iter->vColor
			);
		}
	}
	return S_OK;
}

_vector CDamageFont::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CDamageFont::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CDamageFont::Ready_Components(void* pArg)
{
	return S_OK;
}

void CDamageFont::Add_Damage(DAMAGE_INFO pInfo)
{
	DAMAGE_FONT_INFO Desc;

	Desc.vTarget_Pos = pInfo.vTarget_Pos;
	Desc.pDamage = to_wstring(pInfo.fDamage);
	if (pInfo.eType == DAMAGE_TYPE::NOMAL)
	{
		Desc.vColor = XMVectorSet(1.f ,1.f,1.f,1.f);
	}
	else
	{
		Desc.vColor = XMVectorSet((215.f / 255.f), (185.f / 255.f), (95.f / 255.f), 1.f);
	}

	m_DamageInfos.push_back(Desc);
}

CDamageFont* CDamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDamageFont* pInstance = new CDamageFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDamageFont");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDamageFont::Clone(void* pArg, CGameObject* pOwner)
{
	CDamageFont* pInstance = new CDamageFont(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDamageFont");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDamageFont::Free()
{
	__super::Free();
}

#ifdef _DEBUG
void CDamageFont::Describe_Entity()
{
}
#endif // _DEBUG
