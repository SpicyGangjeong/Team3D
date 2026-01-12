#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CDummyNPC final : public CGameObject
{
public:
	typedef struct tagDummyNpcDesc
	{
		_uint				iID = {};
		_float3				vPosition = {};
		_float3				vRotation = {};
		_float3				vScale = { 1.f, 1.f, 1.f };
	}DUMMY_NPC_DESC;
private:
	CDummyNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummyNPC(const CDummyNPC& rhs);
	virtual ~CDummyNPC() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	CTexture*			m_pNormalTextureCom = { nullptr };
	CTexture*			m_pSurfaceTextureCom = { nullptr };
	CTexture*			m_pDiffuseTextureCom = { nullptr };

	_uint				m_iID = {};
	_float				m_fUsingSurfaceParams = {};

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CDummyNPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
	HRESULT Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root);
};

NS_END
