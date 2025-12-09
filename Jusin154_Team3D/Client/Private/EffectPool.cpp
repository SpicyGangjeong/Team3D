#include "pch.h"
#include "EffectPool.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Effect_Container.h"
#include "Bombard.h"
#include "NomalJap.h"
#include "Decendo.h"
#include "Protego.h"
#include "Revelio.h"
#include "Levioso.h"

#include "BombardSide.h"
#include "LeviosoSide.h"
#include "DecendoSide.h"

#include "NomalJapSide.h"
#include "Lumos.h"


#include "Troll_Rush_Hit.h"
#include "Troll_Nomal_Smoke.h"
#include "TrollSwing.h"

CEffectPool::CEffectPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectPool::CEffectPool(const CEffectPool& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CEffectPool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectPool::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Effect())) {
		return E_FAIL;
	}

	if (FAILED(Ready_MonsterEffect())) {
		return E_FAIL;
	}
	

	return S_OK;
}

void CEffectPool::Priority_Update(_float fTimeDelta)
{
	for (auto iter = m_ActiveEffectList.begin(); iter != m_ActiveEffectList.end(); )
	{
		if (false == (*iter)->Get_Visible())
		{
			++iter;
		}
		else
		{
			(*iter)->Priority_Update(fTimeDelta);
			++iter;
		}


	}
}

void CEffectPool::Update(_float fTimeDelta)
{
	for (auto iter = m_ActiveEffectList.begin() ; iter != m_ActiveEffectList.end(); )
	{
		if (false == (*iter)->Get_Visible())
		{
			++iter;
		}
		else 
		{
			(*iter)->Update(fTimeDelta);
			++iter;
		}
	}
}

void CEffectPool::Late_Update(_float fTimeDelta)
{
	for (auto iter = m_ActiveEffectList.begin(); iter != m_ActiveEffectList.end(); )
	{
		if (false == (*iter)->Get_Visible())
		{
			SAFE_RELEASE(*iter);
			iter = m_ActiveEffectList.erase(iter);
		}
		else
		{
			(*iter)->Late_Update(fTimeDelta);
			++iter;
		}
	}
}

HRESULT CEffectPool::Render()
{
	return S_OK;
}


HRESULT CEffectPool::Ready_Components()
{
	if (FAILED(__super::Ready_Components(nullptr))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffectPool::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CEffectPool::Ready_Effect()
{
	if(FAILED(Create_Effect(SKILL_TYPE::JAP, 10, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {
		CNomalJap* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CNomalJap>(iPrototypeLevel, nullptr);

		return pEffect;}
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BOMBARDA, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CBombard* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBombard>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::DESCENDO, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CDecendo* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDecendo>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::PROTEGO, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CProtego* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CProtego>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::REVELIO, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CRevelio* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRevelio>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::JAP_SIDE, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CNomalJapSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CNomalJapSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::LEVIOSO, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLevioso* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLevioso>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::LUMOS, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLumos* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLumos>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BOMBARDA_SIDE, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CBombardSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBombardSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::LEVIOSO_SIDE, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLeviosoSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLeviosoSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::DESCENDO_SIDE, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CDecendoSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDecendoSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	return S_OK;
}

HRESULT CEffectPool::Ready_MonsterEffect()
{


	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_ATTACK, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTrollSwing* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTrollSwing>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_NOMAL_SMOKE, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Nomal_Smoke* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Nomal_Smoke>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_RUSH_HIT, 5, NEXT_LEVEL, NEXT_LEVEL, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Rush_Hit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Rush_Hit>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	return S_OK;
}

HRESULT CEffectPool::Create_Effect(SKILL_TYPE eType, _uint iNumEffect, _uint iPrototypeLevel, _uint iCloneLevel, function<CEffect_Container* (_uint, _uint)> AddPrototypeEvent)
{
	for (_uint i = 0; i < iNumEffect; i++)
	{
		CEffect_Container* pEffect_Container = AddPrototypeEvent(iPrototypeLevel, iCloneLevel);

		if (pEffect_Container == nullptr) {
			return E_FAIL;
		}

		m_EffectList[ENUM_CLASS(eType)].push_back(pEffect_Container);

	}

	return S_OK;
}

HRESULT CEffectPool::Use_Skill(SKILL_TYPE eType, CGameObject* pOwner, void* pArg)
{
	if (pOwner == nullptr)
		return S_OK;

	for (auto& pSkill : m_EffectList[ENUM_CLASS(eType)])
	{
		if (pSkill->Get_Visible() == true)
			continue;

		pSkill->Pre_Setting(pOwner, pArg);

		m_ActiveEffectList.push_back(pSkill);
		Safe_AddRef(pSkill);

		break;
	}

	return S_OK;
}


CEffectPool* CEffectPool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectPool* pInstance = new CEffectPool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectPool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectPool::Clone(void* pArg, CGameObject* pOwner)
{
	CEffectPool* pInstance = new CEffectPool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectPool");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEffectPool::Free()
{
	__super::Free();

	for (size_t i = 0; i < ENUM_CLASS(SKILL_TYPE::END); i++)
	{
		for (auto& pEffect : m_EffectList[i])
		{
			SAFE_RELEASE(pEffect);
		}
	}

	for (auto& pEffect : m_ActiveEffectList)
	{
		SAFE_RELEASE(pEffect);
	}
}
#ifdef _DEBUG

void CEffectPool::Describe_Entity()
{
}

#endif // _DEBUG
