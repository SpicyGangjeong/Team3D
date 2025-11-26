#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CWand final : public CPartObject
{
public:
	typedef struct tagWand : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}WAND_DESC;
private:
	CWand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWand(const CWand& Prototype);
	virtual ~CWand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	const _float4x4*	 m_pSocketMatrices = {  };
	CShader*			 m_pShaderCom = { nullptr };
	CModel*				 m_pModelCom = { nullptr };

	class CTrailObject*	  m_pTrail = { nullptr };
	class CEffectParts*   m_pEffectParts = { nullptr };

	_float3				  m_vOffset = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
