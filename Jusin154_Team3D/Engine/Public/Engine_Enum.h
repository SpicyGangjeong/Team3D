#pragma once

NS_BEGIN(Engine)

enum class STATE { RIGHT, UP, LOOK, POSITION, END };
enum class LIGHT { DIRECTIONAL, POINT, SPOT, END };
enum class COLLIDER { AABB, OBB, SPHERE, END };
enum class RENDER {PRIORITY, SHADOW_NEAR, NONBLEND, DECAL, BLUR, NONLIGHT, EFFECT ,BLEND, BLOOM, DISTORTION, BULR_MESH ,UI, OCCLUSION, SHADOW_MIDDLE, PRESHADOW, UI_OVERLAY ,END };
enum class RAY { LOCAL, WORLD, END };
enum class ACTOR { BOX, CAPSULE, SPHERE, PLANE, TRIANGLEMESH, HEIGHTFIELD, END };
enum class BLEND_BONE{SPINE,SHOULDER_L,SHOULDER_R,NECK,SHOULDER_NECK_L, SHOULDER_NECK_R,HEAD, HIPS_CLOTH, END};
enum class STAT { NAME, CURRENTHP, MAXHP, TARGETHP, MELEE, MAGIC, DEFENSE, SPEED, AGILITY, LEVEL, EXPERIENCE, MAX_EXPERIENCE, GOLD, END };
//enum class CONDITION { POISON, HPREGENERATION, PANIC, GRAP, END };
enum class SHADOW : _ubyte { 
		SHADOW_NEAR		= 1 << 0
	,	SHADOW_MIDDLE	= 1 << 1
	,	SHADOW_FAR		= 1 << 2 // Far는 현재 미사용
	,	SHADOW_PRE		= 1 << 3
////////////////
	,	END = 3 // 케스케이드 셰도우 한정
};


enum class TEXTURE_LOAD_TYPE { 
    SINGLE,  // original/diffuseddd.png
    INCREMENTAL, // original/diffuse[%d].png
    PATH, // original/
	CUBE,
    END 
};
enum class PXMATERIAL {
    DEFAULT,
    END, // 정지마찰, 운동마찰, 반발계수
};
enum class PXBODYSTATE {
    DYNAMIC,
    KINEMATIC,
};

enum class MOUSEKEYSTATE { LBUTTON, RBUTTON, WBUTTON, XBUTTON };
enum class MOUSEMOVESTATE { X, Y, W };
enum class D3DTS { VIEW, PROJ, VIEW_INV, PROJ_INV, END };
enum class MODEL { ANIM, NONANIM, PBR_NONANIM, PBR_ANIM, ENVIRONMENT, ANIM_LOCAL,NONANIM_LOCAL, END};

enum class NAVI_POINT { A, B, C, END };
enum class NAVI_LINE { AB, BC, CA, END };
enum class SHADER_PASS_CELL { DEBUG, END };

enum class SHADER_PASS_DEFERRED { DEBUG, DIRECTIONAL, POINT, COMBINED, BLUR, SPOT,
	UPSAMPLE, BLOOM_BLURX, BLOOM_BLURY, EMBOSSING, BLOOM_ACCUM, BLOOM_FINISH, BLURX_ENVIRONMENT,
	POSTCOMBINED, FOG, TONE_MAPPING, SSAO_OCCLUSION, SSAO_BLUR, MOTIONBLUR, DOWNSAMPLE, MOTIONBLURTILE, MOTIONBLURTENT, PRINT_BACKBUFFER,
	END };

enum class SHADER_PASS_BLUR { BLUR_X , BLUR_Y , COMBINED,  END};

enum class TEXTURE_MOUNT_SLOT0 { NOT_DEFINED, METALLIC, SPECULAR, HEIGHT,   END };
enum class TEXTURE_MOUNT_SLOT1 { NOT_DEFINED, ROUGHNESS,                    END };
enum class TEXTURE_MOUNT_SLOT2 { NOT_DEFINED, OCCLUSION, ALPHA, Amount_Scatter, END };
enum class TEXTURE_MOUNT_SLOT3 { NOT_DEFINED, HEIGHT, ALPHA, Brightness_Scatter, END };

enum PBR_FLAG
{
	PBR_NONE = 0,
	PBR_MRO = 1,
	PBR_SRO = 2,
	PBR_MROH = 3,
	PBR_MROA = 4,
	PBR_SROH = 5,
	PBR_SROA = 6,
	PBR_MROA_MASK = 7,
	PBR_MRS = 8,
	PBR_SRA = 9,
	PBR_HRO = 10
};

