#pragma once



#ifndef GUI
#define	GUI	ImGui
#endif // !GUI
#ifndef IMGUI_GLOBAL_BEGIN_FLAG
#define IMGUI_GLOBAL_BEGIN_FLAG  ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoBringToFrontOnFocus
#endif // IMGUI_GLOBAL_BEGIN_FLAG
#define COMPUTE_TIMEDELTA(TIMER) m_pGameInstance->Compute_TimeDelta(TEXT(TIMER))

#ifndef PSX
#define PSX physx
#endif // !PSX


#ifdef _DEBUG
#define RELEASE_DEBUGGER
#endif // _DEBUG
#ifndef _DEBUG
#define RELEASE_DEBUGGER
#endif // !_DEBUG

//#ifdef 기무리
//#define DEBUG_MOTIONBLUR
//#define DEBUG_SSAO
//#define DEBUG_SHADOW
//#define DEBUG_CAMERAS
//#endif // 기무리



#pragma region ProgrammerInfo

#define ASSERT_NURI(expression)
#define ASSERT_JINWOO(expression)
#define ASSERT_JINHO(expression)
#define ASSERT_HYUNBIN(expression)
#define ASSERT_INHYUK(expression)
#define IMGUI_GLOBAL_ITEM_WIDTH 160

#define OPTIONAL_TRUE_KEYINPUTGUICHECK

#define USE_LOWTEXTURE
#ifndef _DEBUG // 릴리즈 모드일 때는 고해상도 유지
#undef USE_LOWTEXTURE
#endif // !_DEBUG

#ifdef _DEBUG
#ifdef 기무리
#undef ASSERT_NURI
#define ASSERT_NURI(expression) assert(expression)
#define MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK); assert(false); 
#undef USE_LOWTEXTURE // 매크로 옵션으로 해상도 조절, 주석 걸면 디버그모드에서 저해상도 유지
//#undef OPTIONAL_TRUE_KEYINPUTGUICHECK
//#define OPTIONAL_TRUE_KEYINPUTGUICHECK true||
#elif 진우
#undef ASSERT_JINWOO
#define ASSERT_JINWOO(expression) assert(expression)
#undef USE_LOWTEXTURE // 매크로 옵션으로 해상도 조절, 주석 걸면 디버그모드에서 저해상도 유지
//#undef OPTIONAL_TRUE_KEYINPUTGUICHECK
//#define OPTIONAL_TRUE_KEYINPUTGUICHECK true||
#elif gimch
#undef ASSERT_JINHO
#define ASSERT_JINHO(expression) assert(expression)
#undef USE_LOWTEXTURE // 매크로 옵션으로 해상도 조절, 주석 걸면 디버그모드에서 저해상도 유지
#undef OPTIONAL_TRUE_KEYINPUTGUICHECK
#define OPTIONAL_TRUE_KEYINPUTGUICHECK true||
#elif Bin
#undef ASSERT_HYUNBIN
#define ASSERT_HYUNBIN(expression) assert(expression)
#undef USE_LOWTEXTURE // 매크로 옵션으로 해상도 조절, 주석 걸면 디버그모드에서 저해상도 유지
//#undef OPTIONAL_TRUE_KEYINPUTGUICHECK
//#define OPTIONAL_TRUE_KEYINPUTGUICHECK ||true
#elif 나
#undef ASSERT_INHYUK
#define ASSERT_INHYUK(expression) assert(expression)
#undef USE_LOWTEXTURE // 매크로 옵션으로 해상도 조절, 주석 걸면 디버그모드에서 저해상도 유지
//#undef OPTIONAL_TRUE_KEYINPUTGUICHECK
//#define OPTIONAL_TRUE_KEYINPUTGUICHECK || true
#endif
#endif // _DEBUG


#pragma endregion


#pragma region DINPUT

