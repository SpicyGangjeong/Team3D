#pragma once

NS_BEGIN(Engine)

typedef struct tagEngineDesc
{
	HINSTANCE			hInstance;
	HWND				hWnd;
	_uint				iNumLevels;
	_uint				iNumCollidableGroup;
	_uint				iStaticLevel;
	_uint				iWinSizeX, iWinSizeY;
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
	XMFLOAT3		vScale;
	XMFLOAT4		vRotation;
	XMFLOAT3		vTranslation;
	float			fTrackPosition;

}KEYFRAME;

struct LERPDESC {
	_int iSlot;
	tagKeyFrame tagKeyFrame;
};

typedef struct tagLightDesc
{
	LIGHT		eType;
	_float4		vDiffuse;
	_float4		vAmbient;
	_float4		vSpecular;

	_float4		vDirection;
	_float4		vPosition;
	_float		fRange;
	_float2		vSpotAngles;
}LIGHT_DESC;

typedef struct tagShadowLight
{
	_float4		vEye, vAt;
	_float		fWidth, fHeight, fNear, fFar;
}SHADOW_LIGHT_DESC;

typedef struct tagVertexInstance_Particle
{
	_float4			vRight;
	_float4			vUp;
	_float4			vLook;
	_float4			vTranslation;

	_float2			vLifeTime;
}VTX_INSTANCE_PARTICLE;

typedef struct tagVertexBlock
{
	_float3 vPosition;
	_float3 vNormal;
	_float3 vTexcoord;

	static constexpr _uint iNumElements = { 3 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXBLOCK;

typedef struct tagVertexPosition {
	_float3	vPosition;
	
	static constexpr _uint iNumElements = { 1 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXPOS;

typedef struct tagVertexPositionTexcoord {
	_float3	vPosition;
	_float2	vTexcoord;
	
	static constexpr _uint iNumElements = { 2 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXPOSTEX;

typedef struct tagVertexPositionNormalTexcoord {
	_float3	vPosition;
	_float3	vNormal;
	_float2	vTexcoord;
	
	static constexpr _uint iNumElements = { 3 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

}VTXNORTEX;

typedef struct tagVertexMesh
{
	_float3			vPosition;
	_float3			vNormal;
	_float3			vTangent;
	_float3			vBinormal;
	_float2			vTexcoord;

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
	_float3			vPosition;
	_float3			vNormal;
	_float3			vTangent;
	_float3			vBinormal;
	_float2			vTexcoord;

	XMUINT4			vBlendIndex;
	_float4			vBlendWeight;

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
	static constexpr unsigned int					iNumElements = { 6 };
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
	static constexpr unsigned int					iNumElements = { 10 };
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

NS_END