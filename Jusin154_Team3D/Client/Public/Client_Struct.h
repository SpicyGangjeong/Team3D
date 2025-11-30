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
		_uint		iSpell_ID{};
		_wstring	pSpell_Name{};
		_uint		iSpell_Type{};
		_uint		iSpell_CoolTime{};
		_bool		bSpell_Lock = false;
		_bool		bEquip_Spell = false;

	}SPELLINFO;
};
