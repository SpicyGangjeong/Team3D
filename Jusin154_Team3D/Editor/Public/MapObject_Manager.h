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
private:
	CMapObject_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject_Manager(const CMapObject_Manager& Prototype);
	virtual ~CMapObject_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype(vector<_wstring>& ModelPrototypeTags);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CMapObject*					m_pSelectObject = { nullptr };
	list<class CMapObject*>				m_MapObjects = { nullptr };
	vector<_wstring>					m_ModelPrototypeTags;
	vector<_wstring>					m_LODModelPrototypeTags;

	_bool								m_bLOD = { false };
	class CBuildingContainer*			m_pContainer = { nullptr };

private:
	HRESULT		Ready_Components();
	HRESULT		Bind_ShaderResources();
	HRESULT		Save_MapData(const _char* pFileName);
	HRESULT		Load_MapData(const _char* pFileName);

	void		Update_PrototypeList();
	void		Update_ObjectList();
	void		Update_Edit();

public:
	static CMapObject_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<_wstring>& ModelPrototypeTags);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END