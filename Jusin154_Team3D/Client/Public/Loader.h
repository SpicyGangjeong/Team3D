#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CModel;
class CVIBuffer_Model_Instance;
NS_END

NS_BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const { return m_isFinished; }
private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	LEVEL					m_eNextLevelID = { LEVEL::END };

	_wstring				m_strMessage = {};
	_bool					m_isFinished = { false };

private:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_For_Field();
	HRESULT Loading_For_Restart();

private:
	HRESULT Asset_FileLoad(const _char* pDirectoryPath, const _tchar* pPreName, function<HRESULT(_wstring, const _char*)> AddPrototypeEvent);
	future<vector<FOLDER_LOAD*>*> Deferred_FolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag);
	future<pair<_wstring, CModel*>*> Deferred_ModelLoad(MODEL eType, const _char* pDirectoryPath, _fmatrix PreTransform, const _tchar* pPrototypeTag);
	void Ready_MapModels(vector<future<vector<FOLDER_LOAD*>*>>& jobMapModels);
	HRESULT Ready_RigidBody_Static(CVIBuffer_Model_Instance* pModel_Instance);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END
