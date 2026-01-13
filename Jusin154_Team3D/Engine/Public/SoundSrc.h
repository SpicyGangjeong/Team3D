#pragma once

NS_BEGIN(Engine)
NS_BEGIN(SOUND)


enum class SD_CHANNEL_GROUP {
	BGM,
	ENVIRONMENT,
	EFFECT,
	VOICE,
	END
};

enum class SD_KIND {
#pragma region BGM
	BGM_0,
	BGM_1,
#pragma endregion
#pragma region SPELL
	SP_JAP_0,
	SP_JAP_1,
	SP_JAP_13,
	SP_BLINK_2,
#pragma endregion
	END,
};

constexpr size_t SD_KIND_COUNT = static_cast<size_t>(SD_KIND::END);

struct ENGINE_DLL SD_PATH final {
	static const _tchar* const SD_KIND_PATHS[SD_KIND_COUNT];
};


NS_END // NS_SOUND
NS_END // NS_Engine
