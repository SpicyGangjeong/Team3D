#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CEnemy_Detection final : public CPartObject
{
public:
	typedef struct tagEnemyDetection : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {};
	}ENEMY_DETECTION_DESC;
private:
	CEnemy_Detection(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy_Detection(const CEnemy_Detection& Prototype);
	virtual ~CEnemy_Detection() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Render_Disolve();

private:
	void SizeUpX(_float fX);
	void SizeUpY(_float fY);
	void SizeUpdate(_float fSize);

private:
	const _float4x4* m_pSocketMatrices = {  };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CTexture* m_pDiffuse_TextureCom3 = { nullptr };
	CTexture* m_pDiffuse_TextureCom4 = { nullptr };
	CTexture* m_pDiffuse_TextureCom5 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_vector m_vPosition{};

	_float4 m_vImageposi{};

	_bool m_bStep1 = { false };
	_bool m_bStep2 = { false };
	_bool m_bStep3 = { false };

	_bool m_fActive = { false };
	_bool m_fSizeUp = { false };

	_float3 m_fOrigin_Size{};

	_float2 m_fSize;

	_float m_fAlpha{};
	_float m_fTime{};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEnemy_Detection* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