#ifndef EDITOR_PROJECT
enum aiTextureType {
    aiTextureType_NONE = 0,
    aiTextureType_DIFFUSE = 1,
    aiTextureType_SPECULAR = 2,
    aiTextureType_AMBIENT = 3,
    aiTextureType_EMISSIVE = 4,
    aiTextureType_HEIGHT = 5,
    aiTextureType_NORMALS = 6,
    aiTextureType_SHININESS = 7,
    aiTextureType_OPACITY = 8,
    aiTextureType_DISPLACEMENT = 9,
    aiTextureType_LIGHTMAP = 10,
    aiTextureType_REFLECTION = 11,
    aiTextureType_BASE_COLOR = 12,
    aiTextureType_NORMAL_CAMERA = 13,
    aiTextureType_EMISSION_COLOR = 14,
    aiTextureType_METALNESS = 15,
    aiTextureType_DIFFUSE_ROUGHNESS = 16,
    aiTextureType_AMBIENT_OCCLUSION = 17,
    aiTextureType_UNKNOWN = 18,
    aiTextureType_SHEEN = 19,
    aiTextureType_CLEARCOAT = 20,
    aiTextureType_TRANSMISSION = 21,
    aiTextureType_MAYA_BASE = 22,
    aiTextureType_MAYA_SPECULAR = 23,
    aiTextureType_MAYA_SPECULAR_COLOR = 24,
    aiTextureType_MAYA_SPECULAR_ROUGHNESS = 25,
    aiTextureType_ANISOTROPY = 26,
    aiTextureType_GLTF_METALLIC_ROUGHNESS = 27,

#ifndef SWIG
    _aiTextureType_Force32Bit = INT_MAX
#endif
};
#endif // !EDITOR_PROJECT

enum class PHYSX_KIND {
    NOT_DEFINED,
    BODY_STATIC,
    BODY_DYNAMIC,
    CCTActor,
    OBSTACLEActor,
};

enum class PHYSX_JOINT {
	D6,
	DISTANCE,
	REVOLUTE,
	FIXED,
	END
};

enum class PXOBJECT : _uint {
#pragma region NOT_DEFINE
	NOT_DEFINE = 0,
	LEG,
#pragma endregion
#pragma region PLAYER
	PLAYER = 10,
	AI,
#pragma endregion
#pragma region MONSTER
	MONSTER = 30,
	GOBLIN_WARRIOR,		// 31
	GOBLIN_MAGICIAN,
	GOBLIN_ASSASSIN,
	TROLL,
	PENSIVE_GUARDIAN,
	RANROK,

#pragma endregion
#pragma region HOSTILE_HITBOX
	HOSTILE_HITBOX = 100,
	GOBLIN_SMASH,
	GOBLIN_DAGGER,
	GOBLIN_SHIELD_BREAK_MELEE,
	GOBLIN_SHIELD_BREAK_MAGIC,
	GOBLIN_NORMAL_MAGIC,
	TROLL_GROUND_STRIKE,
	GOBLIN_PROTEGO,
#pragma endregion
#pragma region ALLY_HITBOX
	ALLY_HITBOX = 200,
	SKILL_REVAILIO,
	SKILL_PROTEGO,
	SKILL_ACCIO,
	SKILL_DEPULSO,
	SKILL_DEPULSO_OBSTACLE,
	SKILL_DEPULSO_OBSTACLE_FLAMED,
	SKILL_NORMALJAP,
	SKILL_NORMALJAP_FINAL,
	SKILL_BOMBARD_STRIKE,
	SKILL_BOMBARD_FLAME,
	SKILL_INCENDIO_STRIKE,
	SKILL_INCENDIO_SPREAD,
#pragma endregion
#pragma region NEUTRAL_HITBOX
	NEUTRAL_HITBOX = 300,
	FLAMED_FLOOR,

#pragma endregion
#pragma region ENVIRIONMENT
	ENVIRIONMENT = 400,
	TERRAIN,
	FLOOR,
	ROAD,
	WATER,
	WALL,
	WALL_BREAKABLE,
	WALL_CLIMBABLE,
	DOOR,
	BUILDING,
	BOX,
	RACERING,
#pragma endregion
#pragma region NPC
	NPC = 500,
	OLLIVANDER,
	ELEAZARFIG,

#pragma endregion
#pragma region ITEM
	ITEM = 600,
	DROPITEM = 630,
	TRAP = 660,

#pragma endregion
#pragma region TRIGGERBOX
	TRIGGERBOX = 700,


	CINEMATIC = 770,
	CINEMATIC_INTRO,
	CINEMATIC_HOGSMEADE_ENTER,
	CINEMATIC_TROLL_ENCOUNTER,
	CINEMATIC_TROLL_DEAD,
#pragma endregion
#pragma region REGION
	REGION = 800,
	HOGWARTS,
	HOGSMEADE,

#pragma endregion
#pragma region NOCOLLIDE
	NOCOLLIDE = 900,
	GAIL,
	JOINT_ROUTE,
	JOINT_ANCHOR,

#pragma endregion
	END = 999
};

enum class HIT_TYPE
{
	HIT_NONE = 0,				// 히트모션 없음
	HIT_PROJECTILE,				// 발사체
	HIT_MEDIUM,				// 적당한 공격
	HIT_HEAVY,				// 강공격
	END
};


