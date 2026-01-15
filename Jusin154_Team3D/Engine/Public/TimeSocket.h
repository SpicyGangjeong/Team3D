#pragma once
#include "GameInstance.h"

NS_BEGIN(Engine)
class CGameObject;
class CTimeSocket;

typedef enum class typeTimeSocket_Param {
	NOT_USE,
	TARGET,
	TRANSFORM,
	END
}TIMESOCKET_PARAM;

static constexpr pair<string_view, TIMESOCKET_PARAM> g_SocketParamTypeTable[ENUM_CLASS(TIMESOCKET_PARAM::END) + 1] =
{
	{"NOT_USE",		TIMESOCKET_PARAM::NOT_USE},
	{"TARGET",		TIMESOCKET_PARAM::TARGET},
	{"TRANSFORM",	TIMESOCKET_PARAM::TRANSFORM},
	{"END",			TIMESOCKET_PARAM::END},
}; 

typedef enum class typeTimeSocket_Func {
	// COMMON
	TELEPORTATION,
	TRANSLATION,
	TRANSLATION_LERP,
	ROTATION,
	ROTATION_LERP,
	AFFINE_LERP,
	TOGGLE_FLAGS,
	SET_WORLD_LOCKPOSITION,

	// CAMERA
	SETTING_CAMERA,
	BIND_CAMERA,
	ZOOM_IN,
	ZOOM_OUT,
	ZOOM_LERP,
	FADE_IN,
	FADE_OUT,
	END_CINEMATIC,
	SET_PITCHLIMIT,
	LOOK_AT,
	DONT_LOOK_AT,
	FOLLOW,
	DONT_FOLLOW,
	SHAKE,

	// CHARACTER
	SET_ANIMSTATE,
	SET_ANIMTRACKPOSITION,
	SET_FSMSTATE,
	BIND_SOCKET_MATRIX,
	UNBIND_SOCKET_MATRIX,

	// EFFECT

	// SOUND

	// TRIGGER
	SET_CAMERA_ENVIRONMENT,

	//SYSTEM
	STOP_CINEMATIC,
	RESET_ENVIRONMENT,
	SET_VOLUMETRIC,

	END
}TIMESOCKET_FUNC;

