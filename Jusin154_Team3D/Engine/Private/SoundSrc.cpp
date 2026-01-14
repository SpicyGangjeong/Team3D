#include "pch.h"
#include "Engine_Define.h"
#include "SoundSrc.h"


using namespace Engine::SOUND;

const _tchar* const SD_PATH::SD_KIND_PATHS[SD_KIND_COUNT] = {
#pragma region BGM
	TEXT("BGM/battle_BGM_0.wav"),
	TEXT("BGM/battle_BGM_1.wav"),
	TEXT("BGM/grassland_day.wav"),
	TEXT("BGM/grassland_night.wav"),
	TEXT("BGM/hogwart_BGM_0.wav"),
	TEXT("BGM/hogwart_BGM_1.wav"),
	TEXT("BGM/hogwart_BGM_int.wav"),
	TEXT("BGM/Ranrok_BGM.wav"),
#pragma endregion

#pragma region SPELL
	TEXT("Player/spells_jap_akb/jap_0.wav"),
	TEXT("Player/spells_jap_akb/jap_1.wav"),
	TEXT("Player/spells_jap_akb/jap_13.wav"),
	TEXT("Player/spells_perk_shadowblink_akb/blink_2.wav"),
#pragma endregion

#pragma region AVADA_KEDAVRA
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_0.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_1.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_2.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_3.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_4.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_5.wav"), // 5
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_6.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_7.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_8.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_9.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_10.wav"), // 10
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_11.wav"),
	TEXT("Player/spells_avada_kedavra_akb/avada_kedavra_12.wav"),
#pragma endregion

#pragma region ANCIENT_LIGHTNING
	TEXT("Player/spells_finisher_am_lightningstrike_akb/lightnig_0.wav"),
	TEXT("Player/spells_finisher_am_lightningstrike_akb/lightnig_1.wav"),
	TEXT("Player/spells_finisher_am_lightningstrike_akb/lightnig_2.wav"),
	TEXT("Player/spells_finisher_am_lightningstrike_akb/lightnig_3.wav"),
#pragma endregion

#pragma region ACCIO
	TEXT("Player/spells_accio_akb/accio_3.wav"),
	TEXT("Player/spells_accio_akb/accio_6.wav"),
	TEXT("Player/spells_accio_akb/accio_7.wav"),
	TEXT("Player/spells_accio_akb/accio_11.wav"),
	TEXT("Player/spells_accio_akb/accio_12.wav"),
	TEXT("Player/spells_accio_akb/accio_15.wav"),
	TEXT("Player/spells_accio_akb/accio_21.wav"),
#pragma endregion

#pragma region REPARO
	TEXT("Player/spells_reparo_akb/reparo_0.wav"),
	TEXT("Player/spells_reparo_akb/reparo_11.wav"),
	TEXT("Player/spells_reparo_akb/reparo_21.wav"),
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
	TEXT("Player/Voice/stupefy_voice.wav")
#pragma endregion

};

//static_assert(SD_KIND_COUNT == _countof(SD_PATH::SD_KIND_PATHS), 
//	"SD_KIND and SD_PATH::SD_KIND_PATHS num is not match" "match same num");
