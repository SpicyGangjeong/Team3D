#include "pch.h"
#include "Engine_Define.h"
#include "SoundSrc.h"


using namespace Engine::SOUND;

const _tchar* const SD_PATH::SD_KIND_PATHS[SD_KIND_COUNT] = {
#pragma region KIND_A
	TEXT("A/HIT00.ogg"),
	TEXT("A/HIT01.ogg"),
#pragma endregion
#pragma region KIND_B
	TEXT("B/HIT00.ogg"),
	TEXT("B/HIT01.ogg"),
#pragma endregion
};

static_assert(SD_KIND_COUNT == _countof(SD_PATH::SD_KIND_PATHS), 
	"SD_KIND and SD_PATH::SD_KIND_PATHS num is not match" "match same num");