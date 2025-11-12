#pragma once

#include "Editor_Define.h"
#include "ContainerObject.h"
NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Editor)

class CRootModelPart final : public CContainerObject
{
public:
	enum class PARTSTYPE{HEAD,BODY,HAIR,END};
public:
	void Set_PrototypeModelName(const _wchar* PrototypeModelName) { m_PrototypeModelName = PrototypeModelName; }
	void Set_ModelName(const _char* Name) { m_ModelName = Name; }
	const _wchar* Get_PrototypeModelName() { return m_PrototypeModelName; }
	const _char* Get_ModelName() { return m_ModelName; }
	class CModelParts* Get_ModelParts(_uint iIndex) { return m_ModelParts[iIndex]; }
	class CModel* Get_MainModel() { return m_pMainModel; }
	_uint Get_Index() { return m_iAnimIndex; }

private:
	CRootModelPart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRootModelPart(const CRootModelPart& Prototype);
	virtual ~CRootModelPart() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Update_Anim(_float fTimeDelta);
	void Set_MainModel(class CModel* Model);
	void Change_Model(_uint iIndex);
	void Set_Animation(_uint iAnimIndex);

private:
	class CModel*				m_pMainModel = { nullptr };
	vector<class CModelParts*>	m_ModelParts = {};
	const _wchar*				m_PrototypeModelName = { 0 };
	const _char*				m_ModelName = { 0 };
	_uint						m_iAnimIndex = { 0 };
	_float						m_fTrackPosition = {};

private:
	virtual HRESULT Ready_Components(void*pArg);
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	static CRootModelPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END