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
	_float3		vScale = {};
	_float4		vRotation = {};
	_float3		vTranslation = {};
	_float		fTrackPosition = {};

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

	const _float4* pDirection = nullptr;
	const _float4* pPosition = nullptr;

	_float4				vPosOffset = {};

	_float				fRange = {};
	_float2				vSpotAngles = {};

	_uint				iLevel = {};
}LIGHT_DESC;

struct SaveVertex
{
	_float3 Pos;
	_float3 Normal;
	_float3 Tan;
	_float3 BiNoraml;
	_float2 UV;

	bool bHasNormal;
	bool bHasTan;
	bool bHasUV;

	XMUINT4 BlendIndex;
	_float4 BlendWeight;
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
	vector<string> Path[AI_TEXTURE_TYPE_MAX];
	_float2 vSRV_Flag;
	_float3 vPBR_Flag;
};

struct SaveKeyFrameVec
{
	float Time;
	_float3 Value;
};

struct SaveKeyFrameRotation
{
	float Time;
	_float4 Value;
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

typedef struct tagVertexInstance_Model
{
	_float4			vRight = {};
	_float4			vUp = {};
	_float4			vLook = {};
	_float4			vTranslation = {};
}VTX_INSTANCE_MODEL;

typedef struct tagVertexInstance_Particle
{
	_float4			vRight = {};
	_float4			vUp = {};
	_float4			vLook = {};
	_float4			vTranslation = {};

	_float2			vLifeTime = {};
}VTX_INSTANCE_PARTICLE;

typedef struct tagVertexInstance_UI
{
	_float2 fSize = {};
	_float2 fPos = {};
	_float4 fUV = _float4(0.f, 0.f, 1.f, 1.f);
	_float  vColor = 0;
	_float  bHover = 0;
	_float  bSpell = 0;
}VTX_INSTANCE_UI;

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

typedef struct tagVertexPositionNormal {
	_float3	vPosition = {};
	_float3	vNormal = {};

	static constexpr _uint iNumElements = { 2 };
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

}VTXPOSNOR;

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

typedef struct tagVertexModelInstanceModelDesc
{
	static constexpr _uint					iNumElements = { 9 };
	static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
}VTX_MODEL_INSTANCE_MODEL;

#pragma endregion

typedef struct tagVertexInstance_UIDesc
{
	static constexpr unsigned int iNumElements = { 8 };
	static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},

	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 1, 8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 4, DXGI_FORMAT_R32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 5, DXGI_FORMAT_R32_FLOAT, 1, 36, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "TEXCOORD", 6, DXGI_FORMAT_R32_FLOAT, 1, 40, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

}VTX_POSTEX_INSTANCE_UI;

typedef struct tagPHYSX_USERDATA {

	PHYSX_KIND			eKind = PHYSX_KIND::NOT_DEFINED;
	_bool				bAutoOwnerTranslation = true;

	class CGameObject* pOwner = { nullptr };
	_float4x4			BeforeMatrix = { };
	_uint				iSubKind = UINT_MAX;

	union {
		class CRigidBody* pBody;
		class CCharacter_Controller* pCharacter;
		class CObstacle_Controller* pObstacle = { nullptr };
	};

}PHYSX_USERDATA;

