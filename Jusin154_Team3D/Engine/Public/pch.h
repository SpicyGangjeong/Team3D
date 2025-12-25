#pragma once

#include <d3d11.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#pragma warning(disable : 4251)

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

// DXTK
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/Effects.h"
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#include "DirectXTK/GeometricPrimitive.h"

// FX11
#include "FX11/d3dx11effect.h"

// TinyXML2
#include "tinyxml2.h"

#ifdef EDITOR_PROJECT
// assimp
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp" 

#endif // EDITOR_PROJECT

using namespace DirectX;

// STL
#include <vector>
#include <list>
#include <array>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <functional>
#include <string>
#include <ctime>
#include <cassert>
#include <random>
#include <fstream>
#include <filesystem>
#include <Thread>
#include <future>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>


using namespace std;

// IMGUI
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_dx11.h"
#include "IMGUI/imgui_impl_win32.h"
#include "tchar.h"

// FMOD
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
using namespace FMOD;


// PhysX
#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 33010)

#include "PhysX/PxPhysicsAPI.h"
#include "PhysX/extensions/PxExtensionsAPI.h"
#pragma warning(pop)


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

