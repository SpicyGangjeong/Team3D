#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Model_Instance;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Editor)

class CInstancedProp final : public CGameObject
{
public:
	typedef struct tagInstancedPropDesc
	{
		_bool		bEditMode{};
		_wstring 	strPrototypeTag;
		string 		strInstanceDataPath;
	}INSTANCE_PROP_DESC;
private:
	CInstancedProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancedProp(const CInstancedProp& rhs);
	virtual ~CInstancedProp() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool						m_bEditMode = {};
	CVIBuffer_Model_Instance*	m_pVIBufferInstanceCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

	vector<_float4x4>			m_WorldMatrices = {};
	_float4x4 					m_WorldMatrixIdentity = {};
	_wstring 					m_strPrototypeTag;

	_float4						m_vPosition = {};
	_float3						m_vRotation = {};
	_float3						m_vScale = {};
#ifdef _DEBUG
	_char						m_szFileName[MAX_PATH] = {};

#endif // _DEBUG


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	void Add_WorldMatrix();
	void Load_InstancedProp(const _char* pFilePath);
	void Patch_Data(const _char* pFilePath);

#ifdef _DEBUG
	void Save_InstancedProp(const _char* pFilePath);
#endif // _DEBUG

public:
	static	CInstancedProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;

};

NS_END
