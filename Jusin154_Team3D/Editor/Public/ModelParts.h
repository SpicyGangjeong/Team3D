#pragma once

#include "Editor_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CModelParts abstract : public CPartObject
{
public:
	typedef struct tagPartsObjectDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _tchar* pModelPrototypeTag;
	}PARTS_OBJECT_DESC;

protected:
	CModelParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelParts(const CModelParts& Prototype);
	virtual ~CModelParts() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Change_Model(const _wchar* Prototype);
	_wstring& Get_PrototypeTag() { return m_strModelPrototypeTag; }

protected:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool		m_bRimLight = { true };
	_float		m_fRimLightPower = { 3.2f };
	_float		m_fRimLightStrength = { 3.04f };
	_float3		m_vRimLightColor = { 69.f / 255.f, 5.f / 255.f, 10.f / 255.f };

	_wstring	m_strModelPrototypeTag;
protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)PURE;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END