#ifndef ENGINE_SHADER_CONSTANTS_HLSLI
#define ENGINE_SHADER_CONSTANTS_HLSLI

static const vector g_vMtrlDiffuse = 1.f;
static const vector g_vMtrlAmbient = 1.f;
static const vector g_vMtrlSpecular = 1.f;

static const float PI = 3.14159265359;
static const float PI2 = 6.28318530718;

static const float FLT_EPSILON7 = 1.192092896e-07;
static const float FLT_EPSILON5 = 1.192092896e-05;
static const float FLT_EPSILON3 = 1.192092896e-03;


static const float AI_TEXTURE_TYPE_NONE                     = 0.f  / 27.f;
static const float AI_TEXTURE_TYPE_DIFFUSE                  = 1.f  / 27.f;
static const float AI_TEXTURE_TYPE_SPECULAR                 = 2.f  / 27.f;
static const float AI_TEXTURE_TYPE_AMBIENT                  = 3.f  / 27.f;
static const float AI_TEXTURE_TYPE_EMISSIVE                 = 4.f  / 27.f;
static const float AI_TEXTURE_TYPE_HEIGHT                   = 5.f  / 27.f;
static const float AI_TEXTURE_TYPE_NORMALS                  = 6.f  / 27.f;
static const float AI_TEXTURE_TYPE_SHININESS                = 7.f  / 27.f;
static const float AI_TEXTURE_TYPE_OPACITY                  = 8.f  / 27.f;
static const float AI_TEXTURE_TYPE_DISPLACEMENT             = 9.f  / 27.f;
static const float AI_TEXTURE_TYPE_LIGHTMAP                 = 10.f / 27.f;
static const float AI_TEXTURE_TYPE_REFLECTION               = 11.f / 27.f;
static const float AI_TEXTURE_TYPE_BASE_COLOR               = 12.f / 27.f;
static const float AI_TEXTURE_TYPE_NORMAL_CAMERA            = 13.f / 27.f;
static const float AI_TEXTURE_TYPE_EMISSION_COLOR           = 14.f / 27.f;
static const float AI_TEXTURE_TYPE_METALNESS                = 15.f / 27.f;
static const float AI_TEXTURE_TYPE_DIFFUSE_ROUGHNESS        = 16.f / 27.f;
static const float AI_TEXTURE_TYPE_AMBIENT_OCCLUSION        = 17.f / 27.f;
static const float AI_TEXTURE_TYPE_UNKNOWN                  = 18.f / 27.f;
static const float AI_TEXTURE_TYPE_SHEEN                    = 19.f / 27.f;
static const float AI_TEXTURE_TYPE_CLEARCOAT                = 20.f / 27.f;
static const float AI_TEXTURE_TYPE_TRANSMISSION             = 21.f / 27.f;
static const float AI_TEXTURE_TYPE_MAYA_BASE                = 22.f / 27.f;
static const float AI_TEXTURE_TYPE_MAYA_SPECULAR            = 23.f / 27.f;
static const float AI_TEXTURE_TYPE_MAYA_SPECULAR_COLOR      = 24.f / 27.f;
static const float AI_TEXTURE_TYPE_MAYA_SPECULAR_ROUGHNESS  = 25.f / 27.f;
static const float AI_TEXTURE_TYPE_ANISOTROPY               = 26.f / 27.f;
static const uint  AI_TEXTURE_TYPE_GLTF_METALLIC_ROUGHNESS  = 1;
static const uint  AI_TEXTURE_TYPE_MAX = AI_TEXTURE_TYPE_GLTF_METALLIC_ROUGHNESS;


#endif // ENGINE_SHADER_CONSTANTS_HLSLI
