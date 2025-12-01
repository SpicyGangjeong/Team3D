#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CMapObject_Manager final : public CGameObject
{
	enum class ADD_TYPE {CONTAINER, ELEMENT_STATIC, ELEMENT_INTERACT, ELEMENT_LIGHT };

private:
	CMapObject_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Manager(const CMapObject_Manager& Prototype);
	virtual ~CMapObject_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype(vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePaths);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	const filesystem::path Get_PrototypePath(_uint iModelIndex);

private:
	ADD_TYPE							m_eType = {};

	_bool								m_bModelVisable = {true};
	_bool								m_bCollisionVisable = { true };

	class CMapObject*					m_pSelectObject = { nullptr };
	class CMapElement*					m_pSelectElemnt = { nullptr };
	list<class CMapObject*>				m_MapObjects = {};
	list<class CMapObject*>				m_Collision = {};
	vector<_wstring>					m_ModelPrototypeTags;
	map<_wstring, _uint>				m_PartObjectKeyCount;
	vector<_wstring>					m_LODModelPrototypeTags;
	vector<filesystem::path>			m_ModelPrototypePaths;

	_uint								m_iContainerObjectIndex = {};

	class CBuildingContainer*			m_pContainer = { nullptr };
	_char								m_szSaveFileName[MAX_PATH] = {};
	_char								m_szSaveContainerName[MAX_PATH] = {};
private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();

#pragma region SAVE_LOAD
	HRESULT		Save_MapData(const _char* pFileName);
	HRESULT		Load_MapData(const _char* pFileName);

	HRESULT		Save_ContainerData(const _char* pFileName, const _char* pContainerName);
	HRESULT		Load_ContainerData(const _char* pFileName, const _char* pContainerName);
	HRESULT		Load_ContainerToMapObject(const _char* pFileName, const _char* pContainerName);

	HRESULT		Save_LightObject(const _char* pFileName);
#pragma endregion

	void		Update_PrototypeList();
	void		Update_ObjectList();
	void		Update_Edit();
	void		Update_ContainerObject();

	void		Create_PartObject(_wstring& strPrototypeTag);
	void		Create_Elemnt(_wstring& strPrototypeTag);

	_bool		Find_Lod_Prototype(_wstring strPrototypeTag, vector<_uint>& LodModelIndices);
	_uint		Get_KeyCount(_wstring strPrototypeTag);
public:
	static CMapObject_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<_wstring>& ModelPrototypeTags, vector<filesystem::path>& ModelPrototypePaths);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
