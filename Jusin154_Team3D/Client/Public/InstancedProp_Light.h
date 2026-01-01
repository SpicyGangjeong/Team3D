#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Model_Instance;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CInstancedProp_Light final : public CGameObject
{
public:
	typedef struct tagInstancedPropLightDesc
	{
		_bool		isShake;
		_uint 		iGlassMeshIndex{};
		_float2		vRadius;
		_float2		vSpeed;
		_wstring 	strPrototypeTag;
		_string 		strInstanceDataPath;
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
	_bool						m_bLightOn = { true };
	CVIBuffer_Model_Instance*	m_pVIBufferInstanceCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

	CTexture*					m_pGlassTextureCom = { nullptr };
	CTexture*					m_pMaskTextureCom = { nullptr };
	CTexture*					m_pEmissiveTextureCom = { nullptr };

	_uint						m_iNumMesh = {};
	_uint 						m_iGlassMeshIndex = {};

	_float 						m_fGlassRatio = { 0.5f };
	_float						m_fBloomStrength = { 15.f };

	

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	HRESULT Load_InstancedProp(const _char* pFilePath);

public:
	static	CInstancedProp_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
