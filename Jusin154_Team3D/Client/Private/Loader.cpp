#pragma region HEADER
#include "pch.h"
#include "Camera_Debug.h"
#include "Client_Struct.h"
#include "GameInstance.h"
#include "Light_Main.h"
#include "Loader.h"
#include "MainApp.h"
#include "CamPosition_WorldLook.h"
#include "Model.h"

#include "Broom.h"
#include "Camera_Gaze.h"
#include "Camera_Cinematic.h"
#include "CamPosition_Arm.h"
#include "CamPosition_Shoulder.h"
#include "CamPosition_Socket.h"
#include "CamPosition_Target.h"
#include "CamPosition_AI.h"
#include "SkyBox.h"
#include "Troll_Weapon.h"
#include "Troll_Rock.h"
#include "Goblin_Dagger.h"
#include "Wand.h"
#include "Item_Potion.h"
#include "RaceRing.h"
#include "Goblin_Sword.h"
#include "Goblin_BattleAxe.h"
#include "BroomRaceManager.h"
#include "ReparoObject.h"
#include "ThestralCarriage.h"

#pragma region ACTOR

#include "Player.h"
#include "Human_Duelist.h"
//#include "PlayerRobe.h"
#include "Goblin.h"
#include "Goblin_Mage.h"
#include "Goblin_Assassin.h"
#include "Goblin_Spector.h"
#include "Goblin_Assassin_Spector.h"
#include "Troll.h"
#include "NPC_Ollivander.h"
#include "NPC_EleazarFig.h"
#include "BroomRacerAI.h"
#include "Ranrok.h"
#include "RandomNpc.h"
#include "Elf.h"
#pragma endregion


#pragma region UI

#include "UI_Manager.h"

#include "Damage_Font.h"
#include "Dialogue.h"
#include "Dialogue_Font.h"

#include "Logo.h"
#include "Logo_Text.h"
#include "Logo_Glow.h"

#include "Mouse_Cursor.h"
#include "CameraLockOn.h"

#include "Loding_Panel.h"

#include "GamePlay_Canvas.h"

#include "Mission_Panel.h"
//#include "MissionBanner_Border.h"
//#include "MissionBanner_Key.h"
//#include "Mission_Icon.h"
//#include "Mouse_Cursor.h"
#include "MiniMap_TrimBorder.h"

#include "MiniMap_Panel.h"
#include "NoMountIcon.h"

#include "Loading_Panel.h"
#include "LoadingWidget_Flame.h"

#include "Action_Panel.h"
#include "Spell_Slot.h"
#include "Spell_Overlay.h"
#include "Slot_Number.h"
#include "HpBarBG.h"
#include "Potion.h"
#include "Magic_Meter.h"
#include "Magic_Icon.h"
#include "Spell_UI.h"
#include "Magic_Item.h"

#include "Enemy_Panel.h"
#include "Enemy_HpBar.h"
#include "Enemy_Info.h"
#include "Enemy_SkillUI.h"
#include "Boss_HpBar.h"
#include "Enemy_Detection.h"

#include "Spell_Canvas.h"
#include "Spell_Panel.h"
#include "Current_Spell_Slot.h"
#include "Spell_List.h"
#include "Eessential_Spell_Slot.h"
#include "Eessential_Spell.h"
#include "Spell_List_Image.h"
#include "Spell_State.h"
#include "Spell_Hover.h"
#include "Spell_Hover_Effect.h"
#include "Spell_Preview.h"
#include "Spell_Header.h"
#include "Spell_Header_Line.h"
#include "Spell_Vidio_Border.h"
#include "Spell_Anim.h"
#include "Current_Slot_Number.h"
#include "Spell_Drag.h"

#include "Quest_Canvas.h"
#include "Quest_Panel.h"
#include "Quest_Info.h"
#include "Quest_Info_Header.h"
#include "Quest_Info_Line.h"
#include "Quest_Entry_Line.h"
#include "Quest_Status.h"
#include "Quest_Slot.h"

#include "QuestInstance.h"

#include "SpellLearn_Canvas.h"
#include "SpellLearn_Panel.h"
#include "SpellLearn_Name.h"
#include "SpellLearn.h"
#include "SpellLearn_MovePointer.h"
#include "SpellLearn_ChaserPointer.h"
#include "SpellLearn_LookPointer.h"
#include "SpellLearn_Booster.h"
#include "SpellLearn_Slot.h"
#include "SpellLearn_Overlay.h"

#include "Broom_Panel.h"
#include "Broom_Flag.h"
#include "Broom_Circle.h"
#include "Broom_Scoreboard.h"
#include "Broom_TargetGate.h"
#include "Broom_Finish.h"
#include "Broom_Record.h"
#include "Broom_Exit.h"
#include "Broom_Trophy.h"
#include "Broom_TargetGate2D.h"

#include "Ride_Panel.h"
#include "Ride_Info_Key.h"
#include "Ride_Info.h"
#include "Ride_InfoBG.h"
#include "Ride_Booster_Slot.h"
#include "Ride_BoosterBar.h"
#include "Ride_HpBar.h"
#include "Ride_HpSlot.h"

#include "Dialogue_Canvas.h"
#include "Dialogue_Panel.h"
#include "Dialogue_Choice.h"

#include "Interaction_Key.h"

#include "NPCInteraction.h"

#pragma endregion

#pragma region PHYSX
#include "PhysXEffectHitBox.h"

#pragma endregion

#pragma region EFFECT

#include "EffectParts.h"
#include "Bombard.h"
#include "Decendo.h"
#include "NomalJap.h"
#include "Duelist_NormalJap.h"
#include "Protego.h"
#include "Duelist_Protego.h"
#include "Revelio.h"
#include "NomalJapSide.h"
#include "DecendoSide.h"
#include "BombardSide.h"
#include "LeviosoSide.h"
#include "AccioSide.h"
#include "TransformationSide.h"
#include "AvadakedavraSide.h"
#include "Reparo.h"

#include "Protego_Hit.h"
#include "Goblin_ProtegoHit.h"
#include "Duelist_ProtegoHit.h"

#include "TrailObject.h"
#include "Instance_Model.h"
#include "Trail.h"
#include "EffectPool.h"
#include "Levioso.h"
#include "Duelist_Levioso.h"
#include "Duelist_JapSide.h"
#include "Lumos.h"
#include "Accio.h"
#include "Blink.h"
#include "Avadakedavra.h"
#include "Goblin_Teleport.h"

#include "PotionBroken.h"
#include "PotionScreen.h"
#include "HitScreen.h"
#include "BroomRace_Bubble.h"

#include "TrollSwing.h"
#include "Troll_Nomal_Smoke.h"
#include "Troll_Rush_Hit.h"
#include "Troll_Rush.h"
#include "Troll_Shout.h"
#include "Troll_Self_Hit.h"

#include "WandEnd.h"
#include "Goblin_Protego.h"
#include "Goblin_Attack.h"

#include "Mage_Down_Attack.h"
#include "Mage_Nomal_Attack.h"
#include "MageSide.h"
#include "Ranrok_FireBall.h"
#include "Ranrok_Breath.h"
#include "Ranrok_Point.h"
#include "Ranrok_Charge.h"
#include "Ranrok_Pulse.h"
#include "Ranrok_Hit.h"
#include "Ranrok_Impact.h"
#include "Ranrok_Land.h"
#include "Ranrok_GroundPulse.h"
#include "Ranrok_Rush_Bottom.h"
#include "Ranrok_Swipe.h"
#include "Ranrok_DeadSplash.h"
#include "Ranrok_DeadImpact.h"
#include "Ranrok_Prop.h"
#include "Ranrok_PropHit.h"

#include "StunEffect.h"
#include "Box_Splesh.h"
#include "Chair_Splesh.h"
#include "Barral_Splesh.h"
#include "Screen_Wind.h"
#include "Stupefy.h"
#include "StupefySide.h"
#include "Lightning.h"
#include "LightningSide.h"
#include "Transformation.h"

#pragma endregion

#pragma region MAP

#include "Terrain.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Model_Instance.h"
#include "BuildingContainer.h"
#include "StreetContainer.h"
#include "MapObject_Render.h"
#include "MapObject_Collision.h"
#include "InstancedProp.h"
#include "InstancedProp_Light.h"
#include "MapElement_Light.h"
#include "MapElement_Interactable.h"
#include "MapElement_Lake.h"
#include "MapElement_Door.h"
#include "Land.h"
#include "Unified.h"
#include "MapElement_Chest.h"
#include "MapElement_Chest_Lid.h"
#include "MapElement_Static.h"
#include "WorldDecal.h"
#include "PointLight.h"
#include "MapElement_Balloon.h"
#include "MapElement_Cave.h"

#pragma endregion

#pragma endregion
CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading())) {
		assert(false);
		int a = 0;
		return 1;
	}

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
////
	m_pGameInstance->EnqueueJob(&LoadingMain, this);

	return S_OK;
}

HRESULT CLoader::Loading()
{

	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return E_FAIL;
	}

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case LEVEL::FIELD:
		hr = Loading_For_Field();
		break;
	case LEVEL::RESTART:
		hr = Loading_For_Restart();
		break;
	default:
		break;
	}

	CoUninitialize();

	if (FAILED(hr)){
		return E_FAIL;
	}

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	Asset_FileLoad("../Bin/Resources/Textures/Logo", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);

			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::LOGO), wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0, L"", ENUM_CLASS(LEVEL::LOGO))))) {
				return E_FAIL;
			}

			return S_OK;

		});

	m_strMessage = TEXT("사운드를(을) 로딩 중 입니다.");

	const _tchar* pSoundFilePath = TEXT("../Bin/Resources/Sounds/");

	 for (int iIndex = 0; iIndex < ENUM_CLASS(SOUND::SD_KIND::END); ++iIndex) {
		 if (FAILED(m_pGameInstance->Load_Sound((SOUND::SD_KIND)iIndex, (_wstring(pSoundFilePath) + SOUND::SD_PATH::SD_KIND_PATHS[iIndex]).c_str(), FMOD_DEFAULT))) {
			 return E_FAIL;
		 }
	 }

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

#ifdef 기무리
	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIEDITOR,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIINSTANCE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UI_Instance.hlsl"),
			VTX_POSTEX_INSTANCE_UI::Elements, VTX_POSTEX_INSTANCE_UI::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_PROP_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxWorldModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_MODEL::Elements, VTX_MODEL_INSTANCE_MODEL::iNumElements)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_VTXPOS,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPos.hlsl"),
			VTXPOS::Elements, VTXPOS::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_NPC_PBR_ANIM,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_NPC_PBR_Anim.hlsl"),
			VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_MESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH::Elements, VTXMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_ANIMMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxAnimMesh.hlsl"),
			VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_POSTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPosTex.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_NORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxNorTex.hlsl"),
			VTXNORTEX::Elements, VTXNORTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_RANROK_ETHER,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxRanrokEther.hlsl"),
			VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}

