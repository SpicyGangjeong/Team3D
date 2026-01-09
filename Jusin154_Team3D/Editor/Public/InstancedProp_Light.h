#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Model_Instance;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Editor)

class CInstancedProp_Light final : public CGameObject
{
public:
	typedef struct tagInstancedPropLightDesc
	{
		_bool		bEditMode{};
		_bool		isShake;
		_uint 		iGlassMeshIndex{};
		_float2		vRadius;
		_float2		vSpeed;
		_wstring 	strPrototypeTag;
		string 		strInstanceDataPath;
	}INSTANCE_PROP_LIGHT_DESC;
private:
	CInstancedProp_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancedProp_Light(const CInstancedProp_Light& rhs);
	virtual ~CInstancedProp_Light() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Bloom() override;
	

private:
	_bool						m_bEditMode = {};
	_bool						m_bLightOn = {true};
	CVIBuffer_Model_Instance*	m_pVIBufferInstanceCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

	vector<_float4x4>			m_WorldMatrices = {};
	_float4x4 					m_WorldMatrixIdentity = {};
	_wstring 					m_strPrototypeTag;

	_float4						m_vPosition = {};
	_float3						m_vRotation = {};
	_float3						m_vScale = {};

	_uint 						m_iGlassMeshIndex = {};
	_float 						m_fGlassRatio = {0.5f};
	_float						m_fBloomStrength = { 15.f };

	CTexture*					m_pGlassTextureCom = { nullptr };
	CTexture*					m_pMaskTextureCom = { nullptr };
	CTexture*					m_pEmissiveTextureCom = { nullptr };
#ifdef _DEBUG
	_uint						m_iInstanceIndex = {};
	_uint						m_iNumInstanceIndex = {};
	_char						m_szFileName[MAX_PATH] = {};
#endif // _DEBUG


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	void Add_WorldMatrix();
	void Load_InstancedProp(const _char* pFilePath);
	void Load_InstancedPropFromXML(const _char* pFilePath);
	void Patch_Data(const _char* pFilePath);



#ifdef _DEBUG
	void Save_InstancedProp(const _char* pFilePath);
#endif // _DEBUG

public:
	static	CInstancedProp_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;

};

NS_END
