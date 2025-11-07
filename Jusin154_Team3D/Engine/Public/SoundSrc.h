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
#pragma region KIND_A
	A_HIT00,
	A_HIT01,
#pragma endregion
#pragma region KIND_B
	B_HIT00,
	B_HIT01,
#pragma endregion
	END,
};

constexpr size_t SD_KIND_COUNT = static_cast<size_t>(SD_KIND::END);

struct ENGINE_DLL SD_PATH final {
	static const _tchar* const SD_KIND_PATHS[SD_KIND_COUNT];
};


NS_END // NS_SOUND
NS_END // NS_Engine