#endif // 기무리

#if 진우
#pragma region SHADER
	vector<future<pair<_wstring, CShader*>*>> jobMapShaders;

	{
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_UIEDITOR,
			TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_UIINSTANCE,
			TEXT("../Bin/Resources/ShaderFiles/Shader_UI_Instance.hlsl"), VTX_POSTEX_INSTANCE_UI::Elements, VTX_POSTEX_INSTANCE_UI::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_INSTANCE_MODEL,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_INSTANCE_PROP_MODEL,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxWorldModelInstance.hlsl"), VTX_MODEL_INSTANCE_MODEL::Elements, VTX_MODEL_INSTANCE_MODEL::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_NPC_PBR_ANIM,
			TEXT("../Bin/Resources/ShaderFiles/Shader_NPC_PBR_Anim.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_ANIMMESH,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_POSTEX,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_NORTEX,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_MESH,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_VTXPOS,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPos.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements));
		jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
			FX_RANROK_ETHER,
			TEXT("../Bin/Resources/ShaderFiles/Shader_VtxRanrokEther.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements));
	}

	for (auto& JobMapShader : jobMapShaders)
	{
		pair<_wstring, CShader*>* pOut = JobMapShader.get();
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pOut->first, pOut->second))) {
			return E_FAIL;
		}
		Safe_Delete(pOut);
	}

#endif

#pragma endregion

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Debug>(g_iStaticLevel, CCamera_Debug::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLight_Main>(g_iStaticLevel, CLight_Main::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLogo>(g_iStaticLevel, CLogo::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLogo_Text>(g_iStaticLevel, CLogo_Text::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLogo_Glow>(g_iStaticLevel, CLogo_Glow::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}



	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	_bool isLoad_Background = { true };
	_bool isLoad_Hogwart = { true };
	_bool isLoad_UI_SEQUANTIAL = { true };
	_bool isLoad_NPC = { true };
	_bool isLoad_Monster = { true };
	_bool isLoad_DataClassroom = { true };

#ifdef _DEBUG
#ifdef gimch
	isLoad_Background = true;
	isLoad_Hogwart = false;
	isLoad_UI_SEQUANTIAL = false;
	isLoad_NPC = true;
	isLoad_DataClassroom = false;
#endif // gimch
#ifdef 진우
	isLoad_Background = false;
	isLoad_Hogwart = false;
	isLoad_UI_SEQUANTIAL = false;
#endif // 
#ifdef 기무리
	isLoad_Background = true;
	isLoad_Hogwart = true;
	isLoad_UI_SEQUANTIAL = false;
	isLoad_NPC = true;
	isLoad_Monster = true;
#endif // 
#ifdef 나
	isLoad_Background = false;
	isLoad_Hogwart = false;
	isLoad_UI_SEQUANTIAL = true;
	isLoad_Monster = true;
#endif // 
#ifdef Bin
	isLoad_Background = false;
	isLoad_Hogwart = false;
	isLoad_UI_SEQUANTIAL = false;
#endif // Bin
#endif // _DEBUG
#ifndef _DEBUG
#ifdef gimch
	isLoad_Background = true;
	isLoad_Hogwart = false;
	isLoad_UI_SEQUANTIAL = false;
	isLoad_NPC = true;
	isLoad_DataClassroom = false;
#endif // gimch
#endif // 

#pragma region MAP_MODELS
	vector<future<vector<FOLDER_LOAD*>*>> jobMapModels;
	vector<future<pair<_wstring, CModel*>*>> jobCharacterModels = {};
	vector<future<pair<_wstring, CShader*>*>> jobMapShaders;
	{
		{ /* Terrain */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/Terrain",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Terrain",
				".bin", false
			));
		}

		{ /* Ollivanders*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Ollivanders/Meshes",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Ollivanders/Collision",
				".bin", false
			));
		}

		{ /* Gatehouse*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Gatehouse/Meshes",
				".bin", false
			));
		}
		{	/* Hogwart LOD */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/HogwartsLOD",
				".bin", false
			));
		}
		{	/* Hogsmeade LOD */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/HogsmeadeLOD/ProxyAssets",
				".bin", false
			));
		}
		/* Interactable */
		{/* Barrel */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Objects/Meshes",
				".bin", false
			));
			/* Box */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/MiscProps",
				".bin", false
			));
			/* TeaShop Table */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Tables",
				".bin", false
			));
			/* TeaShop Chair*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Chairs",
				".bin", false
			));
			/* Object Interactables (Chest)*/
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Objects/Interactables",
				".bin", false
			));
		}

		if (true == isLoad_Background)
		{
			{ /* TScrolls */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TScrolls/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TScrolls/Collisions",
					".bin", false
				));
			}
			{ /* 3BroomStick*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Collision",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_ThreeBroomsticks/Meshes/3Broom_Kit",
					".bin", false
				));
			}


			{ /* BLDG_QuidditchShop */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_QuidditchShop/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_QuidditchShop/Collision",
					".bin", false
				));
			}
			{ /* BLDG_HogsheadInn */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_HogsheadInn/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_HogsheadInn/Collision",
					".bin", false
				));
			}
			{ /* BLDG_Honeydukes */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Honeydukes/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Honeydukes/Collision",
					".bin", false
				));
			}
			{ /* BLDG_OwlPost */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_OwlPost/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_OwlPost/Collision",
					".bin", false
				));
			}
			{ /* BLDG_TeaShop */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TeaShop/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_TeaShop/Collision",
					".bin", false
				));
			}
			{ /* BLDG_Zonkos */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Zonkos/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Zonkos/Collision",
					".bin", false
				));
			}
			{ /* BLDG_DB_GR */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_DB_GR/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_DB_GR/Collision",
					".bin", false
				));
			}
			{ /* BLDG_Potions */
				/*jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Potions/Meshes",
					".bin", false
				));*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Potions/Collisions",
					".bin", false
				));
			}
			{ /* BLDG_Salon */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Salon/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_Salon/Collision",
					".bin", false
				));
			}
			{ /* Hengist_Tree */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Vegetation/Hengist_Tree",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/Vegetation",
					".bin", false
				));
			}
			{ /* GEN A*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_A/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_A/Collision",
					".bin", false
				));
			}
			{ /* GEN B*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_B/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_B/Collision",
					".bin", false
				));
			}
			{ /* GEN C*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_C/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_C/Collision",
					".bin", false
				));
			}
			{ /* GEN E*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_E/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_E/Collision",
					".bin", false
				));
			}
			{ /* GEN F*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_F/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_F/Collision",
					".bin", false
				));
			}
			{ /* GEN G*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_G/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_G/Collision",
					".bin", false
				));
			}
			{ /* GEN H*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_H/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_H/Collision",
					".bin", false
				));
			}
			{ /* GEN J*/
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_J/Meshes",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/BLDG_GEN_J/Collision",
					".bin", false
				));
			}
			{/* Light Objects */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/LightPosts",
					".bin", false
				));
			}
			{/* Doors */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/Doors",
					".bin", false
				));
			}
			{/* Step and Stair */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Meshes/GroundSurfaces",
					".bin", false
				));
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/Common/Collision/GroundSurfaces",
					".bin", false
				));
			}
			{	/* SUB_HogsHead */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_HogsHead/SUB_HogsHead_EXTLOD/ProxyAssets",
					".bin", false
				));
				/* SUB_PippensPotions */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_PippensPotions/SUB_Pippens_EXTLOD/ProxyAssets",
					".bin", false
				));
				/* GEN_E_LOD */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_GEN_E/SUB_GEN_E_EXTLOD/ProxyAssets",
					".bin", false
				));
				/* GEN_F_LOD */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_GEN_F/SUB_GEN_F_EXTLOD/ProxyAssets",
					".bin", false
				));
				/* GEN_G_LOD */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_GEN_G/SUB_GEN_G_EXTLOD/ProxyAssets",
					".bin", false
				));
				/* GEN_H_LOD */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_GEN_H/SUB_GEN_H_EXTLOD/ProxyAssets",
					".bin", false
				));
				/* GEN_J_LOD */
				jobMapModels.emplace_back(Deferred_FolderLoad(
					"../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/SUB_GEN_J/SUB_GEN_J_EXTLOD/ProxyAssets",
					".bin", false
				));
			}
		}

		/* ------------------------------------ HOGWART ------------------------------- */
		if (true == isLoad_Hogwart)
		{
			/* QuidditchPitch */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_QuidditchPitch/Static_Mesh/KIT_Ext",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_QuidditchPitch/Static_Mesh/Collisions",
				".bin", false
			));

			/* Greenhouse */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_Greenhouses/Static_Mesh/Kit_EXT",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_Greenhouses/SUB_Greenhouses_EXTLOD/ProxyAssets",
				".bin", false
			));

			/* ViaductEntrance */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ViaductEntrance/SUB_ViaductEntrance_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ViaductEntrance/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* CentralTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_CentralTower/SUB_CentralTower_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_CentralTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* Library */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_Library/SUB_Library_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_Library/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* BellTowers */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_BellTowers/SUB_BellTowers_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_BellTowers/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* GrandStaircaseTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GrandStaircaseTower/SUB_GrandStaircaseTower_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GrandStaircase/SUB_GrandStaircase_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GrandStaircaseTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* ViaductCourtyard */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ViaductCourtyard/SUB_ViaductCourtyard_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ViaductCourtyard/Static_Mesh",
				".bin", false
			));

			/* ViaductBridge */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ViaductBridge/SUB_ViaductBridge_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ViaductBridge/Static_Mesh",
				".bin", false
			));

			/* Boathouse */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_Boathouse/SUB_Boathouse_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_Boathouse/Static_Mesh",
				".bin", false
			));

			/* BoathouseStairway */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_BoathouseStairway/SUB_BoathouseStairway_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_BoathouseStairway/Static_Mesh",
				".bin", false
			));

			/* EntranceHall */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_EntranceHall/SUB_EntranceHall_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_EntranceHall/Static_Mesh/Kit_Ext",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_EntranceHall/Static_Mesh",
				".bin", false
			));

			/* GreatHall */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GreatHall/SUB_GreatHall_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GreatHall/Static_Mesh/Kit_Ext",
				".bin", false
			));

			/* Ravenclaw */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_RavenclawTower/SUB_RavenclawTower_ExtLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_RavenclawTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* QuadCourtyard */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_QuadCourtyard/SUB_QuadCourtyard_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_QuadCourtyard/Static_Mesh",
				".bin", false
			));

			/* HospitalWing */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_HospitalWing/SUB_HospitalWing_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_HospitalWing/Static_Mesh",
				".bin", false
			));

			/* GryffindorTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GryffindorTower/SUB_GryffindorTower_ExtLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_GryffindorTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* FacultyTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_FacultyTower/SUB_FacultyTower_ExtLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_FacultyTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* NorthTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_NorthTower/SUB_NorthTower_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_NorthTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* NorthHall */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_NorthHall/SUB_NorthHall_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_NorthHall/Static_Mesh/StaticMesh/Kit_EXT",
				".bin", false
			));

			/* AstronomyTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_AstronomyTower/SUB_AstronomyTower_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_AstronomyTower/Static_Mesh/EXT",
				".bin", false
			));

			/* ClockTower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ClockTower/SUB_ClockTower_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ClockTower/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* TransfigurationCourtyard */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_TransfigurationCourtyard/SUB_TransfigurationCourtyard_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_TransfigurationCourtyard/Static_Mesh",
				".bin", false
			));

			/* ClockTowerCourtyard */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ClockTowerCourtyard/SUB_ClockTowerCourtyard_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_ClockTowerCourtyard/Static_Mesh/Kit_EXT",
				".bin", false
			));

			/* DADATower */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_DADATower/SUB_DADATower_EXTLOD/ProxyAssets",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_DADATower/StaticMesh/EXT",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Overland_Common/Meshes",
				".bin", false
			));
		}

		if (true == isLoad_DataClassroom)
		{
			/* DADA INT */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/SUB_DADATower/StaticMesh/INT",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/Meshes/Doors",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/Meshes/Doorway",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/Meshes/Props",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/Meshes/Props/SM_HW_DADA_DragonBones",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/Meshes/PictureFrames",
				".bin", false
			));
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Hogwarts/Meshes/PictureFrames",
				".bin", false
			));
		}
	}

	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Box/Box.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Box")
	));

