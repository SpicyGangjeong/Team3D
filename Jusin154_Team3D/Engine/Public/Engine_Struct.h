#pragma once

NS_BEGIN(Engine)

typedef struct tagEngineDesc
{
	HINSTANCE			hInstance = {};
	HWND				hWnd = {};
	_uint				iNumLevels = {};
	_uint				iNumCollidableGroup = {};
	_uint				iStaticLevel = {};
	_uint				iWinSizeX, iWinSizeY = {};
}ENGINE_DESC;

typedef struct tagModelDesc {
	vector<class CMesh*>* pMeshes = { nullptr };
	vector<class CMaterial*>* pMaterials = { nullptr };
	vector<class CBone*>* pBones = { nullptr };
	vector<class CAnimation*>* pAnimations = { nullptr };
	MODEL						eType = {};
	_float4x4					PreTransformMatrix = {};
	_uint						iRootBoneIndex = {};
}MODEL_DESC;

typedef struct tagKeyFrame
{
	XMFLOAT3		vScale = {};
	XMFLOAT4		vRotation = {};
	XMFLOAT3		vTranslation = {};
	_float			fTrackPosition = {};

}KEYFRAME;

struct LERPDESC {
	_int iSlot = {};
	tagKeyFrame tagKeyFrame = {};
};

typedef struct tagLightDesc
{
	LIGHT				eType = {};
	_float4				vDiffuse = {};
	_float4				vAmbient = {};
	_float4				vSpecular = {};

	const XMFLOAT4* pDirection = nullptr;
	const XMFLOAT4* pPosition = nullptr;

	_float4				vPosOffset = {};

	_float				fRange = {};
	_float2				vSpotAngles = {};

	_uint				iLevel = {};
}LIGHT_DESC;

typedef struct tagShadowLight
{
	_float4		vEye{}, vAt{};
	_float		fWidth{}, fHeight{}, fNear{}, fFar{};
}SHADOW_LIGHT_DESC;

struct SaveVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT3 Tan;
	XMFLOAT3 BiNoraml;
	XMFLOAT2 UV;

	bool bHasNormal;
	bool bHasTan;
	bool bHasUV;

	XMUINT4 BlendIndex;
	XMFLOAT4 BlendWeight;
};

struct SaveBoneWeight
{
	unsigned int VertexId = {};
	float Weight = {};
};

struct SaveBone
{
	string BoneName;

	unsigned int BoneNameSize = {};
	_float4x4 OffsetMatrix = {};
	int BoneIndex = {};
	unsigned int WeightsCount = {};
	vector<SaveBoneWeight>Weights = {};
};

struct SaveNode
{
	string NodeName;

	unsigned int NodeNameSize = {};
	int ParentIndex = {};
	unsigned int ChildrenCount = {};
	_float4x4 Transformation = {};
	vector<int> ChildrenIndices;
};

struct SaveMesh
{
	string MeshName;

	unsigned int MeshNameSize = {};
	unsigned int VertexCount = {};
	unsigned int IndexCount = {};
	unsigned int MaterialIndex = {};
	unsigned int BoneCount = {};

	vector<SaveVertex> Vertices;
	vector<unsigned int> Indices;

	vector<SaveBone> Bones;
};

struct SaveMaterial
{
	vector<string> Path[27];

};

struct SaveKeyFrameVec
{
	float Time;
	XMFLOAT3 Value;
};

struct SaveKeyFrameRotation
{
	float Time;
	XMFLOAT4 Value;
};

struct SaveChannel
{
	string ChannelName;

	unsigned int ChannelNameSize = {};
	unsigned int ScalingKeyCount = { };
	unsigned int RotationKeyCount = { };
	unsigned int PositionKeyCount = { };

	vector<SaveKeyFrameVec> ScalingKeys;
	vector<SaveKeyFrameRotation> RotationKeys;
	vector<SaveKeyFrameVec> PositionKeys;
};

struct SaveAnimation
{
	string AnimName;

	unsigned int AnimNameSize = {};
	float mDuration = {};
	float mTicksPerSecond = {};
	unsigned int ChannelCount = {};
	vector<SaveChannel> Channels;
};

struct SaveModel
{
	unsigned int MeshCount = {};
	unsigned int MaterialCount = {};
	unsigned int AnimationCount = {};
	unsigned int NodeCount = {};
	vector<SaveMesh> Meshes;
	vector<SaveMaterial> Materials;
	vector<SaveAnimation> Animations;
	vector<SaveNode>Nodes;
};


#pragma region VTX

typedef struct tagVertexInstance_Particle
{
	_float4			vRight = {};
	_float4			vUp = {};
	_float4			vLook = {};
	_float4			vTranslation = {};

	_float2			vLifeTime = {};
}VTX_INSTANCE_PARTICLE;

typedef struct tagVertexBlock
{
	_float3 vPosition = {};
	_float3 vNormal = {};
	_float3 vTexcoord = {};

	static constexpr _uint iNumElements = { 3 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXBLOCK;

typedef struct tagVertexPosition {
	_float3	vPosition = {};

	static constexpr _uint iNumElements = { 1 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXPOS;

typedef struct tagVertexPositionTexcoord {
	_float3	vPosition = {};
	_float2	vTexcoord = {};

	static constexpr _uint iNumElements = { 2 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXPOSTEX;

typedef struct tagVertexPositionNormalTexcoord {
	_float3	vPosition = {};
	_float3	vNormal = {};
	_float2	vTexcoord = {};

	static constexpr _uint iNumElements = { 3 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXNORTEX;

typedef struct tagVertexMesh
{
	_float3			vPosition = {};
	_float3			vNormal = {};
	_float3			vTangent = {};
	_float3			vBinormal = {};
	_float2			vTexcoord = {};

	static constexpr _uint					iNumElements = { 5 };
	static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
}VTXMESH;

typedef struct tagVertexAnimationMesh
{
	_float3			vPosition = {};
	_float3			vNormal = {};
	_float3			vTangent = {};
	_float3			vBinormal = {};
	_float2			vTexcoord = {};

	XMUINT4			vBlendIndex = {};
	_float4			vBlendWeight = {};

	static constexpr _uint					iNumElements = { 7 };
	static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0, 48,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BLENDINDEX", 0,	DXGI_FORMAT_R32G32B32A32_UINT,	0, 56,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BLENDWEIGHT",0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
}VTXANIMMESH;

typedef struct tagVertexPosInstanceParticleDesc
{
	static constexpr _uint					iNumElements = { 6 };
	static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
}VTX_POS_INSTANCE_PARTICLE;


typedef struct tagVertexModelInstanceParticleDesc
{
	static constexpr _uint					iNumElements = { 10 };
	static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
}VTX_MODEL_INSTANCE_PARTICLE;

#pragma endregion


typedef struct tagPhsXUserData {

	PHYSX_KIND			eKind = PHYSX_KIND::NOT_DEFINED;
	class CGameObject*	pOwner = { nullptr };
	_float4x4			BeforeMatrix = { };
	_uint				iSubKind = UINT_MAX;

	union {
		class CRigidBody* pBody;
		class CCharacter_Controller* pCharacter;
		class CObstacle_Controller* pObstacle = { nullptr };
	};

}PhsXUserData;

NS_END