typedef struct tagD6JointDesc
{   // 0: Locked 
	// 1: Limited 
	// 2: Free
	int iMotionX = 0; 
	int iMotionY = 0;
	int iMotionZ = 0;
	int iMotionTwist = 0;
	int iMotionSwing1 = 0;
	int iMotionSwing2 = 0;

	float fLinearLimit = 0.f;
	float fSwingDeg = 0.f;
	float fTwistAbsDeg = 0.f;

	bool  bEnableSlerpDrive = false;
	float fSlerpStiffness = 0.f;
	float fSlerpDamping = 0.f;
	float fSlerpForceLimit = 0.f;
	bool  bSlerpAccel = false;

	bool  bEnableLinearDrive = false;
	float fLinearStiffness = 0.f;
	float fLinearDamping = 0.f;
	float fLinearForceLimit = 0.f;
	bool  bLinearAccel = false;

	tagD6JointDesc() {
		iMotionX = 0;
		iMotionY = 0;
		iMotionZ = 0;
		iMotionTwist = 0;
		iMotionSwing1 = 0;
		iMotionSwing2 = 0;
		fLinearLimit = 0.f;
		fSwingDeg = 0.f;
		fTwistAbsDeg = 0.f;
		bEnableSlerpDrive = false;
		fSlerpStiffness = 0.f;
		fSlerpForceLimit = 0.f;
		fSlerpDamping = 0.f;
		bSlerpAccel = false;
		bEnableLinearDrive = false;
		fLinearStiffness = 0.f;
		fLinearDamping = 0.f;
		bLinearAccel = false;
	}

	static int MotionToIndex(PSX::PxD6Motion::Enum motion)
	{
		if (motion == PSX::PxD6Motion::eLOCKED)  return 0;
		if (motion == PSX::PxD6Motion::eLIMITED) return 1;
		return 2; // eFREE
	}

	static PSX::PxD6Motion::Enum IndexToMotion(int index)
	{
		return (index == 0) ? PSX::PxD6Motion::eLOCKED :
			(index == 1) ? PSX::PxD6Motion::eLIMITED :
			PSX::PxD6Motion::eFREE;
	}

	static bool HasAccelFlag(const PSX::PxD6JointDrive& drive)
	{
		return (drive.flags & PSX::PxD6JointDriveFlag::eACCELERATION) != 0;
	}

	static PSX::PxD6JointDrive MakeDrive(float stiffness, float damping, float forceLimit, bool accel)
	{
		PSX::PxD6JointDrive drive(stiffness, damping, forceLimit, accel);
		return drive;
	}

	void InitFromJoint(const PSX::PxD6Joint& joint)
	{
		iMotionX = MotionToIndex(joint.getMotion(PSX::PxD6Axis::eX));
		iMotionY = MotionToIndex(joint.getMotion(PSX::PxD6Axis::eY));
		iMotionZ = MotionToIndex(joint.getMotion(PSX::PxD6Axis::eZ));
		iMotionTwist = MotionToIndex(joint.getMotion(PSX::PxD6Axis::eTWIST));
		iMotionSwing1 = MotionToIndex(joint.getMotion(PSX::PxD6Axis::eSWING1));
		iMotionSwing2 = MotionToIndex(joint.getMotion(PSX::PxD6Axis::eSWING2));

		{
			PSX::PxJointLinearLimit pxLimit = joint.getLinearLimit();
			fLinearLimit = pxLimit.value;
		}

		{
			PSX::PxJointLimitCone pxSwing = joint.getSwingLimit();
			fSwingDeg = XMConvertToDegrees(pxSwing.yAngle); // 대칭 cone 기준

			PSX::PxJointAngularLimitPair pxTwist = joint.getTwistLimit();
			const float fAbsRad = PSX::PxMax(PSX::PxAbs(pxTwist.lower), PSX::PxAbs(pxTwist.upper));
			fTwistAbsDeg = XMConvertToDegrees(fAbsRad);
		}

		{
			PSX::PxD6JointDrive pxDrive = joint.getDrive(PSX::PxD6Drive::eSLERP);
			bEnableSlerpDrive = (pxDrive.stiffness > 0.f) || (pxDrive.damping > 0.f) || (pxDrive.forceLimit > 0.f);
			fSlerpStiffness = pxDrive.stiffness;
			fSlerpDamping = pxDrive.damping;
			fSlerpForceLimit = pxDrive.forceLimit;
			bSlerpAccel = HasAccelFlag(pxDrive);
		}

		{
			PSX::PxD6JointDrive pxDriveX = joint.getDrive(PSX::PxD6Drive::eX);
			bEnableLinearDrive = (pxDriveX.stiffness > 0.f) || (pxDriveX.damping > 0.f) || (pxDriveX.forceLimit > 0.f);
			fLinearStiffness = pxDriveX.stiffness;
			fLinearDamping = pxDriveX.damping;
			fLinearForceLimit = pxDriveX.forceLimit;
			bLinearAccel = HasAccelFlag(pxDriveX);
		}
	}

	void ApplyToJoint(PSX::PxD6Joint& joint) const
	{
		joint.setMotion(PSX::PxD6Axis::eX, IndexToMotion(iMotionX));
		joint.setMotion(PSX::PxD6Axis::eY, IndexToMotion(iMotionY));
		joint.setMotion(PSX::PxD6Axis::eZ, IndexToMotion(iMotionZ));
		joint.setMotion(PSX::PxD6Axis::eTWIST, IndexToMotion(iMotionTwist));
		joint.setMotion(PSX::PxD6Axis::eSWING1, IndexToMotion(iMotionSwing1));
		joint.setMotion(PSX::PxD6Axis::eSWING2, IndexToMotion(iMotionSwing2));

		{
			PSX::PxJointLinearLimit pxLimit = joint.getLinearLimit();
			pxLimit.value = PSX::PxMax(0.f, fLinearLimit);
			joint.setLinearLimit(pxLimit);
		}

		{
			PSX::PxJointLimitCone pxSwing = joint.getSwingLimit();
			const float swingRad = XMConvertToRadians(PSX::PxClamp(fSwingDeg, 0.f, 180.f));
			pxSwing.yAngle = swingRad;
			pxSwing.zAngle = swingRad;
			joint.setSwingLimit(pxSwing);
		}
		{
			PSX::PxJointAngularLimitPair pxTwist = joint.getTwistLimit();
			const float twistRad = XMConvertToRadians(PSX::PxClamp(fTwistAbsDeg, FLT_EPSILON, 180.f));
			pxTwist.lower = -twistRad;
			pxTwist.upper = +twistRad;
			
			joint.setTwistLimit(pxTwist);
		}

		if (!bEnableSlerpDrive)
		{
			joint.setDrive(PSX::PxD6Drive::eSLERP, MakeDrive(0.f, 0.f, 0.f, bSlerpAccel));
		}
		else
		{
			joint.setDrive(PSX::PxD6Drive::eSLERP, MakeDrive(fSlerpStiffness, fSlerpDamping, fSlerpForceLimit, bSlerpAccel));
		}

		if (!bEnableLinearDrive)
		{
			const PSX::PxD6JointDrive off = MakeDrive(0.f, 0.f, 0.f, bLinearAccel);
			joint.setDrive(PSX::PxD6Drive::eX, off);
			joint.setDrive(PSX::PxD6Drive::eY, off);
			joint.setDrive(PSX::PxD6Drive::eZ, off);
		}
		else
		{
			const PSX::PxD6JointDrive on = MakeDrive(fLinearStiffness, fLinearDamping, fLinearForceLimit, bLinearAccel);
			joint.setDrive(PSX::PxD6Drive::eX, on);
			joint.setDrive(PSX::PxD6Drive::eY, on);
			joint.setDrive(PSX::PxD6Drive::eZ, on);
		}
	}
}D6JOINTDESC;