#pragma region ACTOR

	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/PlayableCharacter/Playable.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Playable_Model")
	));
	if (true == isLoad_NPC) {
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::ANIM, "../Bin/Resources/Models/Human/Npc/Npc.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Npc_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/GerboldOllivander/GerboldOlivander.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_GerboldOlivander_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/EleazarFig/Professor_EleazarFig.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Professor_EleazarFig_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/VictorRookWood/VictorRookWood.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_VictorRookWood_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/ChiyoKogawa/ChiyoKogawa.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_ChiyoKogawa_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/MatildaWeasely/MatildaWeasely.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_MatildaWeasely_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/SatyavatiShah/SatyavatiShah.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_SatyavatiShah_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/Ghost_Peeves/Ghost_Peeves.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Ghost_Peeves_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/M_Student/M_Student.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_M_Student_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/PhineasBlack/PhineasBlack.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_PhineasBlack_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/F_Student/F_Student.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_F_Student_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/Elf/Elf.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Elf_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/Elf/Kitchen_Female/Kitchen_Female.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Kitchen_Female_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/BaiHowin/BaiHowin.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_BaiHowin_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/GeorgeOsric/GeorgeOsric.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_GeorgeOsric_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/DinahHecat/DinahHecat.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_DinahHecat_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/MudiwaOnai/MudiwaOnai.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_MudiwaOnai_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/GerboldOllivander/GerboldOlivander.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_GerboldOlivander_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Human/Npc/EleazarFig/Professor_EleazarFig.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Professor_EleazarFig_Model")
		));
	}
	if (true == isLoad_Monster) {
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/SK_GOB_M_T4Melee_INST_A_Master.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Goblin_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin_Mage/GoblinMage.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Goblin_Mage_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::ANIM, "../Bin/Resources/Models/Monster/GoblinSpector/GoblinSpector.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Goblin_Spector_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/Goblin_Assassin/SK_GOB_M_Assassin_Master.bin", XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Goblin_Assassin_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/SubTroll/troll.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Troll_Model")
		));
		jobCharacterModels.emplace_back(Deferred_ModelLoad(
			MODEL::PBR_ANIM, "../Bin/Resources/Models/Monster/ConjuredDragon/ConjuredDragon.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
			TEXT("Prototype_Component_Ranrok_Model")
		));
	}
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Camera/Camera.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Camera_Model")
	));
#pragma endregion

#pragma region ITEM

	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Wand/SK_Wands_Amit.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Wand_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Potion/SM_Bottle_EmptyCapOff.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_Potion_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/RaceRing/SM_BRR_RaceRing_01.bin", XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_RaceRing_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/Broom/Broom.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Broom_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/Broom/MoonTrimmerBroom/SK_MoonTrimmerBroom.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_SK_MoonTrimmerBroom_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/Broom/DarkWizardBroom/DarkWizardBroom.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_DarkWizardBroom_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/Broom/WildFireBroom/WildFireBroom.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_WildFireBroom_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/SubTroll_Weapon/SK_WPN_Troll_Club07.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Weapon_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/Object/Troll_Rock/VFX_SM_Rock_01.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Troll_Rock_Big_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_Dagger/SK_WPN_GOB_SmallSword.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_Dagger_Model")
	));

	//jobCharacterModels.emplace_back(Deferred_ModelLoad(
	//	MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_Sword/SK_WPN_GOB_SmallSword.bin", XMMatrixIdentity(),
	//	TEXT("Prototype_Component_Goblin_Sword_Model")
	//));

	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_Sword/SK_WPN_GOB_SmallSword_L.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_Sword_L_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_Sword/SK_WPN_GOB_SmallSword_R.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_Sword_R_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Goblin_BattleAxe/SK_WPN_GOB_BattleAxe01.bin", XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_Goblin_BattleAxe_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Reparo_Object/VFX_SK_OLI_TrollFight_BlockerA.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerA_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::ANIM, "../Bin/Resources/Models/Object/Reparo_Object/VFX_SK_OLI_TrollFight_BlockerB.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerB_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::PBR_ANIM, "../Bin/Resources/Models/Object/ThestralCarriage/ThestralCarriage.bin", XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixIdentity(),
		TEXT("Prototype_Component_ThestralCarriage_Model")
	));
	jobCharacterModels.emplace_back(Deferred_ModelLoad(
		MODEL::NONANIM, "../Bin/Resources/Models/SkyBox/SkyBox.bin", XMMatrixIdentity(),
		TEXT("Prototype_Component_SkyboxModel")
	));

#pragma endregion
	_bool bLoadShaderAsThread = true;
#ifdef 기무리
	bLoadShaderAsThread = false;
#endif
#ifndef 진우
#pragma region SHADER
	if (true == bLoadShaderAsThread)
	{
		{
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_UIEDITOR,
				TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_UIINSTANCE,
				TEXT("../Bin/Resources/ShaderFiles/Shader_UI_Instance.hlsl"), VTX_POSTEX_INSTANCE_UI::Elements, VTX_POSTEX_INSTANCE_UI::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_INSTANCE_MODEL,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_INSTANCE_PROP_MODEL,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxWorldModelInstance.hlsl"), VTX_MODEL_INSTANCE_MODEL::Elements, VTX_MODEL_INSTANCE_MODEL::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_NPC_PBR_ANIM,
				TEXT("../Bin/Resources/ShaderFiles/Shader_NPC_PBR_Anim.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_ANIMMESH,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_POSTEX,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_NORTEX,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_MESH,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements));
			jobMapShaders.emplace_back(Deferred_ShaderLoad(m_pDevice, m_pContext,
				FX_VTXPOS,
				TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPos.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements));
		}

		{
			m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");
			_uint iIndex = 0;
			for (auto& JobMapShader : jobMapShaders)
			{
				pair<_wstring, CShader*>* pOut = JobMapShader.get();
				if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pOut->first, pOut->second))) {
					return E_FAIL;
				}
				Safe_Delete(pOut);
			}
		}
	}
#pragma endregion
#endif

	/* LAND_MODEL */
	{
		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 
		{
			Desc.eType = ACTOR::TRIANGLEMESH;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
			Desc.fContactOffset = 0.001f;
		}
	
		/* For.Prototype_Component_South_Hogwart_Land_LOD1 */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_South_Hogwart_Land_LOD1"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BCLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_South_Hogwart_Land_LOD1_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_North_Hogwart_Land_LOD1 */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_North_Hogwart_Land_LOD1"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_AVLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_North_Hogwart_Land_LOD1_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_North_Hogwart2_Land_LOD1 */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_North_Hogwart2_Land_LOD1"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_AULOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_North_Hogwart2_Land_LOD1_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_West_Hogwart_Land_LOD1 */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_West_Hogwart_Land_LOD1"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_AZLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_West_Hogwart_Land_LOD1_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_East_Hogwart_Land_LOD1 */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_East_Hogsmeade_Land_LOD1"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_AYLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_East_Hogsmeade_Land_LOD1_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BA */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BA"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BALOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BA_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BD */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BD"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BDLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BD_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BE */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BE"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BELOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BE_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BF */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BF"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BFLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BF_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BG */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BG"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BGLOD/SM_LandscapeStreamingProxy_1_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BG_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BH */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BH"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BHLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BH_RigidBody"), pRigid))) {
			return E_FAIL;
		}
	
		/* For.Prototype_Component_Land_HN_BI */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BI"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BILOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BI_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_HN_BJ */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_HN_BJ"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/HN_BJLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_HN_BJ_RigidBody"), pRigid))) {
			return E_FAIL;
		}

		/* For.Prototype_Component_Land_TU_BB */
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_Land_TU_BB"),
			CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/Levels/Overland/HOG/TU_BBLOD/SM_LandscapeStreamingProxy_0_LOD1.bin"))))
			return E_FAIL;
		pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Land_TU_BB_RigidBody"), pRigid))) {
			return E_FAIL;
		}
	}
