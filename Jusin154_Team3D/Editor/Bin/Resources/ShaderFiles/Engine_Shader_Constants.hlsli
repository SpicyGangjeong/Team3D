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

static const int AI_TEXTURE_TYPE_NONE                           = 0 /* / 27*/;
static const int AI_TEXTURE_TYPE_DIFFUSE                        = 1 /* / 27*/;
static const int AI_TEXTURE_TYPE_SPECULAR                       = 2 /* / 27*/;
static const int AI_TEXTURE_TYPE_AMBIENT                        = 3 /* / 27*/;
static const int AI_TEXTURE_TYPE_EMISSIVE                       = 4 /* / 27*/;
static const int AI_TEXTURE_TYPE_HEIGHT                         = 5 /* / 27*/;
static const int AI_TEXTURE_TYPE_NORMALS                        = 6 /* / 27*/;
static const int AI_TEXTURE_TYPE_SHININESS                      = 7 /* / 27*/;
static const int AI_TEXTURE_TYPE_OPACITY                        = 8 /* / 27*/;
static const int AI_TEXTURE_TYPE_DISPLACEMENT                   = 9 /* / 27*/;
static const int AI_TEXTURE_TYPE_LIGHTMAP                       = 10/* / 27*/;
static const int AI_TEXTURE_TYPE_REFLECTION                     = 11/* / 27*/;
static const int AI_TEXTURE_TYPE_BASE_COLOR                     = 12/* / 27*/;
static const int AI_TEXTURE_TYPE_NORMAL_CAMERA                  = 13/* / 27*/;
static const int AI_TEXTURE_TYPE_EMISSION_COLOR                 = 14/* / 27*/;
static const int AI_TEXTURE_TYPE_METALNESS                      = 15 /* / 27*/;
static const int AI_TEXTURE_TYPE_DIFFUSE_ROUGHNESS              = 16/* / 27*/;
static const int AI_TEXTURE_TYPE_AMBIENT_OCCLUSION              = 17/* / 27*/;
static const int AI_TEXTURE_TYPE_UNKNOWN                        = 18/* / 27*/;
static const int AI_TEXTURE_TYPE_SHEEN                          = 19/* / 27*/;
static const int AI_TEXTURE_TYPE_CLEARCOAT                      = 20/* / 27*/;
static const int AI_TEXTURE_TYPE_TRANSMISSION                   = 21 /* / 27*/;
static const int AI_TEXTURE_TYPE_MAYA_BASE                      = 22/* / 27*/;
static const int AI_TEXTURE_TYPE_MAYA_SPECULAR                  = 23/* / 27*/;
static const int AI_TEXTURE_TYPE_MAYA_SPECULAR_COLOR            = 24/* / 27*/;
static const int AI_TEXTURE_TYPE_MAYA_SPECULAR_ROUGHNESS        = 25/* / 27*/;
static const int AI_TEXTURE_TYPE_ANISOTROPY                     = 26 /* / 27*/;
static const uint AI_TEXTURE_TYPE_GLTF_METALLIC_ROUGHNESS       = 27;
static const uint AI_TEXTURE_TYPE_MAX = AI_TEXTURE_TYPE_GLTF_METALLIC_ROUGHNESS;


#endif // ENGINE_SHADER_CONSTANTS_HLSLI
