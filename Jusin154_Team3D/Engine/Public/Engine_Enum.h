#pragma once

NS_BEGIN(Engine)

enum class STATE { RIGHT, UP, LOOK, POSITION, END };
enum class LIGHT { DIRECTIONAL, POINT, SPOT, END };
enum class COLLIDER { AABB, OBB, SPHERE, END };
enum class RENDER { PRIORITY, SHADOW, NONBLEND, BLUR, NONLIGHT, BLEND, UI, END };
enum class RAY { LOCAL, WORLD, END };


enum class TEXTURE_LOAD_TYPE { 
    SINGLE,  // original/diffuseddd.png
    INCREMENTAL, // original/diffuse[%d].png
    PATH, // original/
    END 
};



enum class MOUSEKEYSTATE { LBUTTON, RBUTTON, WBUTTON, XBUTTON };
enum class MOUSEMOVESTATE { X, Y, W };
enum class D3DTS { VIEW, PROJ, VIEW_INV, PROJ_INV, END };
enum class MODEL { ANIM, NONANIM };

enum class NAVI_POINT { A, B, C, END };
enum class NAVI_LINE { AB, BC, CA, END };
enum class SHADER_PASS_CELL { DEBUG, END };

enum class SHADER_PASS_DEFERRED { DEBUG, DIRECTIONAL, POINT, COMBINED, BLUR, SPOT, END };

#ifndef EDITOR_PROJECT
enum aiTextureType {
    aiTextureType_NONE                      = 0,
    aiTextureType_DIFFUSE                   = 1,
    aiTextureType_SPECULAR                  = 2,
    aiTextureType_AMBIENT                   = 3,
    aiTextureType_EMISSIVE                  = 4,
    aiTextureType_HEIGHT                    = 5,
    aiTextureType_NORMALS                   = 6,
    aiTextureType_SHININESS                 = 7,
    aiTextureType_OPACITY                   = 8,
    aiTextureType_DISPLACEMENT              = 9,
    aiTextureType_LIGHTMAP                  = 10,
    aiTextureType_REFLECTION                = 11,
    aiTextureType_BASE_COLOR                = 12,
    aiTextureType_NORMAL_CAMERA             = 13,
    aiTextureType_EMISSION_COLOR            = 14,
    aiTextureType_METALNESS                 = 15,
    aiTextureType_DIFFUSE_ROUGHNESS         = 16,
    aiTextureType_AMBIENT_OCCLUSION         = 17,
    aiTextureType_UNKNOWN                   = 18,
    aiTextureType_SHEEN                     = 19,
    aiTextureType_CLEARCOAT                 = 20,
    aiTextureType_TRANSMISSION              = 21,
    aiTextureType_MAYA_BASE                 = 22,
    aiTextureType_MAYA_SPECULAR             = 23,
    aiTextureType_MAYA_SPECULAR_COLOR       = 24,
    aiTextureType_MAYA_SPECULAR_ROUGHNESS   = 25,
    aiTextureType_ANISOTROPY                = 26,
    aiTextureType_GLTF_METALLIC_ROUGHNESS   = 27,

#ifndef SWIG
    _aiTextureType_Force32Bit = INT_MAX
#endif
};
#endif // !EDITOR_PROJECT

NS_END