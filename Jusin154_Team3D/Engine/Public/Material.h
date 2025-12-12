#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:

	HRESULT UnbindAllMaterialTextures(class CShader* shader);
	HRESULT Bind_SRV(class CShader* pShader, MODEL eType = MODEL::END);
	_int Get_UsingPass() { return m_iUsingPass; }
	void Set_UsingPass(_int iPass) { m_iUsingPass = iPass; }
	_int Get_OutLinePass() { return m_iOutLinePass; }
	void Set_OutLinePass(_int iPass) { m_iOutLinePass = iPass; }
	
private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	vector<ID3D11ShaderResourceView*>		m_SRVs[AI_TEXTURE_TYPE_MAX];
	_int		m_iUsingPass = { -1 };
	_int		m_iOutLinePass = { -1 };

#ifdef EDITOR_PROJECT
public:
	SaveMaterial Get_SaveMaterial() { return m_SaveMaterial; }

	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pMaterialFilePath, const _char* pTextureFilePath);
private:
	SaveMaterial							m_SaveMaterial;

	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	HRESULT Initialize(const _char* pModelFilePath, MODEL eType, const aiMaterial* pAIMaterial);
	HRESULT Initialize(const _char* pMaterialFilePath, const _char* pTextureFilePath);
	HRESULT Read_MaterialFile(const _char* pMaterialFilePath, const _char* pTextureFolderPath);
	HRESULT Add_Texture(const _char* pTextureFolderPath, string& FileType);
#endif // EDITOR_PROJECT

	// 바이너리
	HRESULT Initialize(const _char* pModelFilePath, const SaveMaterial& _SaveMaterial);
	//

public:
	// 바이너리
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePathconst, const SaveMaterial& _SaveMaterial);
	//
	virtual void Free() override;
};

NS_END