static D6JOINTDESC MakeD6JointUIState(const PSX::PxD6Joint& joint)
{
	D6JOINTDESC state;
	state.InitFromJoint(joint);
	return state;
}


typedef struct tagOnCollsionInfo
{
	_float4 vWorldPos = {};		// 접촉지점
	_float4 vWorldNomal = {};	// 접촉노말
	_float4 vHitDir = {};		// 시도한 move 방향
	_float  fLength = {};		// 작용된 힘
	_int	eHitType =  ENUM_CLASS(HIT_TYPE::HIT_PROJECTILE); 	// 발사체/적당한 공격/강공격
	_float	fDamage = {}; // 데미지
	class CGameObject* pObject = { nullptr }; // 시전자
}ON_COLLISION_INFO;

typedef struct tagKeyFrameDesc
{
	_float3 vScale;
	_float4 vRotation;
	_float3 vTranslation;
	_float fTrackPosition;

}KEYFRAME_DESC;

typedef struct tagChannelDesc
{
	_uint StartIndex;
	_uint KeyCount;
	_uint BoneIndex;
	//_uint Padding;
}CHANNEL_DESC;

typedef struct tagBoneMatrix
{
	_float4x4 Combined;
	_float4x4 Local;
	_float4x4 LocalCombined;
}BONE_DESC;


typedef struct tagSSAO_Geometry_Hemisphere {
	_float3 SamplePos[SSAO_SAMPLE_NUMBER];
}SSAO_GEOMETRY_HEMISPHERE;

typedef struct tagSSAO_GeometryDirections_RANDOM_REAL {
	_float4 vDir[16];
}SSAO_GEOMETRYDIRECTIONS_RANDOM_REAL;

typedef struct tagPrevVPMatrices {
	_float4x4 PrevViewMatrix;
	_float4x4 PrevProjMatrix;
}PREVVPMATRICES;

typedef struct tagBoolean {
	_bool	b[16] = {};
}_boolean;

NS_END
