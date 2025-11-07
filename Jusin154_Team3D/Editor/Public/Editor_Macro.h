#pragma once
namespace Editor {

#ifndef __TEXT
#define __TEXT(quote) L##quote
#endif // !__TEXT
#ifndef TEXT
#define TEXT(quote) __TEXT(quote)
#endif // !TEXT


#pragma region FX_NAME
#define FX_ANIMMESH			TEXT("FX_ANIMMESH")
#define FX_MESH				TEXT("FX_MESH")
#define FX_POSTEX			TEXT("FX_POSTEX")
#define FX_NORTEX			TEXT("FX_NORTEX")
#define FX_CELL				TEXT("FX_CELL")
#define FX_DEFERRED			TEXT("FX_DEFFERED")
#define FX_PARTICLE			TEXT("FX_PARTICLE")

#pragma endregion
#pragma region MACRO_PROTOTYPE
#define CURRENT_LEVEL	m_pGameInstance->Get_CurrentLevelID()
#define NEXT_LEVEL		m_pGameInstance->Get_NextLevelID()

#pragma endregion
#pragma region OBJECT

#pragma endregion
#pragma region COMPONENT

#pragma endregion
#pragma region MACRO_LAYER

#define LAYER_FLOOR			TEXT("Layer_Floor")
#define LAYER_WALL			TEXT("Layer_Wall")
#define LAYER_CUBE			TEXT("Layer_Cube")
#define LAYER_EDITOR_CAMERA TEXT("Layer_Editor_Camera")
#define LAYER_BACKGROUND	TEXT("Layer_BackGround")
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

#pragma endregion
#pragma region MACRO_TEXTURE

#pragma endregion
#pragma region MACRO_EXTRA


#pragma endregion
}