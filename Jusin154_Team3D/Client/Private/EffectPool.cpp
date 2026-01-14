#include "pch.h"
#include "EffectPool.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Effect_Container.h"
#include "Bombard.h"
#include "NomalJap.h"
#include "Duelist_NormalJap.h"
#include "Decendo.h"
#include "Protego.h"
#include "Duelist_Protego.h"
#include "Revelio.h"
#include "Levioso.h"
#include "Duelist_Levioso.h"

#include "BombardSide.h"
#include "LeviosoSide.h"
#include "DecendoSide.h"
#include "AvadakedavraSide.h"
#include "StupefySide.h"
#include "TransformationSide.h"
#include "AccioSide.h"

#include "NomalJapSide.h"
#include "Lumos.h"
#include "WandEnd.h"

#include "Protego_Hit.h"

#include "Troll_Rush_Hit.h"
#include "Troll_Nomal_Smoke.h"
#include "TrollSwing.h"
#include "Troll_Rush.h"
#include "Troll_Shout.h"
#include "Troll_Self_Hit.h"

#include "Goblin_Protego.h"
#include "Goblin_Attack.h"
#include "Mage_Down_Attack.h"
#include "Mage_Nomal_Attack.h"
#include "MageSide.h"
#include "StunEffect.h"
#include "Box_Splesh.h"
#include "Chair_Splesh.h"
#include "Barral_Splesh.h"
#include "Goblin_Teleport.h"
#include "Accio.h"
#include "Screen_Wind.h"
#include "Stupefy.h"
#include "Lightning.h"
#include "LightningSide.h"
#include "Blink.h"
#include "Transformation.h"
#include "Avadakedavra.h"
#include "Reparo.h"
#include "PotionBroken.h"
#include "PotionScreen.h"
#include "HitScreen.h"
#include "BroomRace_Bubble.h"

#include "Goblin_ProtegoHit.h"
#include "Duelist_ProtegoHit.h"

#include "Ranrok_FireBall.h"
#include "Ranrok_Breath.h"
#include "Ranrok_Point.h"
#include "Ranrok_Pulse.h"
#include "Ranrok_Charge.h"
#include "Ranrok_Hit.h"
#include "Ranrok_Impact.h"
#include "Ranrok_Land.h"
#include "Ranrok_GroundPulse.h"
#include "Ranrok_Rush_Bottom.h"
#include "Ranrok_Swipe.h"
#include "Ranrok_DeadSplash.h"
#include "Ranrok_DeadImpact.h"
#include "Ranrok_Prop.h"

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
	m_isActiveEffectCreate = true;
	m_isActiveMonsterEffectCreate = true;
	// 디버그 모드일 때
#ifdef _DEBUG
#ifdef gimch
	m_isActiveEffectCreate = true;
	m_isActiveMonsterEffectCreate = false;
#endif // gimch
#ifdef Bin
	m_isActiveEffectCreate = true;
	m_isActiveMonsterEffectCreate = true;
#endif // 
#ifdef 진우
	m_isActiveEffectCreate = true;
	m_isActiveMonsterEffectCreate = true;
#endif // 
#ifdef 기무리
	m_isActiveEffectCreate = true;
	m_isActiveMonsterEffectCreate = false;
#endif // 
#ifdef 나
	m_isActiveEffectCreate = true;
	m_isActiveMonsterEffectCreate = true;