NS_END


/*
// 스텐실 버퍼 마스크
// 0000 0001 -> 아웃라인
// 0000 0010 -> 터레인 및 지형 및 건물 등등

*/

//enum PxRigidBodyFlag::Enum // createBody에 들어감
//{
// // 해당 바디는 키네마틱 모드가 됨. 
// // 중력, 힘, 모멘텀에 영향을 안받고 무한질량으로 취급함
// // 다른 다이나믹 바디는 밀어내지만
// // 스태틱, 키네마틱과는 충돌x, CCT도 이걸 씀
// // 직접 개발자가 이동시켜줘야 함
// // CCD와 호환되지 않음, 자동으로 한쪽이 꺼짐
// // 키는 순간 바디는 sleep 상태가 되고 속도도 0이 됨
//    [eKINEMATIC]
// 
// // 주의 * 다음 프레임에 적용됨 *
// // 씬 쿼리를 세팅하고 이미 움직인 것 처럼 씬 쿼리를 하고 싶을 때 씀
//    [eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES]
// 
// // 고속 이동으로 인한 튕김/뚫림 방지를 이 바디에 등록함
// // 이 플래그가 필요한 shape 페어에는 eDetect_ccd_contact도 켜야 됨
// // 키네마틱과 호환되지 않음
// // 많이 키면 렉걸림
//    [eENABLE_CCD]
// 
// // CCD에 마찰을 활성화 시킴
// // 총알이 표면을 스치면서 마찰로 속도감소 등
//    [eENABLE_CCD_FRICTION]
// 
// // contact offset을 바디 속도에 따라 키워서
// // 관통을 줄이는 방식의 CCD
// // 엄밀히 CCD는 아니지만 터널링을 줄일 순 있음
//    [eENABLE_SPECULATIVE_CCD]
// 
// // 물리 결과를 scene.fetchResult 도중에 얻을 수 있음
// // 콜백으로 받게 됨, 물리 결과를 미리 반영하고 싶을 때 사용
//    [eENABLE_POSE_INTEGRATION_PREVIEW]
// 
// // 충돌 시 적용되는 최대 임펄스 수를 제한?
// // 일반 접촉에만 적용되던 걸 CCD에도 적용하게끔 만듬
// // 보통 off라고 함
//    [eENABLE_CCD_MAX_CONTACT_IMPULSE]
// 
// // 힘/가속도가 다음 시뮬레이션에서도 계속해서 쓰임
// // addForce(), addTorque() 등으로 준 것들은 원래는 시뮬 끝나면 지워짐
// // 레일건 같은 총알이나 우주선 등 
//    [eRETAIN_ACCELERATIONS]
// 
// // 키네마틱페어를 전체적으로 없애는 
// // PxSceneDesc::kineKineFilteringMode가 켜져 있어도
// // 키네마틱 페어를 서로 충돌이벤트 발생시키게 함
// // 런타임에 바꾸면 씬에서 제거하고 다시 추가하기 전까지 반영 안됨
//    [eFORCE_KINE_KINE_NOTIFICATIONS]
// 
// // 위랑 똑같은데 위에껀 키네마틱 페어라면
// // 이건 스태틱-키네마틱 페어임
//    [eFORCE_STATIC_KINE_NOTIFICATIONS]
// 
// // 강하게 회전하는 바디에서 관성에 의한 보정을 더 엄밀히 계산함
// // 비용증가, 운동량 안정화
//    [eENABLE_GYROSCOPIC_FORCES]
// 
// // 예약 플래그임. 안써야함
//    [eRESERVED]
//};
//


//enum PxShapeFlag::Enum // createShape에 들어감
//{
// // 물리 시뮬레이션에서 충돌/접촉 계산에 참여, 
// // PxSimulationEventCallback::onContact 접촉 이벤트도 이 플래그 들 끼리의 충돌에서 나옴
// // eTrigger_Shape와 동시에 킬 수 없음 
// // (default = on)
//    [eSIMULATION_SHAPE],
// 
// 
// // 씬쿼리 대상이 됨, 레이케스트, 오버랩, 스윕 등
// // 충돌시뮬레이션과는 별도의 동작 
// // (default = on)
//    [eSCENE_QUERY_SHAPE],
// 
// // 다른 shape가 들어오고 나갈 때 onTrigger 콜백을 발생시킴
// // 통과가능한 shape이며 이벤트만 발생시킴
// // TriangleMesh, HeightField는 트리거사용불가 
// // (default = off)
//    [eTRIGGER_SHAPE],
// 
// // 디버그용, 전용디버거 PVD에 이 shape를 그리게 함
// // 꺼도 되고, 키면 PxScene에서 설정을 해줘야함 
// // (default = on)
//    [eVISUALIZATION],
// 
// // PhysX Particle들이 닿는 순간 그 파티클을 '흡수'해서 제거함
// // (default = off)
//    [ePARTICLE_DRAIN],
//};
