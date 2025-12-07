#pragma once
NS_BEGIN(Engine)
	class CModel;
	class CRigidBody_Static;
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
};