#endif // 
#endif
	if (m_isActiveEffectCreate)
	{
		if (FAILED(Ready_Effect()))
			return E_FAIL;
	}

	if (m_isActiveMonsterEffectCreate)
	{
		if (FAILED(Ready_MonsterEffect()))
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
#ifdef  _DEBUG
#if 진우


	//(*m_EffectList[ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA_SIDE)].begin())->Describe_Entity();

	//Describe_Entity();

#endif
#endif


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
	if (FAILED(Create_Effect(SKILL_TYPE::JAP, 10, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {
		CNomalJap* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CNomalJap>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::DUELIST_JAP, 10, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {
		CDuelist_NormalJap* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDuelist_NormalJap>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BOMBARDA, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CBombard* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBombard>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::DESCENDO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CDecendo* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDecendo>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::PROTEGO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CProtego* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CProtego>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::DUELIST_PROTEGO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CDuelist_Protego* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDuelist_Protego>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::REVELIO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CRevelio* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRevelio>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::JAP_SIDE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel)-> CEffect_Container* {

		CNomalJapSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CNomalJapSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::LEVIOSO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLevioso* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLevioso>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::DUELIST_LEVIOSO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CDuelist_Levioso* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDuelist_Levioso>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::LUMOS, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLumos* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLumos>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::ACCIO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CAccio* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CAccio>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BOMBARDA_SIDE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CBombardSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBombardSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::LEVIOSO_SIDE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLeviosoSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLeviosoSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::DESCENDO_SIDE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CDecendoSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDecendoSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::WAND_END, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CWandEnd* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CWandEnd>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BOX_SPLESH, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CBox_Splesh* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBox_Splesh>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::CHAIL_SPLESH, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CChair_Splesh* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CChair_Splesh>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BARRAL_SPLASH, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CBarral_Splesh* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBarral_Splesh>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::SCREEN_WIND, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CScreen_Wind* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CScreen_Wind>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::STUPEFY, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CStupefy* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CStupefy>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::LIGHTNING, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLightning* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLightning>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::LIGHTNING_SIDE, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CLightningSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CLightningSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::BLINK, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CBlink* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBlink>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::TRANSFORMATION, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTransformation* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTransformation>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::TRANSFORMATION_SIDE, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTransformationSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTransformationSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::AVADAKEDAVRA, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CAvadakedavra* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CAvadakedavra>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::AVADAKEDAVRA_SIDE, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CAvadakedavraSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CAvadakedavraSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;
	

	if (FAILED(Create_Effect(SKILL_TYPE::PROTEGO_HIT, 10, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CProtego_Hit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CProtego_Hit>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::STUPEFY_SIDE, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CStupefySide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CStupefySide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::ACCIO_SIDE, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CAccioSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CAccioSide>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::REPARO, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CReparo* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CReparo>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::POTION_BROKEN, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CPotionBroken* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CPotionBroken>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::SCREEN_POTION, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CPotionScreen* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CPotionScreen>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::SCREEN_HIT, 3, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CHitScreen* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CHitScreen>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::BROOM_RACE_BUBBLE, 20, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CBroomRace_Bubble* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CBroomRace_Bubble>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	return S_OK;
}

HRESULT CEffectPool::Ready_MonsterEffect()
{


	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_ATTACK, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTrollSwing* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTrollSwing>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_NOMAL_SMOKE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Nomal_Smoke* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Nomal_Smoke>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_RUSH_HIT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Rush_Hit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Rush_Hit>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::GOBLIN_PROTEGO, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CGoblin_Protego* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CGoblin_Protego>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::GOBLIN_ATTACK, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CGoblin_Attack* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CGoblin_Attack>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::GOBLIN_MAGE_ATTACK, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CMage_Nomal_Attack* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CMage_Nomal_Attack>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::GOBLIN_MAGE_DOWN, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CMage_Down_Attack* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CMage_Down_Attack>(iPrototypeLevel, nullptr);

		return pEffect; }


	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::GOBLIN_MAGE_SIDE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CMageSide* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CMageSide>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::STUN, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CStunEffect* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CStunEffect>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::GOBILN_TELEPORT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CGoblin_Teleport* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CGoblin_Teleport>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_FIREBALL, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_FireBall* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_FireBall>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_BREATH, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Breath* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Breath>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_POINT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Point* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Point>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_PURSE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Pulse* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Pulse>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_CHARGE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Charge* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Charge>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_HIT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Hit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Hit>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_IMPACT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Impact* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Impact>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;
	
	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_LAND, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Land* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Land>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_GROUNDPULSE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_GroundPulse* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_GroundPulse>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_RUSH_BOTTOM, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Rush_Bottom* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Rush_Bottom>(iPrototypeLevel, nullptr);

		return pEffect; }

	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_SWIPE, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Swipe* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Swipe>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_DEAD_SPLASH, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_DeadSplash* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_DeadSplash>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_DEAD_IMPACT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_DeadImpact* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_DeadImpact>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;
	
	if (FAILED(Create_Effect(SKILL_TYPE::RANROK_PROP, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CRanrok_Prop* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CRanrok_Prop>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::GOBLIN_PROTEGO_HIT, 10, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CGoblin_ProtegoHit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CGoblin_ProtegoHit>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	if (FAILED(Create_Effect(SKILL_TYPE::DUELIST_PROTEGO_HIT, 10, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CDuelist_ProtegoHit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CDuelist_ProtegoHit>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_SHOUT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Shout* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Shout>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;


	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_RUSH, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Rush* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Rush>(iPrototypeLevel, nullptr);

		return pEffect; }
	))) return E_FAIL;

	
	if (FAILED(Create_Effect(SKILL_TYPE::TROLL_SELF_HIT, 5, g_iStaticLevel, g_iStaticLevel, [&](_uint iPrototypeLevel, _uint iCloneLevel) -> CEffect_Container* {

		CTroll_Self_Hit* pEffect = nullptr;

		pEffect = m_pGameInstance->Clone_Prototype<CTroll_Self_Hit>(iPrototypeLevel, nullptr);

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

HRESULT CEffectPool::Use_Skill(SKILL_TYPE eType, CGameObject* pOwner, void* pArg, CEffect_Container** ppOut)
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

		if (nullptr != ppOut) {
			*ppOut = pSkill;
			SAFE_ADDREF(pSkill);
		}

		break;
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CEffectPool::Reset_Pool()
{
	for (size_t i = 0; i < ENUM_CLASS(SKILL_TYPE::END); i++)
	{
		for (auto& pEffect : m_EffectList[i])
		{
			SAFE_RELEASE(pEffect);
		}

		m_EffectList[i].clear();
	}

	for (auto& pEffect : m_ActiveEffectList)
	{
		SAFE_RELEASE(pEffect);
	}


	m_ActiveEffectList.clear();


	if (FAILED(Ready_Effect()))
		return E_FAIL;

	return S_OK;
}
#endif

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

	GUI::Begin("Effect Pool");
	if (GUI::Button("Reset Pool"))
	{
		Reset_Pool();
	}
	GUI::End();

}

#endif // _DEBUG
