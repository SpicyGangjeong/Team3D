#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CSpellLearn_MovePointer final : public CElementObject
{
private:
	struct Booster
	{
		_bool bBoosterOn = { false };
		_int iBoosterIndex{};
	};
private:
	CSpellLearn_MovePointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_MovePointer(const CSpellLearn_MovePointer& rhs);
	virtual ~CSpellLearn_MovePointer() = default;

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

public:
	void Set_SpellLearn(_int Index);
	_int Get_SpellTrail();
	_bool Get_MoveStart();
private:
	void Line(_float fTime, _float2 fMouse);

	void Set_Booster(void* pArg);


private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class SpellLearm_LookPointer* m_pSpellLeam_LookPointer = { nullptr };

	_bool	m_bMoveStart = { false };
	_bool	m_bBooster = { false };

	Booster m_Booster;

	_int	m_iLineIndex{};
	_int	m_iCurrentLine{};
	vector<_float4> m_MoveLine;
	vector<_float4> m_Boosters;

public:
	static CSpellLearn_MovePointer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
