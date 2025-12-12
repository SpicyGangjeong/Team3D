#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CHpBarBG final : public CElementObject
{
private:
	CHpBarBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHpBarBG(const CHpBarBG& rhs);
	virtual ~CHpBarBG() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;
	virtual void Lerp_PosX(_float X) override;

public:
	void Heal(_float fTimeDelta);
	void Hit(_float fTimeDelta);
	virtual void SizeUpX(_float SizeX) override;
	void Set_Damage(_float fDamage);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CTexture*			m_pDiffuse_TextureCom = { nullptr };
	CTexture*			m_pDiffuse_TextureCom1 = { nullptr };
	CTexture*			m_pDiffuse_TextureCom2 = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	CInfoInstance* m_pInfoInstance = { nullptr };

	_float				m_fBlinkTime{};
	_float				m_fMaxHp{};
	_float				m_fCurrentHp{};
	_float				m_fHpBar{};
	_float				m_fDamage{};
	_float				m_fTargetHp{};
	_float2				m_fHpBG{};
	_float2				m_fHpBGPos{};
	_float2				m_fHpBGSize{};
public:
	static CHpBarBG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
