#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CMagic_Meter final : public CElementObject
{
private:
	CMagic_Meter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMagic_Meter(const CMagic_Meter& rhs);
	virtual ~CMagic_Meter() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

public:
	void Meter_Index(_uint Number);
	//void Set_

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_uint	m_iImageCount{};
	_float4 m_vGaugeUV{};
	_float m_fMaxGauge{};
	_float m_fCurrentGauge{};
	_float m_fGaugeBar{};
	_float m_fTargetGauge{};
public:
	static CMagic_Meter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
