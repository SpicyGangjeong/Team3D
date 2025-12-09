#pragma once
NS_BEGIN(Engine)
class CTransform;
class CModel;
class CRigidBody_Static;
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
NS_END
