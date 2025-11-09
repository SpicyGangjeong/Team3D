#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Bind_SRV(class CShader* pShader, const _char* pConstantName, _uint iType, _uint iTextureIndex);
	SaveMaterial Get_SaveMaterial() { return m_SaveMaterial; }
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<ID3D11ShaderResourceView*>		m_SRVs[AI_TEXTURE_TYPE_MAX];
	SaveMaterial							m_SaveMaterial;

#ifdef EDITOR_PROJECT
public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	HRESULT SaveAsBinary(HANDLE hFile, DWORD& dwByte);
private:
	vector<string>	m_strPath[AI_TEXTURE_TYPE_MAX];
	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
#endif // EDITOR_PROJECT

	virtual HRESULT Initialize(const _char* pModelFilePath, HANDLE hFile, DWORD& dwByte);
	// 바이너리
	HRESULT Initialize(const _char* pModelFilePath, const SaveMaterial& _SaveMaterial);
	//

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, HANDLE hFile, DWORD& dwByte);
	// 바이너리
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePathconst, const SaveMaterial& _SaveMaterial);
	//
	virtual void Free() override;
};

NS_END