#pragma endregion
#pragma region UI

	Asset_FileLoad("../Bin/Resources/Textures/Keyboard", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Action", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Mission", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/SpellWidget", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Enemy", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/SpellUnlock", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Mission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/Mission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("ActiveMission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/ActiveMission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Megic_Metar"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Action/Megic_Meter%d.png"), 3)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Screen_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/LoadingScreen/UI_T_Loding_Screen_BG.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LoadingScreen"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/LoadingScreen/UI_T_LoadingScreen_%d.png"), 2)))) {
		return E_FAIL;
	}

	Asset_FileLoad("../Bin/Resources/Textures/MiniMap", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Loading", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Spells", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Spells/SpellMeters", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Potions", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/GadgetWheel", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/GadgetWheel", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Cursor", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Quest", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});


	Asset_FileLoad("../Bin/Resources/Textures/BroomFlight", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath)
		{

			_string strFilePath = pFilePath;
			_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
				return E_FAIL;
			}

			return S_OK;

		});



	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/GadgetWheel/Item%d.png"), 8)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Accio"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Accio\\Accio%d.png"), 151))))
	{
		return E_FAIL;
	}
#pragma region UI_ANI
	if (true == isLoad_UI_SEQUANTIAL) {


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Ancient_Magic"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Ancient_Magic\\Ancient_Magic%d.png"), 152))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Ancient_Magic_Throw"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Ancient_Magic_Throw\\Ancient_Magic_Throw%d.png"), 89))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Arresto_Momentum"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Arresto_Momentum\\Arresto_Momentum%d.png"), 120))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Avada_Kedavra"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Avada_Kedavra\\Avada_Kedavra%d.png"), 208))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Basic_Cast"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Basic_Cast\\Basic_Cast%d.png"), 308))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Bombarda"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Bombarda\\Bombarda%d.png"), 262))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Descendo"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Descendo\\Descendo%d.png"), 155))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Levioso"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Levioso\\Levioso%d.png"), 180))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Lumos"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Lumos\\Lumos%d.png"), 365))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Petrificus_Totalus"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Petrificus_Totalus\\Petrificus_Totalus%d.png"), 275))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Protego"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Protego\\Protego%d.png"), 285))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Revelio"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Revelio\\Revelio%d.png"), 227))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Stupefy"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Stupefy\\Stupefy%d.png"), 222))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Transformation"),
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("C:\\MeshTable\\SpellAnim\\Transformation\\Transformation%d.png"), 146))))
		{
			return E_FAIL;
		}

	}
#pragma endregion

#pragma endregion
#pragma region Global_SRV
	{
		m_pGameInstance->Load_GlobalSRV(TEXT("GLOBAL_DISOLVE_NOISE_05"), "../Bin/Resources/Textures/Effect/Noises/VFX_T_Noise05_D.png");
		m_pGameInstance->Load_GlobalSRV(TEXT("GLOBAL_DISOLVE_BURN_VERTICAL"), "../Bin/Resources/Textures/Effect/Diffuse/drp_fire_Line_Vertical.png");
	}
#pragma endregion
#pragma region TERRAIN_TEXTURE
	/* Terrain_Diffuse */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Terrain_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Terrain/Terrain_D_%d.dds"), 4)))) {
		return E_FAIL;
	}
	/* Terrain_Normal */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Terrain_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Terrain/Terrain_N_%d.dds"), 4)))) {
		return E_FAIL;
	}
	/* Terrain_MRO */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Terrain_MRO"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Terrain/Terrain_MRO_%d.dds"), 4)))) {
		return E_FAIL;
	}
	/* HogsmeadeAlphaMap */
	if(FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Hogsmeade_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Hogsmeade_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* HogwartAlphaMap */
	if(FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Hogwart_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Hogwart_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}

	/* Land_HN_AU_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_AU_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_AU_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_AV_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_AV_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_AV_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_AY_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_AY_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_AY_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_AZ_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_AZ_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_AZ_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BA_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BA_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BA_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BC_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BC_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BC_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BD_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BD_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BD_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BE_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BE_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BE_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BF_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BF_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BF_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BG_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BG_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BG_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BH_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BH_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BH_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BI_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BI_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BI_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_HN_BJ_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_HN_BJ_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_HN_BJ_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}
	/* Land_TU_BB_AlphaMap */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Land_TU_BB_AlphaMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Data/Map/Terrain/Land_TU_BB_AlphaMap.dds"), 0)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_LightPost_Emissive */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LightPost_Emissive"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("C:/MeshTable/Game/Environment/Hogsmeade/Common/Textures/LightPosts/T_HM_LampPost_Glass_E.png"), 0)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_LightPost_Mask */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LightPost_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("C:/MeshTable/Game/Environment/Hogsmeade/Common/Textures/LightPosts/T_HM_LampPost_Glass_D.png"), 0)))) {
		return E_FAIL;
	}

	/* For.Prototype_Component_LightPost_Base */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LightPost_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("C:/MeshTable/Game/Environment/Hogsmeade/Common/Textures/LightPosts/T_HM_LampPost_Glass_Dark_D.png"), 0)))) {
		return E_FAIL;
	}

	/* Balloon_Diffuse */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Balloon_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Balloon/T_FloatingBalloonTarget_D_%d.dds"), 4)))) {
		return E_FAIL;
	}

	/* Balloon_Normal */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Balloon_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Balloon/T_FloatingBalloonTarget_N.dds"), 0)))) {
		return E_FAIL;
	}

	/* Balloon_MRO */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Balloon_MRO"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Balloon/T_FloatingBalloonTarget_MRO.dds"), 0)))) {
		return E_FAIL;
	}