static constexpr pair<string_view, TIMESOCKET_FUNC> g_SocketFuncTypeTable[ENUM_CLASS(TIMESOCKET_FUNC::END) + 1] =
{
	// COMMON
	{"TELEPORTATION",				TIMESOCKET_FUNC::TELEPORTATION},
	{"TRANSLATION",					TIMESOCKET_FUNC::TRANSLATION},
	{"TRANSLATION_LERP",			TIMESOCKET_FUNC::TRANSLATION_LERP},
	{"ROTATION",					TIMESOCKET_FUNC::ROTATION},
	{"ROTATION_LERP",				TIMESOCKET_FUNC::ROTATION_LERP},
	{"AFFINE_LERP",					TIMESOCKET_FUNC::AFFINE_LERP},
	{"TOGGLE_FLAGS",				TIMESOCKET_FUNC::TOGGLE_FLAGS},
	{"SET_WORLD_LOCKPOSITION",		TIMESOCKET_FUNC::SET_WORLD_LOCKPOSITION},

	// CAMERA
	{"SETTING_CAMERA",		TIMESOCKET_FUNC::SETTING_CAMERA},
	{"BIND_CAMERA",			TIMESOCKET_FUNC::BIND_CAMERA},
	{"ZOOM_IN",				TIMESOCKET_FUNC::ZOOM_IN},
	{"ZOOM_OUT",			TIMESOCKET_FUNC::ZOOM_OUT},
	{"ZOOM_LERP",			TIMESOCKET_FUNC::ZOOM_LERP},
	{"FADE_IN",				TIMESOCKET_FUNC::FADE_IN},
	{"FADE_OUT",			TIMESOCKET_FUNC::FADE_OUT},
	{"END_CINEMATIC",		TIMESOCKET_FUNC::END_CINEMATIC},
	{"SET_PITCHLIMIT",		TIMESOCKET_FUNC::SET_PITCHLIMIT},
	{"LOOK_AT",				TIMESOCKET_FUNC::LOOK_AT},
	{"DONT_LOOK_AT",		TIMESOCKET_FUNC::DONT_LOOK_AT},
	{"FOLLOW",				TIMESOCKET_FUNC::FOLLOW},
	{"DONT_FOLLOW",			TIMESOCKET_FUNC::DONT_FOLLOW},
	{"SHAKE",				TIMESOCKET_FUNC::SHAKE},

	// CHARACTER
	{"SET_ANIMSTATE",			TIMESOCKET_FUNC::SET_ANIMSTATE},
	{"SET_ANIMTRACKPOSITION",	TIMESOCKET_FUNC::SET_ANIMTRACKPOSITION},
	{"SET_FSMSTATE",			TIMESOCKET_FUNC::SET_FSMSTATE},
	{"BIND_SOCKET_MATRIX",		TIMESOCKET_FUNC::BIND_SOCKET_MATRIX},
	{"UNBIND_SOCKET_MATRIX",	TIMESOCKET_FUNC::UNBIND_SOCKET_MATRIX},

	// EFFECT

	// SOUND

	// TRIGGER
	{"SET_CAMERA_ENVIRONMENT",		TIMESOCKET_FUNC::SET_CAMERA_ENVIRONMENT},

	//SYSTEM
	{"STOP_CINEMATIC",		TIMESOCKET_FUNC::STOP_CINEMATIC},
	{"RESET_ENVIRONMENT",	TIMESOCKET_FUNC::RESET_ENVIRONMENT},
	{"SET_VOLUMETRIC",		TIMESOCKET_FUNC::SET_VOLUMETRIC},

	{"END",					TIMESOCKET_FUNC::END},
};

typedef struct tagSocketContents {
	_float				fRatio = { 0.f };
	_bool				bTriggerred = { false };
	CGameObject*		pEventTarget = { nullptr };
	_string				strEventName = {};
	_wstring			wstrLayerName = {};
	_wstring			wstrKeyName = {};
	TIMESOCKET_PARAM	eTypeParam = {};
	TIMESOCKET_FUNC		eTypeFunc = {};
	CGameObject*		pOtherTarget = { nullptr };
	PSX::PxTransform	pxTransform;
	_boolean			vFlags = {};
	_uint4				vParam_10 = {};
	_float4				vParam_11 = {};
	_string				vParam_12 = {};
	function<void(CTimeSocket&)>		funcEvent = { nullptr };
}SOCKETCONTENTS;

class ENGINE_DLL CTimeSocket final : public CBase
{
private:
	CTimeSocket();
	virtual ~CTimeSocket() = default;
public:
	SOCKETCONTENTS m_Contents = {};
	virtual _bool Trigger(_float fRatio);

	inline static _bool TryParse_TimeSocketParam(string_view text, TIMESOCKET_PARAM& outValue)
	{
		return CMyTools::TryParseEnum(text, g_SocketParamTypeTable, outValue);
	}

	inline static string_view ToString_TimeSocketParam(TIMESOCKET_PARAM value)
	{
		return CMyTools::EnumToString(value, g_SocketParamTypeTable, "UNKNOWN_PARAM");
	}

	inline static _bool TryParse_TimeSocketFunc(string_view text, TIMESOCKET_FUNC& outValue)
	{
		return CMyTools::TryParseEnum(text, g_SocketFuncTypeTable, outValue);
	}

	inline static string_view ToString_TimeSocketFunc(TIMESOCKET_FUNC value)
	{
		return CMyTools::EnumToString(value, g_SocketFuncTypeTable, "UNKNOWN_FUNC");
	}

public:
	static CTimeSocket* Create(void* pArg);
	virtual void Free() override;
private:
	HRESULT Initialize(void* pArg);
	void Finalize();
};

NS_END
