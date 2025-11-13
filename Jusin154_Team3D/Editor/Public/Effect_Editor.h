#pragma once

#include "Editor_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CEffect_Editor final : public CContainerObject
{
private:
	CEffect_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Editor(const CEffect_Editor& rhs);
	virtual ~CEffect_Editor() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	void			Reference_Mat_For_EditEffect(class CComponent** pTexture, CGameObject* pObject);

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
private:
	void			ReadMaterials(const char* pDirectoryName);
	void            ReadMaterial(_wstring wstrFileName, const char* pFilePath);

private:
	_string												 m_strCurrentEffectName = {};
	_uint												 m_iNumPart = { 0 };
	class CEditEffect*									 m_pEditEffect = { nullptr };
	map<_wstring, vector<pair<_wstring, _wstring>>>		 m_MatFiles = {}; // map<머테리얼 이름, vector<텍스쳐이름 , 텍스쳐 경로>>

public:
	static CEffect_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;


};

NS_END