#pragma once
NS_BEGIN(Engine)
	class CModel;
	class CRigidBody_Static;
	class CUnit;
NS_END

namespace Client
{
	typedef struct tagFolderLoad {
		_wstring							pModelTag;
		CModel* pLoadedModel;
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
		class CMapElement_Chest* pChest = { nullptr };
	}LOCKON_INFO;

	typedef struct tagUnitInfo
	{
		_wstring		pUnit_Name;
		_float			fCurrentHp{};
		_float			fMaxHp{};
		_float			fTargetHp{};
		_float			fMelee{};
		_float			fMagic{};
		_float			fDefense{};
		_float			fSpeed{};
		_float			fAgility{};
		_int			iLevel{};
		_float			fExprince{};
		_float			fMaxExprience{};
		_int			iGold{};
	}UNITINFO;

	typedef struct tagDamageInfo
	{
		_vector		vTarget_Pos{};
		_float		fDamage{};
		DAMAGE_TYPE eType = DAMAGE_TYPE::NOMAL;
	}DAMAGE_INFO;

	typedef struct tagObjectiveInfo
	{
		_bool	bClear{};
		_int	iTargetID{};
		_int	iRequiredCount{};
		_int	iCurrentCount{};
	}OBJECTIVEINFO;

	typedef struct tagRewardsInfo
	{
		_int iRewardType{};
		_int iRewardID{};
	}REWARDSINFO;

	typedef struct tagQuestInfo
	{
		_int					iQuestID{};
		_int					iType{};
		_wstring				pQuestName;
		_wstring				pQuestInfo;
		vector<OBJECTIVEINFO>	ObjectiveInfo;
		vector<REWARDSINFO>		RewardsInfo;
		_int					iAcceptState;
	}QUESTINFO;

	typedef struct tagSpellLearnInfo
	{
		_wstring		pSpellName{};
		_wstring		pImageName{};
		_int			iSpellID{};
		_int			iSpellType{};
		_float			fSpellSize{};
		_float2			fStartPosition{};
		_float2			fEndPosition{};
		vector<_vector> Booster;
		vector<_vector> Lines;
	}SPELLLEARNINFO;

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
		_bool       isPadding0 = {};
		_bool       isPadding1 = {};
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

		_float3		vDissolveValue = {};

		_float      fModelDistortIntensity = {};
		_float		vPadding0 = {};

		_float      fLightDeley = {};
		_float		fLightIntensity = {};
		_float		isLightTime = { 0.f };
		_bool		isLightDissolve = {};

		_bool		isPadding2 = {};

		_float4     vDissolveColor = {};
		_float2     vDissolveColorCut = {};

		_float		fEmissiveColorCut = {};

		_float      fRimLightPower = {};
		_float      fRimLightStrength = {};

		_float4     vNoiseColor = {};
		_float4     vRimLightColor = {};

		_float2     vMaskOffset = {};
		_float2     vPadding4 = {};
		_float4     vPadding5 = {};

		_bool		isRimLight = {};
		_bool		isPadding3 = {};
		_bool		isPadding4 = {};
		_bool		isPadding5 = {};

	}EFFECT_INFO;
	
	typedef struct tagTrailInfo
	{
		EFFECT_TYPE eEffectType = { EFFECT_TYPE::TRAIL };
		/* 디퓨즈 */
		_bool		isDiffuse = {};
		_float		fDiffuseAlpha = { 1.f };
		_float4		vColor = { 0.f ,0.f ,0.f ,1.f };

		/* 마스크 */
		_bool       isMask = {};
		_float		fSoftMaskEdge = {};
		_float		fSoftMask = {};

		/* 노이즈 */
		_bool       isNoise = {};
		_bool		isNoiseColor = { false };
		_bool		isNoiseAlpha = { false };
		_float      fNoiseStrength = {};

		/* 디스 토션 */
		_bool		isDistortion = {};
		_float2     vDistortionTime = {};
		_float2		vDiffuseDistortioUVAmount = {};
		_float2		vMaskDistortionUVAmount = {};
		_float		fDistortionIntensity = {};

		/* 이미시브 */
		_float4		vEmissive = { 0.f ,0.f ,0.f ,0.f };

		_float		fEmissiveStrength = { 0.f };
		_float		fSoftenExp = { 1.31429f };
		_float		fSoftStrength = {};
		_float		fCoreBoost = {};
		_float		fRadius = {};


		/* 블러 */
		_bool		isBlur = {};
		_bool		isOnlyBlur = { false };
		_float		fBlurIntensity = {};
		_int		iBlurWeight = {};

		/* 블룸 */
		_bool       isBloom = {};

		_bool       isBloomDissolve = {};
		_bool       isBloomReverseDissolve = {};

		_float		fBloomStrength = {};

		BLOOM_TYPE  eBloomType = {};
		_float2     vBloomTime = {};

		_int	    iNumVertex = { 256 };


		RENDER		eRenderOrder = { RENDER::EFFECT };
		SHADER_PASS_POSTEX eShaderPass = { SHADER_PASS_POSTEX::TRAIL };

		_float      fDamping = { 0.5f };
		_float      fRopeLength = { 0.1f };
		_float		fMass = { 0.5f };

		_float2 vDissolveTime = {};

		_float  fPadding0 = {};
		_float3 vPadding2 = {};

		_float  fPadding1 = {};
		_float  fPadding2 = {};
		_float  fPadding3 = {};

		_bool   isDissolve = {};
		_bool   isPadding1 = {};
		_bool   isPadding2 = {};

	}TRAIL_INFO;
};
