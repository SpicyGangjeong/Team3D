#pragma once

#ifndef GUI
#define	GUI	ImGui
#endif // !GUI

#ifndef PSX
#define PSX physx
#endif // !PSX

#pragma region ProgrammerInfo

#define ASSERT_NURI(expression)
#define ASSERT_JINWOO(expression)
#define ASSERT_JINHO(expression)
#define ASSERT_HYUNBIN(expression)
#define ASSERT_INHYUK(expression)

#ifdef 기무리
#undef ASSERT_NURI
#define ASSERT_NURI(expression) assert(expression)
#elif kim05
#undef ASSERT_JINWOO
#define ASSERT_JINWOO(expression) assert(expression)
#elif 82103
#undef ASSERT_JINHO
#define ASSERT_JINHO(expression) assert(expression)
#elif Bin
#undef ASSERT_HYUNBIN
#define ASSERT_HYUNBIN(expression) assert(expression)
#elif 인혁
#undef ASSERT_INHYUK
#define ASSERT_INHYUK(expression) assert(expression)
#endif

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

#define			SAFE_ADDREF(pInstance)		Safe_AddRef(pInstance)
#define			SAFE_RELEASE(pInstance)		Safe_Release(pInstance)

//if ((_float)A - (_float)B < FLT_EPSILON3) {
//	return true;
//}
#define			FLT_EPSILON3				1.192092896e-03F
#define			FLT_EPSILON5				1.192092896e-05F

// GRAVITY 음수 박으면 피직스 로드 실패함
#define			GRAVITY						9.81f

constexpr unsigned int g_iMaxShadowWidth = 16384;
constexpr unsigned int g_iMaxShadowHeight = 9216;

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
				if(0 == iRefCnt)								\
					s_pInstance = nullptr;						\
			}													\
			return iRefCnt;										\
		}