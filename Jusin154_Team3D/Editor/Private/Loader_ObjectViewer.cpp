#include "pch.h"
#include "GameInstance.h"
#include "Loader.h"

#pragma region EFFECT_HEADER

#include "TrailObject.h"
#include "Bombard.h"
#include "EffectPool.h"
#pragma endregion

#pragma region OBJECT_HEADER
#include "RootModelPart.h"
#include "Head.h"
#include "Body.h"
#include "Hair.h"
#include "DummySkyBox.h"
#include "DummyObject.h"
#include "Player.h"
#include "Goblin.h"
#include "Broom.h"
#include "Camera_Gaze.h"
#include "CamPosition_Socket.h"
#include "CamPosition_Target.h"
#include "CamPosition_Shoulder.h"
#include "CamPosition_Arm.h"
#include "Wand.h"

#pragma endregion

HRESULT CLoader::Loading_For_ObjectViewer()
{
	m_strMessage = TEXT("Texture Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_NPC_PBR_ANIM,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_NPC_PBR_Anim.hlsl"),
			VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("TerrainTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), 0)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("Model Loading..");

	// Heavy Wall
	CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
	{
		Desc.eType = ACTOR::BOX;
		Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
		Desc.fContactOffset = { 0.05f };
		Desc.vhalfGeometryInfo = { 2.5f, 4.5f, 3.5f };
		Desc.fDensity = 10.f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 100.f, 100.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_HEAVY_WALL"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), CCharacter_Controller::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	vector<future<pair<_wstring, CModel*>*>> futures = {};

#pragma region MONSTER

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/Goblin.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin_Mage/GoblinMage.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_GoblinMage_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/Goblin_Assassin/SK_GOB_M_Assassin_Master.bin", XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_SK_GOB_M_Assassin_Master_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin_Assassin/SK_GOB_M_Assassin_Master_Tumble.fbx", XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_SK_GOB_M_Assassin_Master_Model")
	));

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinSpector_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/GoblinSpector/GoblinSpector.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Playable_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable.bin", XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/TombProtector/TombProtector.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_TombProtector_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/SubTroll/troll.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_troll_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Monster/Dragon/Dragon.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Dragon_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/ConjuredDragon/ConjuredDragon.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_ConjuredDragon_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/EleazarFig/Professor_EleazarFig.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Professor_EleazarFig_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/Ghost_Peeves/Ghost_Peeves.bin",XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Ghost_Peeves_Model")
	));


#pragma endregion

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Troll_Weapon/Troll_Weapon.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Weapon_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/Troll_Rock.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Rock_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/Troll_Rock_Big.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Rock_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Goblin_Dagger/Goblin_Dagger.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Dagger_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/Npc.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Npc_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/Wand.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_Wand_Model")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/Broom/Broom.bin",XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Broom_Model")
	));


	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Reparo_Object/VFX_SK_OLI_TrollFight_BlockerA.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerA_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Reparo_Object/VFX_SK_OLI_TrollFight_BlockerB.bin", XMMatrixRotationY(XMConvertToRadians(180.f))* XMMatrixIdentity(),
		TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerB_Model")
	));


	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_SkyboxModel")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixIdentity(),
		TEXT("Desc_Box")
	));
	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/GerboldOllivander/GerboldOlivander.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_GerboldOlivander_Model")
	));

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/VictorRookWood/VictorRookWood.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_VictorRookWood_Model")
	));


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CCL_CameraRig.001_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Object/Camera/CCL_CameraRig.001.bin", XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity()))))
		return E_FAIL;

	futures.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable.fbx", XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Playable_Model")
	));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Cmbt.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Rct.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Sneak.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Bm.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Broom.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Lightning.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Protego.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Spell_Learning.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_Spawn.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	//futures.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable_AvadaKedavra.fbx", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Student_Cmbt_Model")
	//));

	for (auto& job : futures) {
		pair<_wstring, CModel*>* pResult = job.get();
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pResult->first, pResult->second))) {
			assert(false);
			return E_FAIL;
		}
		Safe_Delete(pResult);
	}


	m_strMessage = TEXT("Shader Loading..");

	m_strMessage = TEXT("Prototype Loading..");

	///* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, nullptr, 100, 100))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_RootModelPart */
	if (FAILED(m_pGameInstance->Add_Prototype<CRootModelPart>(g_iStaticLevel, CRootModelPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body */
	if (FAILED(m_pGameInstance->Add_Prototype<CBody>(g_iStaticLevel, CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Head */
	if (FAILED(m_pGameInstance->Add_Prototype<CHead>(g_iStaticLevel, CHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hair */
	if (FAILED(m_pGameInstance->Add_Prototype<CHair>(g_iStaticLevel, CHair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DummyObject */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummyObject>(g_iStaticLevel, CDummyObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DummySkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummySkyBox>(g_iStaticLevel, CDummySkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype<CFSM>(g_iStaticLevel, CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CPlayer>(g_iStaticLevel, CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin>(g_iStaticLevel, CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Broom */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom>(g_iStaticLevel, CBroom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Gaze */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Gaze>(g_iStaticLevel, CCamera_Gaze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Socket>(g_iStaticLevel, CCamPosition_Socket::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Shoulder */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Shoulder>(g_iStaticLevel, CCamPosition_Shoulder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Target */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Target>(g_iStaticLevel, CCamPosition_Target::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Arm */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Arm>(g_iStaticLevel, CCamPosition_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Wand */
	if (FAILED(m_pGameInstance->Add_Prototype<CWand>(g_iStaticLevel, CWand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CTrail>(g_iStaticLevel, CTrail::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTrailObject>(g_iStaticLevel, CTrailObject::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBombard>(g_iStaticLevel, CBombard::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEffectPool>(g_iStaticLevel, CEffectPool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;

	return S_OK;
}

