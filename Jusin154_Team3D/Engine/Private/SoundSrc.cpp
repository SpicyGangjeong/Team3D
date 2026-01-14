#include "pch.h"
#include "Engine_Define.h"
#include "SoundSrc.h"


using namespace Engine::SOUND;

const _tchar* const SD_PATH::SD_KIND_PATHS[SD_KIND_COUNT] = {
#pragma region BGM
	TEXT("BGM/Hogwart_BGM_0.wav"),
	TEXT("BGM/Ranrok_BGM.wav"),
#pragma endregion

#pragma region SPELL
	TEXT("Player/spells_jap_akb/jap_0.wav"),
	TEXT("Player/spells_jap_akb/jap_1.wav"),
	TEXT("Player/spells_jap_akb/jap_13.wav"),
	TEXT("Player/spells_perk_shadowblink_akb/blink_2.wav"),
#pragma endregion

#pragma region VOICE
	TEXT("Player/Voice/avadakedavra_voice.wav"),
	TEXT("Player/Voice/bombarda_voice.wav"),
	TEXT("Player/Voice/decendo_voice.wav"),
	TEXT("Player/Voice/levioso_voice.wav"),
	TEXT("Player/Voice/lumos_voice.wav"),
	TEXT("Player/Voice/protego_voice.wav"),
	TEXT("Player/Voice/reparo_voice.wav"),
	TEXT("Player/Voice/revelio.voice.wav"),
	TEXT("Player/Voice/stupefy_voice.wav"),
#pragma endregion

};

//static_assert(SD_KIND_COUNT == _countof(SD_PATH::SD_KIND_PATHS), 
//	"SD_KIND and SD_PATH::SD_KIND_PATHS num is not match" "match same num");