#pragma endregion
#pragma region LAKE_TEXTURE
	/* For.Prototype_Component_Water_Noise_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Noises_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Noises_D.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Water_Normal_Large_N */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Water_Normal_Large_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Water_Normal_Large_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Water_Normal_Subtle_N */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Water_Normal_Subtle_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Water_Normal_Subtle_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Caustics_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Caustics_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Caustics_D.dds"), 0)))) {
		return E_FAIL;
	}
	/* For.Prototype_Component_T_Water_N */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("T_Water_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE,
			TEXT("../Bin/Resources/Models/Lake/T_Water_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* Lake_Cube_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Lake_Cube_D"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Models/Lake/SkyCube.dds"), 0)))) {
		return E_FAIL;
	}

	/* Lake_Cube_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Lake_Refraction"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Models/Lake/T_MudSmallRocks_A_D.dds"), 0)))) {
		return E_FAIL;
	}
#pragma endregion

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Levioso_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Effect/Noises/VFX_T_NoiseCaustics02_Color_D.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Base_MRO"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/Environment//MasterMaterials/BaseTextures/T_Base011_MRO.dds"), 0)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Base_Effect_MRO"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/Environment//MasterMaterials/BaseTextures/T_Base011_MRO.dds"), 0)))) {
		return E_FAIL;
	}

#pragma region EFFECT

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Trails", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;


		});



	Asset_FileLoad("../Bin/Resources/Textures/Effect/Noises", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Diffuse", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Flipbooks", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Gradients", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Nomal", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

	});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Decal", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

	});


	Asset_FileLoad("../Bin/Resources/Textures/Effect/DecalNormal", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/RanrokEther", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

#pragma endregion
	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC SpellSlotUIDesc{};

	SpellSlotUIDesc.iNum = 4;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &SpellSlotUIDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC CurrentSpellSlotUIDesc{};

	CurrentSpellSlotUIDesc.iNum = 16;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Current_Spell_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &CurrentSpellSlotUIDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Spell_List{};

	Spell_List.iNum = 26;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_List"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Spell_List)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Eessential_Spell_Slot{};

	Eessential_Spell_Slot.iNum = 8;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Eessential_Spell_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Eessential_Spell_Slot)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Eessential_Spell{};

	Eessential_Spell.iNum = 8;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Eessential_Spell"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Eessential_Spell)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Spell_List_Image{};

	Spell_List_Image.iNum = 26;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_List_Image"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Spell_List_Image)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC SlotNumberUIDesc{};

	SlotNumberUIDesc.iNum = 4;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Slot_Number_UI_Instance"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &SlotNumberUIDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Magic_MeterDesc{};

	Magic_MeterDesc.iNum = 5;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Magic_Meter_UI_Instance"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Magic_MeterDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Spell_Slot_PreviewDesc{};

	Spell_Slot_PreviewDesc.iNum = 4;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Spell_Slot_Previe_UI_Instance"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Spell_Slot_PreviewDesc)))) {
		return E_FAIL;
	}

	CVIBuffer_UI_Instance::UI_INSTANCE_DESC Quest_Slot_Desc{};

	Quest_Slot_Desc.iNum = 10;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Quest_Slot"),
		CVIBuffer_UI_Instance::Create(m_pDevice, m_pContext, &Quest_Slot_Desc)))) {
		return E_FAIL;
	}
	{
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		Desc.eType = ACTOR::SPHERE;
		Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.001f };
		Desc.fContactOffset = { 0.005f };
		Desc.vhalfGeometryInfo = { 0.025f, 0.025f, 0.025f };
		Desc.fDensity = 0.01f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 12.f, 120.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_JOINT_ANCHOR"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		Desc.eType = ACTOR::SPHERE;
		Desc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.001f };
		Desc.fContactOffset = { 0.005f };
		Desc.vhalfGeometryInfo = { 0.75f, 0.25f, 0.25f };
		Desc.fDensity = 0.01f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 1200.f, 1200.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 1.0f, 0.f };
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_NPC_HITBOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		Desc.eType = ACTOR::CAPSULE;
		Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.001f };
		Desc.fContactOffset = { 0.005f };
		Desc.vhalfGeometryInfo = { 0.025f, 0.085f, 0.f };
		Desc.fDensity = 0.01f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 12.f, 120.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_JOINT_ROUTE"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		Desc.eType = ACTOR::CAPSULE;
		Desc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.5f, 0.5f, 0.01f };
		Desc.fContactOffset = { 0.01f };
		Desc.vhalfGeometryInfo = { 0.06f, 0.25f, 0.f };
		Desc.fDensity = 122.f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 1.f, 1.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_Player_Leg"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		Desc.eType = ACTOR::SPHERE;
		Desc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
		Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
		Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
		Desc.vMatInfo = { 0.01f, 0.01f, 0.01f };
		Desc.fContactOffset = { 0.01f };
		Desc.vhalfGeometryInfo = { 0.120f, 0.460f, 0.010f };
		Desc.fDensity = 100.f;
		Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
		Desc.eLockFlag = {};
		Desc.vAutoDamping = { 1.f, 1.f };
		Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
		Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_Player_LowerBound"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}
	{
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DynamicJoint"), CDynamic_D6Joint::Create(m_pDevice, m_pContext)))) {
			return E_FAIL;
		}
	}
	{ // LIGHT PHYSX DYNAMIC
		{
			CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 0.5f, 0.5f, 0.5f };
			Desc.fDensity = 1.f;
			Desc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc.eLockFlag = {};
			Desc.vAutoDamping = { 1.f, 1.f };
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.f, 0.f, 0.f };
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_THROWABLE_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
				return E_FAIL;
			}
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_HEAVY_WALL"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
				return E_FAIL;
			}
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
				return E_FAIL;
			}
		}

		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC ShieldDesc{};
		{
			ShieldDesc.eType = ACTOR::SPHERE;
			ShieldDesc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
			ShieldDesc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE };
			ShieldDesc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			ShieldDesc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			ShieldDesc.fContactOffset = { 0.05f };
			ShieldDesc.vhalfGeometryInfo = { 2.f, 2.f, 2.f };
			ShieldDesc.fDensity = 1.f;
			ShieldDesc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			ShieldDesc.eLockFlag = {};
			ShieldDesc.vAutoDamping = { 1.f, 1.f };
			ShieldDesc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			ShieldDesc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}

		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc1{};
		{
			Desc1.eType = ACTOR::SPHERE;
			Desc1.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
			Desc1.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc1.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc1.vMatInfo = { 0.5f, 0.5f, 0.6f };
			Desc1.fContactOffset = { 0.05f };
			Desc1.vhalfGeometryInfo = { 4.f, 4.f, 4.f };
			Desc1.fDensity = 1.f;
			Desc1.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			Desc1.eLockFlag = {};
			Desc1.vAutoDamping = { 1.f, 1.f };
			Desc1.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc1.vLocalTranslation = { 0.f, 0.f, 0.f };
		}

		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC DESC_Ranrok_Body{};
		{
			DESC_Ranrok_Body.eType = ACTOR::SPHERE;
			DESC_Ranrok_Body.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
			DESC_Ranrok_Body.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			DESC_Ranrok_Body.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			DESC_Ranrok_Body.vMatInfo = { 0.5f, 0.5f, 0.6f };
			DESC_Ranrok_Body.fContactOffset = { 0.05f };
			DESC_Ranrok_Body.vhalfGeometryInfo = { 1.1f, 1.1f, 1.1f };
			DESC_Ranrok_Body.fDensity = 1.f;
			DESC_Ranrok_Body.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			DESC_Ranrok_Body.eLockFlag = {};
			DESC_Ranrok_Body.vAutoDamping = { 1.f, 1.f };
			DESC_Ranrok_Body.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			DESC_Ranrok_Body.vLocalTranslation = { 0.f, 0.f, 0.f };
		}

		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC PotionDesc{};
		{
			PotionDesc.eType = ACTOR::SPHERE;
			PotionDesc.ePxRigidBodyFlags = { PSX::PxRigidBodyFlag::eKINEMATIC };
			PotionDesc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			PotionDesc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			PotionDesc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			PotionDesc.fContactOffset = { 0.05f };
			PotionDesc.vhalfGeometryInfo = { 0.07f, 0.07f, 0.07f };
			PotionDesc.fDensity = 1.f;
			PotionDesc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			PotionDesc.eLockFlag = {};
			PotionDesc.vAutoDamping = { 1.f, 1.f };
			PotionDesc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			PotionDesc.vLocalTranslation = { 0.f, 0.f, 0.f };
		}


		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC RanrokDesc{};
		{
			RanrokDesc.eType = ACTOR::BOX;
			RanrokDesc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			RanrokDesc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			RanrokDesc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			RanrokDesc.vMatInfo = { 0.5f, 0.5f, 0.6f };
			RanrokDesc.fContactOffset = { 0.05f };
			RanrokDesc.vhalfGeometryInfo = { 0.5f, 0.5f, 0.5f };
			RanrokDesc.fDensity = 1.f;
			RanrokDesc.pxMassCenter = PSX::PxTransform(PSX::PxIDENTITY());
			RanrokDesc.eLockFlag = {};
			RanrokDesc.vAutoDamping = { 1.f, 1.f };
			RanrokDesc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			RanrokDesc.vLocalTranslation = { 0.f, 5.f, 0.f };
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_SHIELD"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, ShieldDesc)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_RANROKPROP"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc1)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_RANROK_BODY"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, DESC_Ranrok_Body)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_POTION"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, PotionDesc)))) {
			return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_RANROK"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, RanrokDesc)))) {
			return E_FAIL;
		}
	}
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), CCharacter_Controller::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	{ // DOOR PHYSX
		CRigidBody_Dynamic::RIGIDBODY_PROTOTYPE_DYNAMIC_DESC Desc{};
		{
			Desc.eType = ACTOR::BOX;
			Desc.ePxRigidBodyFlags = { /*PSX::PxRigidBodyFlag::eKINEMATIC*/ };
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = { PXMATERIAL::DEFAULT };
			Desc.vMatInfo = { 0.5f, 0.5f, 0.1f };
			Desc.fContactOffset = { 0.05f };
			Desc.vhalfGeometryInfo = { 0.5f, 1.f, 0.01f };
			Desc.fDensity = 10.f;
			PSX::PxTransform pxPivotTransform = PSX::PxTransform(PSX::PxVec3(0.f, 1.f, 0.f));
			Desc.vLocalRotQ = { 0.f, 0.f, 0.f, 1.f };
			Desc.vLocalTranslation = { 0.5f, 1.375f, 0.f };

			Desc.pxMassCenter = pxPivotTransform;
			Desc.vAutoDamping = { 5.f, 5.f };
		}
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), CRigidBody_Dynamic::Create(m_pDevice, m_pContext, Desc)))) {
			return E_FAIL;
		}
	}

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Prototype<CTrail>(g_iStaticLevel, CTrail::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEffectParts>(g_iStaticLevel, CEffectParts::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTrailObject>(g_iStaticLevel, CTrailObject::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBombard>(g_iStaticLevel, CBombard::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDecendo>(g_iStaticLevel, CDecendo::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CNomalJap>(g_iStaticLevel, CNomalJap::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CDuelist_NormalJap>(g_iStaticLevel, CDuelist_NormalJap::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CProtego>(g_iStaticLevel, CProtego::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDuelist_Protego>(g_iStaticLevel, CDuelist_Protego::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRevelio>(g_iStaticLevel, CRevelio::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CNomalJapSide>(g_iStaticLevel, CNomalJapSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLevioso>(g_iStaticLevel, CLevioso::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDuelist_Levioso>(g_iStaticLevel, CDuelist_Levioso::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDuelist_JapSide>(g_iStaticLevel, CDuelist_JapSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLumos>(g_iStaticLevel, CLumos::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CAccio>(g_iStaticLevel, CAccio::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLeviosoSide>(g_iStaticLevel, CLeviosoSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBombardSide>(g_iStaticLevel, CBombardSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDecendoSide>(g_iStaticLevel, CDecendoSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CAccioSide>(g_iStaticLevel, CAccioSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CStupefySide>(g_iStaticLevel, CStupefySide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype<CTransformationSide>(g_iStaticLevel, CTransformationSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CAvadakedavraSide>(g_iStaticLevel, CAvadakedavraSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTrollSwing>(g_iStaticLevel, CTrollSwing::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Nomal_Smoke>(g_iStaticLevel, CTroll_Nomal_Smoke::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Rush_Hit>(g_iStaticLevel, CTroll_Rush_Hit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Rush>(g_iStaticLevel, CTroll_Rush::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Shout>(g_iStaticLevel, CTroll_Shout::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Self_Hit>(g_iStaticLevel, CTroll_Self_Hit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Protego>(g_iStaticLevel, CGoblin_Protego::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_ProtegoHit>(g_iStaticLevel, CGoblin_ProtegoHit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDuelist_ProtegoHit>(g_iStaticLevel, CDuelist_ProtegoHit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

		

	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Attack>(g_iStaticLevel, CGoblin_Attack::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMage_Down_Attack>(g_iStaticLevel, CMage_Down_Attack::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMage_Nomal_Attack>(g_iStaticLevel, CMage_Nomal_Attack::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMageSide>(g_iStaticLevel, CMageSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CStunEffect>(g_iStaticLevel, CStunEffect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBox_Splesh>(g_iStaticLevel, CBox_Splesh::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBarral_Splesh>(g_iStaticLevel, CBarral_Splesh::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CChair_Splesh>(g_iStaticLevel, CChair_Splesh::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CWandEnd>(g_iStaticLevel, CWandEnd::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CStupefy>(g_iStaticLevel, CStupefy::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CReparo>(g_iStaticLevel, CReparo::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLightning>(g_iStaticLevel, CLightning::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CProtego_Hit>(g_iStaticLevel, CProtego_Hit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLightningSide>(g_iStaticLevel, CLightningSide::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CBlink>(g_iStaticLevel, CBlink::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTransformation>(g_iStaticLevel, CTransformation::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CAvadakedavra>(g_iStaticLevel, CAvadakedavra::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_FireBall>(g_iStaticLevel, CRanrok_FireBall::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Breath>(g_iStaticLevel, CRanrok_Breath::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Point>(g_iStaticLevel, CRanrok_Point::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Pulse>(g_iStaticLevel, CRanrok_Pulse::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Charge>(g_iStaticLevel, CRanrok_Charge::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Hit>(g_iStaticLevel, CRanrok_Hit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Impact>(g_iStaticLevel, CRanrok_Impact::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Land>(g_iStaticLevel, CRanrok_Land::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_GroundPulse>(g_iStaticLevel, CRanrok_GroundPulse::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Rush_Bottom>(g_iStaticLevel, CRanrok_Rush_Bottom::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Swipe>(g_iStaticLevel, CRanrok_Swipe::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_DeadSplash>(g_iStaticLevel, CRanrok_DeadSplash::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_DeadImpact>(g_iStaticLevel, CRanrok_DeadImpact::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_Prop>(g_iStaticLevel, CRanrok_Prop::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Teleport>(g_iStaticLevel, CGoblin_Teleport::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CScreen_Wind>(g_iStaticLevel, CScreen_Wind::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CPotionBroken>(g_iStaticLevel, CPotionBroken::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CPotionScreen>(g_iStaticLevel, CPotionScreen::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CHitScreen>(g_iStaticLevel, CHitScreen::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype<CBroomRace_Bubble>(g_iStaticLevel, CBroomRace_Bubble::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok_PropHit>(g_iStaticLevel, CRanrok_PropHit::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	


	if (FAILED(m_pGameInstance->Add_Prototype<CEffectPool>(g_iStaticLevel, CEffectPool::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	

	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_WorldLook>(g_iStaticLevel, CCamPosition_WorldLook::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


	m_strMessage = TEXT("Model Loading..");


	Asset_FileLoad("../Bin/Resources/Models/Effect/Box", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/ParticleMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Goo", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/NomalMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Lightning", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;
		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Spline", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/DragonMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	Asset_FileLoad("../Bin/Resources/Models/Effect/Rock", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

		});

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

#pragma region UI 
	/* For.Prototype_GameObject_UI_Manager*/
	if (FAILED(m_pGameInstance->Add_Prototype<CUI_Manager>(g_iStaticLevel, CUI_Manager::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Mouse_Cursor*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMouse_Cursor>(g_iStaticLevel, CMouse_Cursor::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_CameraLockOn*/
	if (FAILED(m_pGameInstance->Add_Prototype<CCameraLockOn>(g_iStaticLevel, CCameraLockOn::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_QuestInstance*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuestInstance>(g_iStaticLevel, CQuestInstance::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_GamePlay_Canvas*/
	if (FAILED(m_pGameInstance->Add_Prototype<CGamePlay_Canvas>(g_iStaticLevel, CGamePlay_Canvas::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Action_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CAction_Panel>(g_iStaticLevel, CAction_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Slot>(g_iStaticLevel, CSpell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Overlay*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Overlay>(g_iStaticLevel, CSpell_Overlay::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Slot_Number*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSlot_Number>(g_iStaticLevel, CSlot_Number::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_HpBarBG*/
	if (FAILED(m_pGameInstance->Add_Prototype<CHpBarBG>(g_iStaticLevel, CHpBarBG::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Potion*/
	if (FAILED(m_pGameInstance->Add_Prototype<CPotion>(g_iStaticLevel, CPotion::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Magic_Meter*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Meter>(g_iStaticLevel, CMagic_Meter::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Magic_Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Icon>(g_iStaticLevel, CMagic_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_UI>(g_iStaticLevel, CSpell_UI::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Magic_Item*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMagic_Item>(g_iStaticLevel, CMagic_Item::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Loading_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CLoading_Panel>(g_iStaticLevel, CLoading_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_LoadingWidget_Flame*/
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget_Flame>(g_iStaticLevel, CLoadingWidget_Flame::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Mission_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Panel>(g_iStaticLevel, CMission_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Minimap_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_Panel>(g_iStaticLevel, CMiniMap_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_MiniMap_TrimBorder*/
	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_TrimBorder>(g_iStaticLevel, CMiniMap_TrimBorder::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_NoMountIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CNoMountIcon>(g_iStaticLevel, CNoMountIcon::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	
	/* For.Prototype_GameObject_NoMountIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_Panel>(g_iStaticLevel, CEnemy_Panel::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_NoMountIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_HpBar>(g_iStaticLevel, CEnemy_HpBar::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_NoMountIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_Info>(g_iStaticLevel, CEnemy_Info::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Enemy_SkillUI*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_SkillUI>(g_iStaticLevel, CEnemy_SkillUI::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_NoMountIcon*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBoss_HpBar>(g_iStaticLevel, CBoss_HpBar::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Enemy_Detection*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEnemy_Detection>(g_iStaticLevel, CEnemy_Detection::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Spell_Canvas*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Canvas>(g_iStaticLevel, CSpell_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Panel>(g_iStaticLevel, CSpell_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Current_Spell_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CCurrent_Spell_Slot>(g_iStaticLevel, CCurrent_Spell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_List*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_List>(g_iStaticLevel, CSpell_List::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Eessential_Spell_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEessential_Spell_Slot>(g_iStaticLevel, CEessential_Spell_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Eessential_Spell*/
	if (FAILED(m_pGameInstance->Add_Prototype<CEessential_Spell>(g_iStaticLevel, CEessential_Spell::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_List_Image*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_List_Image>(g_iStaticLevel, CSpell_List_Image::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_State*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_State>(g_iStaticLevel, CSpell_State::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Hover*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Hover>(g_iStaticLevel, CSpell_Hover::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Hover_Effect*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Hover_Effect>(g_iStaticLevel, CSpell_Hover_Effect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Preview*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Preview>(g_iStaticLevel, CSpell_Preview::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Header*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Header>(g_iStaticLevel, CSpell_Header::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Header_Line*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Header_Line>(g_iStaticLevel, CSpell_Header_Line::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Vidio_Border*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Vidio_Border>(g_iStaticLevel, CSpell_Vidio_Border::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Anim*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Anim>(g_iStaticLevel, CSpell_Anim::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Current_Slot_Number*/
	if (FAILED(m_pGameInstance->Add_Prototype<CCurrent_Slot_Number>(g_iStaticLevel, CCurrent_Slot_Number::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Spell_Drag*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpell_Drag>(g_iStaticLevel, CSpell_Drag::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Quest_Canvas*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Canvas>(g_iStaticLevel, CQuest_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Panel>(g_iStaticLevel, CQuest_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Info*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Info>(g_iStaticLevel, CQuest_Info::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Info_Header*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Info_Header>(g_iStaticLevel, CQuest_Info_Header::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Info_Line*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Info_Line>(g_iStaticLevel, CQuest_Info_Line::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Entry_Line*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Entry_Line>(g_iStaticLevel, CQuest_Entry_Line::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Status*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Status>(g_iStaticLevel, CQuest_Status::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Quest_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CQuest_Slot>(g_iStaticLevel, CQuest_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SpellLearn_Canvas*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_Canvas>(g_iStaticLevel, CSpellLearn_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_Panel>(g_iStaticLevel, CSpellLearn_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Name*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_Name>(g_iStaticLevel, CSpellLearn_Name::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn>(g_iStaticLevel, CSpellLearn::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_MovePointer*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_MovePointer>(g_iStaticLevel, CSpellLearn_MovePointer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_ChaserPointer*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_ChaserPointer>(g_iStaticLevel, CSpellLearn_ChaserPointer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_SpellLearn_LookPointer*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_LookPointer>(g_iStaticLevel, CSpellLearn_LookPointer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_SpellLearn_Booster*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_Booster>(g_iStaticLevel, CSpellLearn_Booster::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_SpellLearn_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_Slot>(g_iStaticLevel, CSpellLearn_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_SpellLearn_Overlay*/
	if (FAILED(m_pGameInstance->Add_Prototype<CSpellLearn_Overlay>(g_iStaticLevel, CSpellLearn_Overlay::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SpellLearn_Broom_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Panel>(g_iStaticLevel, CBroom_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Flag*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Flag>(g_iStaticLevel, CBroom_Flag::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Circle*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Circle>(g_iStaticLevel, CBroom_Circle::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Scoreboard*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Scoreboard>(g_iStaticLevel, CBroom_Scoreboard::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_TargetGate*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_TargetGate>(g_iStaticLevel, CBroom_TargetGate::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Fiish*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Finish>(g_iStaticLevel, CBroom_Finish::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Record*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Record>(g_iStaticLevel, CBroom_Record::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Exit*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Exit>(g_iStaticLevel, CBroom_Exit::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_Trophy*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_Trophy>(g_iStaticLevel, CBroom_Trophy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Broom_TargetGate2D*/
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom_TargetGate2D>(g_iStaticLevel, CBroom_TargetGate2D::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SpellLearn_Ride_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_Panel>(g_iStaticLevel, CRide_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_Info_Key*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_Info_Key>(g_iStaticLevel, CRide_Info_Key::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_Info*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_Info>(g_iStaticLevel, CRide_Info::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_InfoBG*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_InfoBG>(g_iStaticLevel, CRide_InfoBG::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_Bbooster_Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_Booster_Slot>(g_iStaticLevel, CRide_Booster_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_BboosterBar*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_BoosterBar>(g_iStaticLevel, CRide_BoosterBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_HpSlot*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_HpSlot>(g_iStaticLevel, CRide_HpSlot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Ride_HpBar*/
	if (FAILED(m_pGameInstance->Add_Prototype<CRide_HpBar>(g_iStaticLevel, CRide_HpBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_SpellLearn_Dialogue_Canvas*/
	if (FAILED(m_pGameInstance->Add_Prototype<CDialogue_Canvas>(g_iStaticLevel, CDialogue_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Dialogue_Panel*/
	if (FAILED(m_pGameInstance->Add_Prototype<CDialogue_Panel>(g_iStaticLevel, CDialogue_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_SpellLearn_Dialogue_Choice*/
	if (FAILED(m_pGameInstance->Add_Prototype<CDialogue_Choice>(g_iStaticLevel, CDialogue_Choice::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Interaction_Key*/
	if (FAILED(m_pGameInstance->Add_Prototype<CInteraction_Key>(g_iStaticLevel, CInteraction_Key::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_NPCInteraction*/
	if (FAILED(m_pGameInstance->Add_Prototype<CNPCInteraction>(g_iStaticLevel, CNPCInteraction::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Damage_Font*/
	if (FAILED(m_pGameInstance->Add_Prototype<CDamage_Font>(g_iStaticLevel, CDamage_Font::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Dialogue*/
	if (FAILED(m_pGameInstance->Add_Prototype<CDialogue>(g_iStaticLevel, CDialogue::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	/* For.Prototype_GameObject_Dialogue_Font*/
	if (FAILED(m_pGameInstance->Add_Prototype<CDialogue_Font>(g_iStaticLevel, CDialogue_Font::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion

	/* For.Prototype_GameObject_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype<CSkyBox>(g_iStaticLevel, CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region MAP PROTOTYPE
	/* For.Prototype_Component_VIBuffer_Terrain_Hogsmeade */
	{
		CVIBuffer_Terrain* pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "Hogsmeade_HeightMap.bin", 512, 512);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Hogsmeade"), pTerrain))) {
			return E_FAIL;
		}
		pTerrain->ConvertToHeightField(TEXT("Hogsmeade_HeightMap"));

		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 터레인 리지드 지형 추가
		{
			Desc.eType = ACTOR::HEIGHTFIELD;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.fContactOffset = 0.001f;
			Desc.vMatInfo = { 1.2f, 1.0f, 0.0f };
		}
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade"), pRigid))) {
			return E_FAIL;
		}
	}
	/* For.Prototype_Component_VIBuffer_Terrain_Hogwart */
	{
		CVIBuffer_Terrain* pTerrain = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, "Hogwart_HeightMap.bin", 512, 640);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain_Hogwart"), pTerrain))) {
			return E_FAIL;
		}
		pTerrain->ConvertToHeightField(TEXT("Hogwart_HeightMap"));

		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 터레인 리지드 지형 추가
		{
			Desc.eType = ACTOR::HEIGHTFIELD;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.fContactOffset = 0.001f;
			Desc.vMatInfo = { 1.2f, 1.0f, 0.0f };
		}
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogwart"), pRigid))) {
			return E_FAIL;
		}
	}

#pragma region MAP_INSTANCE

	_string strMaterailPath = "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml";

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA */
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OakTree_MedA.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_BearBerry_A.bin",
			"../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_OwlPost_Window_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleA_L_Rectangle_Double_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleC_Retangle_Double_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleA_L_Square_Double_C.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Quid_Window_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Ollivanders_Box_Window.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleC_L_Double_Single_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_WindowsStyleC_Round_Double_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_LightFixture_Base_D */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_LightFixture_Base_D"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HW_LightFixture_Base_D.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Door1a.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Door2b.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_TeaShop_Door_A*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_BLDG_TeaShop_Door_A.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_TeaShop_Door_A"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_OakTree_TallA*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OakTree_TallA.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_OakTree_TallA"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Shrub_B*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Shrub_B"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_GenericShrub_B.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_BogMyrtle_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Dogwood_B*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_Dogwood_B.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Dogwood_B"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_ScotsPine_LargeA*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_ScotsPine_LargeA_Master.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_ScotsPine_LargeA"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_StratifiedCliff_A1*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OL_StratifiedCliff_A1_Lod1.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_StratifiedCliff_A1"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_B*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OL_StratifiedRock_B.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_B"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_D_B*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_OL_StratifiedRock_D_B.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_D_B"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Stone_FrontSteps*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Stone_FrontSteps_A.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Stone_FrontSteps"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_StoneKit_A*/
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Quid_StoneKit_A.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_StoneKit_A"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_LightPost */
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_LightPost.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_LightPost"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Model_Instancel_Light_Post_Floating */
	{
		CVIBuffer_Model_Instance* pModel_Instance = CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_HM_Light_Post_Floating.bin", strMaterailPath.c_str());

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_Light_Post_Floating"),
			pModel_Instance)))
			return E_FAIL;

		if (FAILED(Ready_RigidBody_Static(pModel_Instance)))
			return E_FAIL;
	}


	/* For.Prototype_Component_VIBuffer_Model_Instancel_SK_BRR_RouteMarker */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_VIBuffer_Model_Instancel_SK_BRR_RouteMarker"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SK_BRR_RouteMarker.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_Component_FloatingBalloon */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_FloatingBalloon"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ENVIRONMENT, "../Bin/Resources/Models/MapMesh/Game/RiggedObjects/Props/FloatingBalloonTarget/SK_FloatingBalloonTarget_.bin"))))
		return E_FAIL;

	/* For.Prototype_Component_Hogwart_Lake */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Hogwart_Lake"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Lake/SM_Lake_A_Collision_Deep.bin", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_Hogwart_LakeSurFace */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Hogwart_LakeSurFace"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Lake/SM_Lake_A_Collision_Shallow.bin", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_Hogsmead_NorthLake */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Hogsmead_NorthLake"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/River/NorthLake/HDA_SM_LakeA_Collision_Deep.bin", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_Hogsmead_NorthLakeShallow */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, TEXT("Prototype_Component_Hogsmead_NorthLakeShallow"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/MapMesh/Game/Environment/Hogsmeade/River/NorthLake/HDA_SM_LakeA_Collision_Shallow.bin", XMMatrixIdentity()))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Box */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Box"),
		CVIBuffer_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_OcclusionQuery */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_OcclusionQuery"),
		COcclusionQuery::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CBuildingContainer */
	if (FAILED(m_pGameInstance->Add_Prototype<CBuildingContainer>(g_iStaticLevel, CBuildingContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CStreetContainer */
	if (FAILED(m_pGameInstance->Add_Prototype<CStreetContainer>(g_iStaticLevel, CStreetContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CMapObject_Render */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Render>(g_iStaticLevel, CMapObject_Render::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CMapObject_Collision */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Collision>(g_iStaticLevel, CMapObject_Collision::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CInstancedProp */
	if (FAILED(m_pGameInstance->Add_Prototype<CInstancedProp>(g_iStaticLevel, CInstancedProp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CInstancedProp_Light */
	if (FAILED(m_pGameInstance->Add_Prototype<CInstancedProp_Light>(g_iStaticLevel, CInstancedProp_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Light */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Light>(g_iStaticLevel, CMapElement_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PointLight */
	if (FAILED(m_pGameInstance->Add_Prototype<CPointLight>(NEXT_LEVEL, CPointLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Interactable */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Interactable>(g_iStaticLevel, CMapElement_Interactable::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Door */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Door>(g_iStaticLevel, CMapElement_Door::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Chest */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Chest>(g_iStaticLevel, CMapElement_Chest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Chest_Lid */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Chest_Lid>(g_iStaticLevel, CMapElement_Chest_Lid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Static */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Static>(g_iStaticLevel, CMapElement_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Unified */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Lake>(g_iStaticLevel, CMapElement_Lake::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Cave */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Cave>(g_iStaticLevel, CMapElement_Cave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Land */
	if (FAILED(m_pGameInstance->Add_Prototype<CLand>(g_iStaticLevel, CLand::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Unified */
	if (FAILED(m_pGameInstance->Add_Prototype<CUnified>(g_iStaticLevel, CUnified::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion // MAP PROTOTYPE

#pragma region ACTOR PROTOTYPE

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CPlayer>(g_iStaticLevel, CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Human_Duelist */
	if (FAILED(m_pGameInstance->Add_Prototype<CHuman_Duelist>(g_iStaticLevel, CHuman_Duelist::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BroomRacerAI */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroomRacerAI>(g_iStaticLevel, CBroomRacerAI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin>(g_iStaticLevel, CGoblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Mage */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Mage>(g_iStaticLevel, CGoblin_Mage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Assassin */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Assassin>(g_iStaticLevel, CGoblin_Assassin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Spector */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Spector>(g_iStaticLevel, CGoblin_Spector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Assassin_Spector */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Assassin_Spector>(g_iStaticLevel, CGoblin_Assassin_Spector::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll>(g_iStaticLevel, CTroll::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ollivander */
	if (FAILED(m_pGameInstance->Add_Prototype<CNPC_Ollivander>(g_iStaticLevel, CNPC_Ollivander::Create(m_pDevice, m_pContext)))){
		return E_FAIL;
	}

	/* For.Prototype_GameObject_EleazarFig */
	if (FAILED(m_pGameInstance->Add_Prototype<CNPC_EleazarFig>(g_iStaticLevel, CNPC_EleazarFig::Create(m_pDevice, m_pContext)))){
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Ranrok */
	if (FAILED(m_pGameInstance->Add_Prototype<CRanrok>(g_iStaticLevel, CRanrok::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_RandomNpc */
	if (FAILED(m_pGameInstance->Add_Prototype<CRandomNpc>(g_iStaticLevel, CRandomNpc::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Elf */
	if (FAILED(m_pGameInstance->Add_Prototype<CElf>(g_iStaticLevel, CElf::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
#pragma endregion


	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype<CFSM>(g_iStaticLevel, CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Gaze */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Gaze>(g_iStaticLevel, CCamera_Gaze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Cinematic */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamera_Cinematic>(g_iStaticLevel, CCamera_Cinematic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Player */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Socket>(g_iStaticLevel, CCamPosition_Socket::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_Shoulder */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_Shoulder>(g_iStaticLevel, CCamPosition_Shoulder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamPosition_AI */
	if (FAILED(m_pGameInstance->Add_Prototype<CCamPosition_AI>(g_iStaticLevel, CCamPosition_AI::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Potion */
	if (FAILED(m_pGameInstance->Add_Prototype<CItem_Potion>(g_iStaticLevel, CItem_Potion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RaceRing */
	if (FAILED(m_pGameInstance->Add_Prototype<CRaceRing>(g_iStaticLevel, CRaceRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapElement_Balloon */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapElement_Balloon>(g_iStaticLevel, CMapElement_Balloon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Troll_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Weapon>(g_iStaticLevel, CTroll_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Troll_Rock */
	if (FAILED(m_pGameInstance->Add_Prototype<CTroll_Rock>(g_iStaticLevel, CTroll_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Dagger */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Dagger>(g_iStaticLevel, CGoblin_Dagger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_Sword>(g_iStaticLevel, CGoblin_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Goblin_BattleAxe */
	if (FAILED(m_pGameInstance->Add_Prototype<CGoblin_BattleAxe>(g_iStaticLevel, CGoblin_BattleAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Broom */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroom>(g_iStaticLevel, CBroom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ReparoObject */
	if (FAILED(m_pGameInstance->Add_Prototype<CReparoObject>(g_iStaticLevel, CReparoObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ThestralCarriage */
	if (FAILED(m_pGameInstance->Add_Prototype<CThestralCarriage>(g_iStaticLevel, CThestralCarriage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BroomRaceManager */
	if (FAILED(m_pGameInstance->Add_Prototype<CBroomRaceManager>(g_iStaticLevel, CBroomRaceManager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype<CPhysXEffectHitBox>(g_iStaticLevel, CPhysXEffectHitBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}


#pragma region RECEIVE_THREAD
	{ // MapModels
		m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");
		_uint iIndex = 0;
		for (auto& JobMapModel : jobMapModels)
		{
			vector<FOLDER_LOAD*>* pOut = JobMapModel.get();
			for (_uint i = 0; i < pOut->size(); ++i) {
				FOLDER_LOAD* pContents = (*pOut)[i];
				if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, pContents->pModelTag, pContents->pLoadedModel))) {
					return E_FAIL;
				}

				for (_uint j = 0; j < pContents->pRigidBodyTags.size(); ++j) {
					if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, pContents->pRigidBodyTags[j], pContents->LoadedRigidBody[j]))) {
						return E_FAIL;
					}
				}
				Safe_Delete(pContents);
			}
			iIndex++;
			Safe_Delete(pOut);
		}
	}

	{
		for (auto& job : jobCharacterModels) {
			pair<_wstring, CModel*>* pResult = job.get();
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, pResult->first, pResult->second))) {
				assert(false);
				return E_FAIL;
			}
			Safe_Delete(pResult);
		}
	}


#pragma endregion

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("모델을 다시 불러오는 중입니다. ");

	if (isLoad_Monster) {
		{
			CModel* pModelOriginal = (CModel*)m_pGameInstance->Find_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Ranrok_Model"));
			CMotion_Trail::MODELCAPTURE_DESC Desc{};
			Desc.fMaxCaptureLifeTime = 0.3f;
			Desc.iMaximumCapture = 16;
			Desc.iNumBones = pModelOriginal->Get_BoneAbsoluteCount();
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Ranrok_MotionTrail"), CMotion_Trail::Create(m_pDevice, m_pContext, &Desc)))) {
				return E_FAIL;
			}
		}

		{
			CModel* pModelOriginal = (CModel*)m_pGameInstance->Find_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Assassin_Model"));
			CMotion_Trail::MODELCAPTURE_DESC Desc{};
			Desc.fMaxCaptureLifeTime = 0.7f;
			Desc.iMaximumCapture = 16;
			Desc.iNumBones = pModelOriginal->Get_BoneAbsoluteCount();
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Goblin_Assassin_MotionTrail"), CMotion_Trail::Create(m_pDevice, m_pContext, &Desc)))) {
				return E_FAIL;
			}
		}
	}

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Field()
{
	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

#pragma region DUNGEON

	vector<future<vector<FOLDER_LOAD*>*>> jobMapModels;
	{
		{/* Cave Wall */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Wall",
				".bin", false
			));
		}
		{/* Props */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Props",
				".bin", false
			));
		}
		{/* ArenaFloor */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/ArenaFloor",
				".bin", false
			));
		}
		{/*Sanctum_Dungeon_Center_Structure_AnteChamber */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/Center_Structure/AnteChamber",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_Center_Structure_Core */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/Center_Structure/Core",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_ConjuredDragonAttackZones */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/ConjuredDragonAttackZones",
				".bin", false
			));
		}

		{/* Sanctum_Dungeon_Rock_Barriers */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/Rock_Barriers",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_Rock_SM_Repository_FloatingGround */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/SM_Repository_FloatingGround",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_Rock_SM_Repository_Stage3MoveableRocks */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/Stage3MoveableRocks",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_Rock_Interactables */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository/BreakingRocks/LG_E",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_Rock_Main */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Meshes/Repository",
				".bin", false
			));
		}
		{/* Sanctum_Dungeon_Rock_Main */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Sanctum_Dungeon/Interactables",
				".bin", false
			));
		}
		{/* Cavern_Dungeon_Rocks */
			jobMapModels.emplace_back(Deferred_FolderLoad(
				"../Bin/Resources/Models/MapMesh/Game/Environment/Cavern_Dungeon/Mesh/Rocks",
				".bin", false
			));
		}
		
	}

#pragma endregion

	{ // MapModels
		_uint iIndex = 0;
		for (auto& JobMapModels : jobMapModels)
		{
			vector<FOLDER_LOAD*>* pOut = JobMapModels.get();
			for (_uint i = 0; i < pOut->size(); ++i) {
				FOLDER_LOAD* pContents = (*pOut)[i];
				if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::FIELD), pContents->pModelTag, pContents->pLoadedModel))) {
					return E_FAIL;
				}

				for (_uint j = 0; j < pContents->pRigidBodyTags.size(); ++j) {
					if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::FIELD), pContents->pRigidBodyTags[j], pContents->LoadedRigidBody[j]))) {
						return E_FAIL;
					}
				}
				Safe_Delete(pContents);
			}
			iIndex++;
			Safe_Delete(pOut);
		}
	}

	/* For.Prototype_Component_DecalBox */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_DecalBox"),
		CModel::Create(m_pDevice, m_pContext, MODEL::NONANIM, "../Bin/Resources/Models/Effect/Decal/DecalBox.bin", XMMatrixIdentity()))))
		return E_FAIL;

	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

#pragma region MAP_TEXTURE
	/* Base_MRO */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Base_MRO"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/Environment//MasterMaterials/BaseTextures/T_Base011_MRO.dds"), 0)))) {
		return E_FAIL;
	}
	/* Decal_MSK */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Decal_MSK"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/Environment/Sanctum_Dungeon/Textures/Repository/T_Rift_Material_MSK.dds"), 0)))) {
		return E_FAIL;
	}
	/* Decal_Noraml */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Decal_Noraml"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/Environment/Sanctum_Dungeon/Textures/Repository/T_Rift_Veins_N.dds"), 0)))) {
		return E_FAIL;
	}
	/* Decal_Fade */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Decal_Fade"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/VFX/Textures/Noises/VFX_T_Noise08_D.png"), 0)))) {
		return E_FAIL;
	}
	/* Decal_MaskNoise */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Decal_MaskNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/VFX/Textures/Noises/VFX_T_TankMoteNoiseVinceTest_D.png"), 0)))) {
		return E_FAIL;
	}
	/* Decal_MaskNoise */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Decal_DiffuseMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("C:/MeshTable/Game/VFX/Textures/VFX_T_DementorTatter_D.dds"), 0)))) {
		return E_FAIL;
	}
#pragma endregion


	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A*/
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::FIELD), TEXT("Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A"),
		CVIBuffer_Model_Instance::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Models/InstanceProp/SM_BogMyrtle_A.bin", "../Bin/Resources/Data/Map/Instance/InstanceMaterial.xml"))))
		return E_FAIL;

	/* For.Prototype_GameObject_PointLight */
	if (FAILED(m_pGameInstance->Add_Prototype<CPointLight>(ENUM_CLASS(LEVEL::FIELD), CPointLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WorldDecal */
	if (FAILED(m_pGameInstance->Add_Prototype<CWorldDecal>(ENUM_CLASS(LEVEL::FIELD), CWorldDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Restart()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Asset_FileLoad(const _char* pDirectoryPath, const _tchar* pPreName, function<HRESULT(_wstring, const _char*)> AddPrototypeEvent)
{
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (!strcmp(ext.c_str(), ".txt") || !strcmp(ext.c_str(), ".fbx")){
			continue;
		}

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		wstring wstrFileName = pPreName + file.path().stem().wstring();

		AddPrototypeEvent(wstrFileName, szFilePath);

	}

	return S_OK;
}


vector<FOLDER_LOAD*>* APIENTRY Deferred_FolderLoad_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag)
{
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return nullptr;
	}

	if (!filesystem::exists(pDirectoryPath)) {
		MSG_BOX(" 경로 이상해 ");
		return nullptr;
	}

	vector<FOLDER_LOAD*>* pOut = new vector<FOLDER_LOAD*>;
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory()) { continue; }
		if (strcmp(file.path().extension().string().c_str(), pFileExt)) { continue; }

		FOLDER_LOAD* pContents = new FOLDER_LOAD;

		{ // FOLDER_LOAD
			_char szFilePath[MAX_PATH] = {};
			strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

			_wstring wstrFileName = L"Prototype_GameObject_" + file.path().stem().wstring();

			CModel* pModel = CModel::Create(pDevice, pContext, MODEL::ENVIRONMENT, szFilePath);

			_uint iNumMesh = pModel->Get_NumMeshes();

			{
				pContents->pModelTag = wstrFileName;
				pContents->pathModel = file.path();

				pContents->pLoadedModel = pModel;

				pContents->pRigidBodyTags.reserve(iNumMesh);
				pContents->LoadedRigidBody.reserve(iNumMesh);
			}

			CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{};
			for (_uint i = 0; i < iNumMesh; ++i) {
				{
					Desc.eType = ACTOR::TRIANGLEMESH;
					Desc.ePxRigidBodyFlags = {};
					Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
					Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
					Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
					Desc.fContactOffset = 0.001f;
				}

				pContents->pRigidBodyTags.emplace_back(CMyTools::ToWstring(pModel->Get_MeshName(i) + to_string(i)).c_str());
				pContents->LoadedRigidBody.emplace_back(CRigidBody_Static::Create(pDevice, pContext, Desc));
			}
		}
		(*pOut).emplace_back(pContents);
	}
	(*pOut).shrink_to_fit();

	CoUninitialize();
	return pOut;
}

future<vector<FOLDER_LOAD*>*> CLoader::Deferred_FolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag)
{
	return m_pGameInstance->EnqueueJob(&Deferred_FolderLoad_Main, m_pDevice, m_pContext, pDirectoryPath, pFileExt, bUseTag);
}

pair<_wstring, CModel*>* APIENTRY Deferred_ModelLoad_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pDirectoryPath, const _tchar* pPrototypeTag, _fmatrix PreTransform)
{
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		assert(false);
		return nullptr;
	}

	CModel* pModel = CModel::Create(pDevice, pContext, eType, pDirectoryPath, PreTransform);

	CoUninitialize();
	pair<_wstring, CModel*>* Out = new pair<_wstring, CModel*>(pPrototypeTag, pModel);
	return Out;
}

pair<_wstring, CShader*>* APIENTRY Deferred_ShaderLoad_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	if (FAILED(CoInitializeEx(nullptr, 0))) {
		assert(false);
		return nullptr;
	}

	CShader* pShader = CShader::Create(pDevice, pContext, pShaderFilePath, pElements, iNumElements);

	CoUninitialize();
	pair<_wstring, CShader*>* Out = new pair<_wstring, CShader*>(strPrototypeTag, pShader);
	return Out;
}

future<pair<_wstring, CModel*>*> CLoader::Deferred_ModelLoad(MODEL eType, const _char* pDirectoryPath, _fmatrix PreTransform, const _tchar* pPrototypeTag) {
	return m_pGameInstance->EnqueueJob(&Deferred_ModelLoad_Main, m_pDevice, m_pContext, eType, pDirectoryPath, pPrototypeTag, PreTransform);
}

future<pair<_wstring, CShader*>*> CLoader::Deferred_ShaderLoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements) {
	return m_pGameInstance->EnqueueJob(&Deferred_ShaderLoad_Main, pDevice, pContext, strPrototypeTag, pShaderFilePath, pElements, iNumElements);
}

void CLoader::Ready_MapModels(vector<future<vector<FOLDER_LOAD*>*>>& jobMapModels)
{

	
}

HRESULT CLoader::Ready_RigidBody_Static(CVIBuffer_Model_Instance* pModel_Instance)
{
	_uint iNumMesh = pModel_Instance->Get_NumMesh();
	for (_uint i = 0; i < iNumMesh; ++i)
	{
		CRigidBody_Static::RIGIDBODY_STATIC_PROTOTYPEDESC Desc{}; // 
		{
			Desc.eType = ACTOR::TRIANGLEMESH;
			Desc.ePxRigidBodyFlags = {};
			Desc.ePxShapeFlags = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };
			Desc.ePxMaterialTypes = PXMATERIAL::DEFAULT;
			Desc.vMatInfo = _float3(0.5f, 0.5f, 0.6f);
			Desc.fContactOffset = 0.001f;
		}
		CRigidBody_Static* pRigid = CRigidBody_Static::Create(m_pDevice, m_pContext, Desc);
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(NEXT_LEVEL, CMyTools::ToWstring(pModel_Instance->Get_MeshName(i) + to_string(i)), pRigid))) {
			return E_FAIL;
		}
	}


	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
