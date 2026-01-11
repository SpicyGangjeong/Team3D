#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)
class CInstancedProp;
class CLand final : public CGameObject
{
public:
	typedef struct tagLandDesc
	{
		_float3		vPosition;
		_float3		vScale;
		_wstring	strAlphaMapTag;
		_wstring	strModelComTag;
	}LAND_DESC;
private:
	CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLand(const CLand& rhs);
	virtual ~CLand() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;

	void	Add_InstanceProp(CInstancedProp* pIntanceProp);
private:
	_float				m_fRaduis = {};
	_float				m_fUsingSurfaceParams = {};

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	CTexture*			m_pDiffuseTextureCom = { nullptr };
	CTexture*			m_pNormalTextureCom = { nullptr };
	CTexture*			m_pMROTextureCom = { nullptr };
	CTexture*			m_pMaskTextureCom = { nullptr };

	list<class CInstancedProp*>  m_InstanceProps = {};
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CLand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
