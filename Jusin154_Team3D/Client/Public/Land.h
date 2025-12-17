#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CLand final : public CGameObject
{
public:
	typedef struct tagLandDesc
	{
		_float3 vPosition;
		_float3 vScale;
		_wstring strModelComTag;
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

private:
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	_float				m_fRaduis = {};

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
