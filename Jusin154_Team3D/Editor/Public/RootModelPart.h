#pragma once

#include "Editor_Define.h"
#include "GameObject.h"
NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Editor)

class CRootModelPart final : public CGameObject
{
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
	void Push_ModelParts(class CModelParts* ModelParts);
	void Push_MainModel(CModel* MainModel);
	void Set_AnimIndex(_uint iIndex) { m_iAnimIndex = iIndex; }

private:
	CModel* m_pMainModel = { nullptr };
	vector<class CModelParts*> m_ModelParts = {};
	_uint m_iAnimIndex = {0};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRootModelPart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END