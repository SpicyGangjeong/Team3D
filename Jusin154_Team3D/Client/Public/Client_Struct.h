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
};
