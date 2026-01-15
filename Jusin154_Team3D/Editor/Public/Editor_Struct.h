#pragma once

NS_BEGIN(Engine)
class CModel;
class CRigidBody_Static;
class CUnit;
class CTransform;
NS_END


NS_BEGIN(Editor)

typedef struct tagMapObject_Desc {
	_uint					iRenderType{};
	CTransform*				pParentTransform = { nullptr };
	_float3					vScale;
	_float3					vRotation;
	_float3					vPosition;
}MAPOBJECT;

typedef struct tagMapObjectStaticDesc : public tagMapObject_Desc
{
	_uint					iMaxLodLevel{};
	_wstring				strModelPrototypeTag;
	_uint					iModelPathIndex = { UINT_MAX };
}MAPOBJECT_STATIC_DESC;

typedef struct tagMapObjectLodDesc : public tagMapObject_Desc
{
	_uint					iMaxLodLevel{};
	vector<_wstring>		ModelPrototypeTags;
	vector<_uint>*			pModelPathIndices = { nullptr };
}MAPOBJECT_LOD_DESC;


typedef struct tagFolderLoad {
	_wstring							pModelTag;
	CModel*								pLoadedModel;
	filesystem::path					pathModel;

	_bool								bLoadTags;
	vector<_wstring>					pRigidBodyTags;
	vector<CRigidBody_Static*>			LoadedRigidBody;
}FOLDER_LOAD;

typedef struct tagSpellInfo
{
	_int			iSpell_ID{};
	_wstring		pSpell_Name;
	_wstring		pImage_Name;
	_int			iSpell_Type{};
	_wstring		pType_Name;
	_int			iSkill_Type{};
	_float			fSpell_Damage{};
	_float			fSpell_CoolTime{};
	_float			fDuration{};
	_int			iAnimNum{};
	_wstring		pSpellInfo;
	_bool			bSpell_Lock = false;
	_bool			bEquip_Spell = false;
	_bool			bUse_Skill = false;
	_float			fPreview{};
	_float			fVidio{};
}SPELL_INFO;

typedef struct tagLockOnInfo {
	CUnit* pUnit = { nullptr };
	class CMapElement_Interactable* pInteractive = { nullptr };
}LOCKON_INFO;

typedef struct tagEffectInfo
{
	_float4 vColor = { 0.f ,0.f ,0.f ,1.f };

	_float2 vDiffuseUVGainAmount = {};
	_float2 vMaskingUVGainAmount = {};
	_float2 vNoiseUVGainAmount = {};

	_float2 vDistortionTime = {};
	_float2 vMaskDistortionUVGainAmount = {};

	_float2 vUVCutting = { 1.f ,1.f };
	_float2 vUVMaskCutting = { 1.f ,1.f };

	_int    iMaskMoveLerpOption = { 0 };
	_int    iDiffuseMoveLerpOption = { 0 };
	_int    iNoiseMoveLerpOption = { 0 };

	_int    iMaskDistortionMoveLerpOption = { 0 };
	_int    iDiffuseDistortionMoveLerpOption = { 0 };

	_int	iBlurWeight = { 0 };


	_bool	isDiffuse = {};
	_bool	isMasking = {};
	_bool	isDissolve = {};
	_bool	isNoise = {};
	_bool   isEmissive = {};
	_bool   isDistortion = {};
	_bool	isDiffuseUVMove = {};
	_bool   isNoiseUVMove = {};
	_bool	isMaskUVMove = {};
	_bool   isBlur = {};
	_bool   isBillboard = {};

	_float4 vEmissive = { 0.f ,0.f ,0.f ,0.f };
	_float  fDiffuseAlpha = { 1.f };
	_float  fBlurIntensity = {};
	_float  fNoiseDistortionIntensity = {};
	_float  fEmissiveStrength = { 0.f };

	LIGHT_DESC	LightDesc = {};
	RENDER		eRenderOrder = { RENDER::EFFECT };

	_bool   isReverseDissolve = {};

	EFFECT_TYPE eEffectType = { EFFECT_TYPE::EFFECT };

	_bool   isEmissiveDissolve = { false };

	_bool   isMaskClampSample = { false };
	_bool   isNoiseColor = { false };
	_bool   isNoiseAlpha = { false };
	_bool	isNomalDissolve = {};

	_float fSoftenExp = { 1.31429f };
	_float fSoftStrength = {};
	_float fCoreBoost = {};
	_float fRadius = {};

	_float fSoftMaskEdge = {};
	_float fSoftMask = {};

	_bool   isEmissiveDissolveReverse = { false };
	_bool   isOnlyBlur = { false };

	SHADER_PASS_INSTANCE_MODEL eShaderPass = { SHADER_PASS_INSTANCE_MODEL::NON_NOMALMAP };

	_bool       isBlurNoEmissive = {};


	_bool	    isMotionBlur = {};

	_bool       isNoise_G = {};
	_bool       isNoise_B = {};

	_float      fModelBlurIntensity = { 0.f };

	_bool       isBloom = {};
	_bool       isBloomDissolve = {};
	_bool       isBloomReverseDissolve = {};
	_float      fBloomStrength = {};
	BLOOM_TYPE  eBloomType = {};


	_float		fDissolveDelay = {};
	_float		fReverseDissolveDelay = {};
	_float2		vDissolveUVGainAmount = {};
	_bool		isDissolveMove = {};

	_float3		vDissolveValue = {}; /* Soft Mask , Cut Ratio*/

	_float      fModelDistortIntensity = {};
	_float		fPadding0 = {};

	_float      fLightDeley = {};
	_float		fLightIntensity = {};
	_float		isLightTime = { 0.f };
	_bool		isLightDissolve = {};

	_bool		isDissolve_G = {};

	_float4     vBlurColor = {};
	_float2     vUVNoiseCutting = { 1.f ,1.f };

	_float		fEmissiveColorCut = {};

	_float      fRimLightPower = {};
	_float      fRimLightStrength = {};

	_float4     vNoiseColor = {};
	_float4     vRimLightColor = {};

	_float2     vMaskOffset = {};

	_bool       isDiffuse_R = {};
	_bool       isDiffuse_G = {};
	_bool       isDiffuse_B = {};
	_bool       isBlurColor = {};

	_bool       isBlurDissolve = {};
	_bool       isNomalMap = {};
	_bool       isNoDissolveSmoothStep = {};
	_bool       isNonSoftEffect = {};

	_float2     vDissolveSmoothRange = { 0.1f , 0.1f };

	_float      fPadding1 = {};

	_bool		isScreenFX = {};
	_bool       isPadding0 = {};
	_bool       isPadding1 = {};
	_bool       isPadding2 = {};

	_bool		isRimLight = {};
	_bool		isMask_G = {};
	_bool		isMask_B = {};
	_bool		isDissolve_B = {};

	_float4     vPadding1 = {};
	_float4     vPadding2 = {};
	_float4     vPadding3 = {};
	_float4     vPadding4 = {};
	_float4     vPadding5 = {};
	_float4     vPadding6 = {};
}EFFECT_INFO;

NS_END
