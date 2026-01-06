#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CRide_BoosterBar final : public CElementObject
{
private:
	CRide_BoosterBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRide_BoosterBar(const CRide_BoosterBar& rhs);
	virtual ~CRide_BoosterBar() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	void Active(_float fTimeDelta);
	void NonActive(_float fTimeDelta);

public:
	void Set_Gauge(_float fTime);
	void Booster(_bool bBooster);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float				m_fBlinkTime{};
	_float				m_fMaxGauge{};
	_float				m_fCurrentGauge{};
	_float				m_fGaugeBar{};
	_float				m_fDamage{};
	_float				m_fTargetGauge{};
	_float2				m_fGaugeBG{};
	_float2				m_fGaugeBGPos{};
	_float2				m_fGaugeBGSize{};

	_int				m_iClick{};

	_bool				m_bBooster = { false };

public:
	static CRide_BoosterBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
