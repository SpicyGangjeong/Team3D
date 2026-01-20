#pragma once

#include "Client_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CSpellLearn_Panel final : public CPanelObject
{
private:
	struct Booster
	{
		_bool bBoosterOn = { false };
		_int iBoosterIndex{};
	};
	struct State
	{
		_bool bHit = { false };
		_bool bPerHit = { false };
	};
private:
	CSpellLearn_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_Panel(const CSpellLearn_Panel& rhs);
	virtual ~CSpellLearn_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Learn(_int Index);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CInfoInstance* m_pInfoInstance = { nullptr };

	vector<SPELLLEARNINFO>   m_Info = {};

	CGameObject* m_pSpellLearn_Name = { nullptr };
	CGameObject* m_pSpellLearn = { nullptr };
	CGameObject* m_pSpellLearn_MovePointer = { nullptr };
	CGameObject* m_pSpellLearn_ChaserPointer = { nullptr };
	CGameObject* m_pSpellLearn_Data = { nullptr };
	CGameObject* m_pSpellLearn_Booster = { nullptr };
	CGameObject* m_pSpellLearn_Slot = { nullptr };
	CGameObject* m_pSpellLearn_Overlay = { nullptr };

	_int Index{};
	vector<CGameObject*> m_pBooster;
	Booster m_Booster;
	State m_State;
	vector<State> m_PointerStates;
	vector<State> m_ChaseStates;
public:
	static CSpellLearn_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