#ifndef DIK_MAX
#define DIK_MAX 0XFF
#endif // !DIK_MAX
#ifndef DIM_LBUTTON 
#define DIM_LBUTTON 0
#endif // !DIM_LBUTTON 
#ifndef DIM_RBUTTON
#define DIM_RBUTTON 1
#endif // !DIM_RBUTTON
#ifndef DIM_MBUTTON
#define DIM_MBUTTON 2
#endif // !DIM_MBUTTON
#ifndef DIM_3
#define DIM_3 3
#endif // !DIM_3
#ifndef DIM_4
#define DIM_4 4
#endif // !DIM_4
#ifndef DIM_5
#define DIM_5 5
#endif // !DIM_5
#ifndef DIM_6
#define DIM_6 6
#endif // !DIM_6
#ifndef DIM_7
#define DIM_7 7
#endif // !DIM_7
#pragma endregion

#ifndef PURE
#define PURE = 0
#endif

#define FX_DEFERRED			TEXT("FX_DEFERRED")
#define FX_BLUR				TEXT("FX_BLUR")
#define FX_WEIGHTBELND		TEXT("FX_WEIGHTBELND")
#define FX_DISTORTION       TEXT("FX_DISTORTION")

#define CS_EFFECT					TEXT("CS_EFFECT")
#define CS_INSTANCE_MODEL			TEXT("CS_INSTANCE_MODEL")
#define CS_MODEL					TEXT("CS_MODEL")
#define CS_LOCAL					TEXT("CS_LOCAL")
#define CS_BONEINSERT				TEXT("CS_BONEINSERT")


#define			SAFE_ADDREF(pInstance)		Safe_AddRef(pInstance)
#define			SAFE_RELEASE(pInstance)		Safe_Release(pInstance)

//if ((_float)A - (_float)B < FLT_EPSILON3) {
//	return true;
//}
#define			FLT_EPSILON3				1.192092896e-03F
#define			FLT_EPSILON5				1.192092896e-05F

// GRAVITY 음수 박으면 피직스 로드 실패함
#define			GRAVITY						9.81f


#define			SSAO_SAMPLE_NUMBER			64
#define			TIMER_SHORT_LERP			0.16f
#define         MRO_PARAMETER				2.f/ 27.f
#define         SRO_PARAMETER				15.f/ 27.f

constexpr unsigned int g_iNearShadowWidth		= 8192;
constexpr unsigned int g_iNearShadowHeight		= 8192;

constexpr unsigned int g_iMiddleShadowWidth		= 4096;
constexpr unsigned int g_iMiddleShadowHeight	= 4096;

constexpr unsigned int g_iFarShadowWidth		= 1024;
constexpr unsigned int g_iFarShadowHeight		= 1024;

constexpr unsigned int g_iPreShadowWidth		= 8192;
constexpr unsigned int g_iPreShadowHeight		= 8192;



#ifndef AI_TEXTURE_TYPE_MAX
#define AI_TEXTURE_TYPE_MAX 27
#endif // !AI_TEXTURE_TYPE_MAX

#define			ENUM_CLASS(ENUM)		static_cast<unsigned int>(ENUM)
#define			KR_CSTR(str)			reinterpret_cast<const char*>(u8##str)

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			NS_BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			NS_END					}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


// 클래스 내부의 static 변수는 m_ 가 아닌 s_

#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&) = delete;			\
		CLASSNAME& operator = (const CLASSNAME&)= delete;

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	s_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned int DestroyInstance( void );

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::s_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == s_pInstance) {						\
				s_pInstance = new CLASSNAME;					\
			}													\
			return s_pInstance;									\
		}														\
		unsigned int CLASSNAME::DestroyInstance( void ) {		\
			unsigned int iRefCnt = {};							\
			if(nullptr != s_pInstance)	{						\
				iRefCnt = s_pInstance->Release();				\
				s_pInstance = nullptr;							\
			}													\
			return iRefCnt;										\
		}
