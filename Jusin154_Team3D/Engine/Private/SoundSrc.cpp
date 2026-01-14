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

	TEXT("Player/spells_accio_akb/accio_0.wav"),
	TEXT("Player/spells_accio_akb/accio_1.wav"),
	TEXT("Player/spells_accio_akb/accio_2.wav"),
	TEXT("Player/spells_accio_akb/accio_3.wav"),
	TEXT("Player/spells_accio_akb/accio_4.wav"),
	TEXT("Player/spells_accio_akb/accio_5.wav"),
	TEXT("Player/spells_accio_akb/accio_6.wav"),
	TEXT("Player/spells_accio_akb/accio_7.wav"),
	TEXT("Player/spells_accio_akb/accio_8.wav"),
	TEXT("Player/spells_accio_akb/accio_9.wav"),
	TEXT("Player/spells_accio_akb/accio_10.wav"),
	TEXT("Player/spells_accio_akb/accio_11.wav"),
	TEXT("Player/spells_accio_akb/accio_12.wav"),
	TEXT("Player/spells_accio_akb/accio_13.wav"),
	TEXT("Player/spells_accio_akb/accio_14.wav"),
	TEXT("Player/spells_accio_akb/accio_15.wav"),
	TEXT("Player/spells_accio_akb/accio_16.wav"),
	TEXT("Player/spells_accio_akb/accio_17.wav"),
	TEXT("Player/spells_accio_akb/accio_18.wav"),
	TEXT("Player/spells_accio_akb/accio_19.wav"),
	TEXT("Player/spells_accio_akb/accio_20.wav"),
	TEXT("Player/spells_accio_akb/accio_21.wav"),
	TEXT("Player/spells_accio_akb/accio_22.wav"),
	TEXT("Player/spells_accio_akb/accio_23.wav"),

#pragma endregion

#pragma region REPARO
	TEXT("Player/spells_reparo_akb/reparo_0.wav"),
	TEXT("Player/spells_reparo_akb/reparo_11.wav"),
	TEXT("Player/spells_reparo_akb/reparo_21.wav"),
#pragma endregion

#pragma region DECENDO

	TEXT("Player/spells_descendo_akb/descendo_0.wav"),
	TEXT("Player/spells_descendo_akb/descendo_1.wav"),
	TEXT("Player/spells_descendo_akb/descendo_2.wav"),
	TEXT("Player/spells_descendo_akb/descendo_3.wav"),
	TEXT("Player/spells_descendo_akb/descendo_4.wav"),
	TEXT("Player/spells_descendo_akb/descendo_5.wav"),
	TEXT("Player/spells_descendo_akb/descendo_6.wav"),
	TEXT("Player/spells_descendo_akb/descendo_7.wav"),
	TEXT("Player/spells_descendo_akb/descendo_8.wav"),
	TEXT("Player/spells_descendo_akb/descendo_9.wav"),
	TEXT("Player/spells_descendo_akb/descendo_10.wav"),
	TEXT("Player/spells_descendo_akb/descendo_11.wav"),
	TEXT("Player/spells_descendo_akb/descendo_12.wav"),
	TEXT("Player/spells_descendo_akb/descendo_13.wav"),
	TEXT("Player/spells_descendo_akb/descendo_14.wav"),
	TEXT("Player/spells_descendo_akb/descendo_15.wav"),
	TEXT("Player/spells_descendo_akb/descendo_16.wav"),
	TEXT("Player/spells_descendo_akb/descendo_17.wav"),
	TEXT("Player/spells_descendo_akb/descendo_18.wav"),
	TEXT("Player/spells_descendo_akb/descendo_19.wav"),
	TEXT("Player/spells_descendo_akb/descendo_20.wav"),
	TEXT("Player/spells_descendo_akb/descendo_21.wav"),
	TEXT("Player/spells_descendo_akb/descendo_22.wav"),
	TEXT("Player/spells_descendo_akb/descendo_23.wav"),
	TEXT("Player/spells_descendo_akb/descendo_24.wav"),
	TEXT("Player/spells_descendo_akb/descendo_25.wav"),

#pragma endregion


