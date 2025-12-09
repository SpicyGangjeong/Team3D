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
	typedef struct tagLockOnInfo {
		CUnit* pUnit = { nullptr };
		class CMapElement_Interactable* pInteractive = { nullptr };
	}LOCKON_INFO;
};
