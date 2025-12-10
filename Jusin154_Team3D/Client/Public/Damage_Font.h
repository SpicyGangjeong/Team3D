#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CDamage_Font final : public CGameObject
{
	typedef struct tagDamageFontInfo
	{
		_vector		vTarget_Pos{};
		_wstring	pDamage;
		_float		fTime = 2.f;
		_float		fAlpha = 1.f;
		_vector		vColor{};
	}DAMAGE_FONT_INFO;
private:
	CDamage_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDamage_Font(const CDamage_Font& rhs);
	virtual ~CDamage_Font() = default;

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
	void Add_Damage(void* pArg);

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	vector<DAMAGE_FONT_INFO> m_DamageInfos;

	_vector m_vWhite_Color{};

public:
	static CDamage_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END;