#pragma region BOMBARD

	TEXT("Player/spells_bombard_akb/bombard_0.wav"),
	TEXT("Player/spells_bombard_akb/bombard_1.wav"),
	TEXT("Player/spells_bombard_akb/bombard_2.wav"),
	TEXT("Player/spells_bombard_akb/bombard_3.wav"),
	TEXT("Player/spells_bombard_akb/bombard_4.wav"),
	TEXT("Player/spells_bombard_akb/bombard_5.wav"),
	TEXT("Player/spells_bombard_akb/bombard_6.wav"),
	TEXT("Player/spells_bombard_akb/bombard_7.wav"),
	TEXT("Player/spells_bombard_akb/bombard_8.wav"),
	TEXT("Player/spells_bombard_akb/bombard_9.wav"),
	TEXT("Player/spells_bombard_akb/bombard_10.wav"),
	TEXT("Player/spells_bombard_akb/bombard_11.wav"),
	TEXT("Player/spells_bombard_akb/bombard_12.wav"),
	TEXT("Player/spells_bombard_akb/bombard_13.wav"),
	TEXT("Player/spells_bombard_akb/bombard_14.wav"),
	TEXT("Player/spells_bombard_akb/bombard_15.wav"),
	TEXT("Player/spells_bombard_akb/bombard_16.wav"),
	TEXT("Player/spells_bombard_akb/bombard_17.wav"),
	TEXT("Player/spells_bombard_akb/bombard_18.wav"),
	TEXT("Player/spells_bombard_akb/bombard_19.wav"),
	TEXT("Player/spells_bombard_akb/bombard_20.wav"),
	TEXT("Player/spells_bombard_akb/bombard_21.wav"),
	TEXT("Player/spells_bombard_akb/bombard_22.wav"),
	TEXT("Player/spells_bombard_akb/bombard_23.wav"),
	TEXT("Player/spells_bombard_akb/bombard_24.wav"),
	TEXT("Player/spells_bombard_akb/bombard_25.wav"),
	TEXT("Player/spells_bombard_akb/bombard_26.wav"),
	TEXT("Player/spells_bombard_akb/bombard_27.wav"),
	TEXT("Player/spells_bombard_akb/bombard_28.wav"),
	TEXT("Player/spells_bombard_akb/bombard_29.wav"),
	TEXT("Player/spells_bombard_akb/bombard_30.wav"),
	TEXT("Player/spells_bombard_akb/bombard_31.wav"),
	TEXT("Player/spells_bombard_akb/bombard_32.wav"),
	TEXT("Player/spells_bombard_akb/bombard_33.wav"),
	TEXT("Player/spells_bombard_akb/bombard_34.wav"),
	TEXT("Player/spells_bombard_akb/bombard_35.wav"),
	TEXT("Player/spells_bombard_akb/bombard_36.wav"),
	TEXT("Player/spells_bombard_akb/bombard_37.wav"),
	TEXT("Player/spells_bombard_akb/bombard_38.wav"),
	TEXT("Player/spells_bombard_akb/bombard_39.wav"),
	TEXT("Player/spells_bombard_akb/bombard_40.wav"),
	TEXT("Player/spells_bombard_akb/bombard_41.wav"),
	TEXT("Player/spells_bombard_akb/bombard_42.wav"),
	TEXT("Player/spells_bombard_akb/bombard_43.wav"),
	TEXT("Player/spells_bombard_akb/bombard_44.wav"),
	TEXT("Player/spells_bombard_akb/bombard_45.wav"),
	TEXT("Player/spells_bombard_akb/bombard_46.wav"),
#pragma endregion

#pragma region LEVIOSA
		TEXT("Player/spells_leviosa_akb/leviosa_0.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_1.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_2.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_3.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_4.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_5.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_6.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_7.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_8.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_9.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_10.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_11.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_12.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_13.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_14.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_15.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_16.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_17.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_18.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_19.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_20.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_21.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_22.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_23.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_24.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_25.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_26.wav"),
		TEXT("Player/spells_leviosa_akb/leviosa_27.wav"),

#pragma endregion

#pragma region PROTEGO

		TEXT("Player/spells_protego_akb/protego_0.wav"),
		TEXT("Player/spells_protego_akb/protego_1.wav"),
		TEXT("Player/spells_protego_akb/protego_2.wav"),
		TEXT("Player/spells_protego_akb/protego_3.wav"),
		TEXT("Player/spells_protego_akb/protego_4.wav"),
		TEXT("Player/spells_protego_akb/protego_5.wav"),
		TEXT("Player/spells_protego_akb/protego_6.wav"),
		TEXT("Player/spells_protego_akb/protego_7.wav"),
		TEXT("Player/spells_protego_akb/protego_8.wav"),
		TEXT("Player/spells_protego_akb/protego_9.wav"),
		TEXT("Player/spells_protego_akb/protego_10.wav"),
		TEXT("Player/spells_protego_akb/protego_11.wav"),
		TEXT("Player/spells_protego_akb/protego_12.wav"),
		TEXT("Player/spells_protego_akb/protego_13.wav"),
		TEXT("Player/spells_protego_akb/protego_14.wav"),
		TEXT("Player/spells_protego_akb/protego_15.wav"),
		TEXT("Player/spells_protego_akb/protego_16.wav"),
		TEXT("Player/spells_protego_akb/protego_17.wav"),
		TEXT("Player/spells_protego_akb/protego_18.wav"),
		TEXT("Player/spells_protego_akb/protego_19.wav"),
		TEXT("Player/spells_protego_akb/protego_20.wav"),
		TEXT("Player/spells_protego_akb/protego_21.wav"),
		TEXT("Player/spells_protego_akb/protego_22.wav"),
		TEXT("Player/spells_protego_akb/protego_23.wav"),
		TEXT("Player/spells_protego_akb/protego_24.wav"),
		TEXT("Player/spells_protego_akb/protego_25.wav"),
		TEXT("Player/spells_protego_akb/protego_26.wav"),
		TEXT("Player/spells_protego_akb/protego_27.wav"),
		TEXT("Player/spells_protego_akb/protego_28.wav"),
		TEXT("Player/spells_protego_akb/protego_29.wav"),
		TEXT("Player/spells_protego_akb/protego_30.wav"),

#pragma endregion




#pragma region LUMOS
		TEXT("Player/spells_lumos_akb/lumos_0.wav"),
		TEXT("Player/spells_lumos_akb/lumos_1.wav"),
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

#pragma region BROOM
	TEXT("Player/Broom/broom_boost.wav"),
	TEXT("Player/Broom/broom_down.wav"),
	TEXT("Player/Broom/broom_get_item.wav"),
	TEXT("Player/Broom/broom_normal.wav"),
	TEXT("Player/Broom/broom_ride.wav"),
#pragma endregion

#pragma region DOOR
	TEXT("Interactive/Door/door_open.wav"),
	TEXT("Interactive/Door/door_close.wav"),
#pragma endregion

#pragma region CHEST
	TEXT("Interactive/Chest/chest_found.wav"),
	TEXT("Interactive/Chest/chest_open.wav"),
#pragma endregion
};

static_assert(SD_KIND_COUNT == _countof(SD_PATH::SD_KIND_PATHS), 
	"SD_KIND and SD_PATH::SD_KIND_PATHS num is not match" "match same num");
