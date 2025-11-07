#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final :
	public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;


public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName, _uint iOffset, _uint iCount);
	vector<ID3D11ShaderResourceView*>* Get_Texture() { return &m_SRVs; }
	ID3D11ShaderResourceView* Get_SRV(_uint iTextureIndex);
	_uint Get_Size();

private:
	HRESULT Initialize_Prototype(TEXTURE_LOAD_TYPE eType, const _tchar* pTextureFilePath, const _wstring& wstrTextureKey, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Load_SRV(const _char* szPath, ID3D11ShaderResourceView** ppSRV);
	HRESULT ParseTextureIncrementalToSRVs(_uint iNumTextures, const _tchar* pTextureFilePath);
	HRESULT ParseTexturePathToSRVs(const _tchar* pTextureFolderPath);

private:
	vector<ID3D11ShaderResourceView*>				m_pSRVs = { };

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TEXTURE_LOAD_TYPE eType, const _tchar* pTextureFilePath, const _wstring& wstrTextureKey, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END