#pragma once
namespace Client {

#ifndef __TEXT
#define __TEXT(quote) L##quote
#endif // !__TEXT
#ifndef TEXT
#define TEXT(quote) __TEXT(quote)
#endif // !TEXT


#pragma region FX_NAME
#define FX_ANIMMESH				TEXT("FX_ANIMMESH")
#define FX_MESH					TEXT("FX_MESH")
#define FX_POSTEX				TEXT("FX_POSTEX")
#define FX_NORTEX				TEXT("FX_NORTEX")
#define FX_CELL					TEXT("FX_CELL")
#define FX_INSTANCE_MODEL		TEXT("FX_INSTANCE_MODEL")
#define FX_INSTANCE_PROP_MODEL  TEXT("FX_INSTANCE_PROP_MODEL")
#define FX_UIEDITOR				TEXT("FX_UIEDITOR")
#define FX_UIINSTANCE			TEXT("FX_UIINSTANCE")
#define FX_VTXPOS				TEXT("FX_VTXPOS")
#define FX_NPC_PBR_ANIM			TEXT("FX_NPC_PBR_ANIM")

#pragma endregion
#pragma region MACRO_PROTOTYPE
#define CURRENT_LEVEL	m_pGameInstance->Get_CurrentLevelID()
#define NEXT_LEVEL		m_pGameInstance->Get_NextLevelID()

#pragma endregion
#pragma region OBJECT

#define CAMERA_DEBUG			TEXT("CAMERA_DEBUG")
#define CAMERA_SHOULDER			TEXT("CAMERA_SHOULDER")
#define CAMERA_AI				TEXT("CAMERA_AI")
#define CAMERA_MODEL			TEXT("CAMERA_MODEL")
#pragma endregion
#pragma region COMPONENT

#pragma endregion
#pragma region MACRO_LAYER

#define LAYER_FLOOR			TEXT("Layer_Floor")
#define LAYER_WALL			TEXT("Layer_Wall")
#define LAYER_CUBE			TEXT("Layer_Cube")
#define LAYER_SKYBOX		TEXT("Layer_SkyBox")
#define LAYER_EDITOR_CAMERA TEXT("Layer_Editor_Camera")
#define LAYER_BACKGROUND	TEXT("Layer_BackGround")
#define LAYER_HOGSMEADE		TEXT("Layer_Hogsmeade")
#define LAYER_HOGWART		TEXT("Layer_Hogwart")
#define LAYER_MARKER		TEXT("Layer_Marker")
#define LAYER_PLAYER		TEXT("Layer_Player")
#define LAYER_CAMERA		TEXT("Layer_Camera")
#define LAYER_MONSTER		TEXT("Layer_Monster")
#define LAYER_UI			TEXT("Layer_UI")
#define LAYER_EFFECT		TEXT("Layer_Effect")
#define LAYER_HITBOX		TEXT("Layer_HitBox")
#define LAYER_CHUNK			TEXT("Layer_Chunk")
#define LAYER_DOT			TEXT("Layer_Dot")
#define LAYER_CELL			TEXT("Layer_Cell")
#define LAYER_TRIGGER		TEXT("Layer_Trigger")
#define LAYER_ITEM			TEXT("Layer_Item")
#define LAYER_EFFECTPOOL	TEXT("Layer_EffectPool")
#define LAYER_LIGHT			TEXT("Layer_Light")
#define LAYER_INTERACTABLE	TEXT("Layer_Interactable")
#define LAYER_UNIFIED		TEXT("Layer_Unified")
#define LAYER_DOOR			TEXT("Layer_Door")
#define LAYER_HIDDEN		TEXT("Layer_HIDDEN")
#define LAYER_RING			TEXT("Layer_Ring")
#define LAYER_RACERAI		TEXT("Layer_RacerAI")


#define LAYER_MAPOBJECTMANAGER		TEXT("Layer_MapObjectManager")

#pragma endregion
#pragma region MACRO_TEXTURE

#pragma endregion
#pragma region MACRO_EXTRA

#pragma endregion
}
