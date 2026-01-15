#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Model_Instance;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CInstancedProp final : public CGameObject
{
public:
	typedef struct tagInstancedPropDesc
	{
		SHADER_PASS_WORLDMODLE_INSTANCE eShaderPass;
		_bool							isShake;
		_bool							bEnableRigidbody;
		_bool							isTree;
		_float2							vRadius;
		_float2							vSpeed;
		_wstring 						strPrototypeTag;
		_string 						strInstanceDataPath;
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
	virtual HRESULT Render_Shadow(SHADOW eType)override;

private:
	_bool								m_isShake = {};
	_bool								m_isTree = {};
	_bool								m_bEnableRigidbody = {};
	_uint								m_iNumMesh = {};
	_uint								m_iNumInstacne = {};
	SHADER_PASS_WORLDMODLE_INSTANCE		m_eShaderPass = {};

	CShader*							m_pShaderCom = { nullptr };
	CVIBuffer_Model_Instance*			m_pVIBufferInstanceCom = { nullptr };

	vector<CRigidBody_Static*>			m_RigidBody = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

	HRESULT	ReadyForPhysX();
	HRESULT Load_InstancedProp(const _char* pFilePath, INSTANCE_PROP_DESC* pDesc);

public:
	static	CInstancedProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
