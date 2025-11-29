#pragma once

#include "Editor_Define.h"
#include "Effect_Container.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CEffect_Editor final : public CEffect_Container
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
	_string			Reference_Mat_For_EditEffect(class CComponent** pTexture, CGameObject* pObject);
	HRESULT			Load_Edit(const _char* pPath);

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT         Ready_Child();
	HRESULT			Bind_ShaderResources() override;
private:
	HRESULT         Packaging(const _char* pDirectoryPath);
	HRESULT			ReSaveFile(const _char* pDirectoryPath);
	void			ReadMaterials(const char* pDirectoryName);
	void            ReadMaterial(_wstring wstrFileName, const char* pFilePath);

private:
	_string												 m_strCurrentEffectName = {};

	_uint												 m_iNumPart = { 0 };
	class CEditEffect*									 m_pEditEffect = { nullptr }; // 현재 선택된 이펙트 객체
	class CTrailObject*									 m_pTrailObject = { nullptr };
	map<_wstring, vector<pair<_wstring, _wstring>>>		 m_MatFiles = {}; // map<머테리얼 이름, vector<텍스쳐이름 , 텍스쳐 경로>>

	_char												 m_szBuffer[MAX_PATH] = { "../Bin/Resources/Data/Effect/"};
	_char												 m_szTrailBuffer[MAX_PATH] = { "../Bin/Resources/Data/Effect/Trail/" };
	_char												 m_szPackageBuffer[MAX_PATH] = { "../Bin/Resources/Data/Effect/" };

	_string												 m_strSavePath = {};
	_string												 m_strTrailSavePath = {};
	_string												 m_strPackageSavePath = {};

public:
	static CEffect_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;


};

NS_END
