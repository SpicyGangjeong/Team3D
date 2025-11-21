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

typedef struct tagOnCollsionInfo 
{
	_vector vWorldPos = {};		// 접촉지점
	_vector vWorldNomal = {};	// 접촉노말
	_vector vHitDir = {};		// 시도한 move 방향
	_float  fLength = {};		// 작용된 힘

}ON_COLLISION_INFO;

typedef struct tagFolderLoad {
	_wstring							pModelTag;
	CModel*								pLoadedModel;
	filesystem::path					pathModel;

	vector<const _tchar*>				pRigidBodyTags;
	vector<CRigidBody_Static*>			LoadedRigidBody;
}FOLDER_LOAD;


NS_END
