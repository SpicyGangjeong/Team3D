#pragma once

#include "Editor_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Loading();
	void Output();

public:
	_bool isFinished() const {
		return m_isFinished;
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_eNextLevelID = { LEVEL::END };

	_wstring			m_strMessage = {};
	_bool				m_isFinished = { false };


	CGameInstance* m_pGameInstance = { nullptr };

	//list<SaveModel> m_SaveModel;

private:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_ObjectViewer();
	HRESULT Loading_For_MapViewer();
	HRESULT Loading_For_UI();
	HRESULT Loading_For_Effect();
	HRESULT Loading_For_PhysXLevel();
	HRESULT Loading_For_Bloom();

private:
	HRESULT Asset_FileLoad(const _char* pDirectoryPath, const _tchar* pPreName, function<HRESULT(_wstring, const _char*)> AddPrototypeEvent);
	HRESULT MapFolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag, vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePath);
	future<void> Deferred_FolderLoad(const _char* pDirectoryPath, const _char* pFileExt, _bool bUseTag, vector<FOLDER_LOAD*>** OutContents);
	future<pair<_wstring, CShader*>*> Deferred_ShaderLoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strPrototypeTag, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	future<pair<_wstring, CModel*>*> Deferred_ModelLoad(MODEL eType, const _char* pDirectoryPath, _fmatrix PreTransform, const _tchar* pPrototypeTag);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

NS_END
