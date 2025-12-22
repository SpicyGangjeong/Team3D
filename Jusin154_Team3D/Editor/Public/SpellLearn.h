#pragma once

#include "Editor_Define.h"
#include "ElementObject.h"

NS_BEGIN(Editor)

class CSpellLearn final : public CElementObject
{
private:
	CSpellLearn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn(const CSpellLearn& rhs);
	virtual ~CSpellLearn() = default;

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
	HRESULT Change_Image(_int SpellID);
	void Set_Pointer(class CSpellLearn_MovePointer* Pointer, class CSpellLearn_ChaserPointer* Chase);

private:
	void Clear();

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class CSpellLearn_MovePointer* m_pPointer = { nullptr };
	class CSpellLearn_ChaserPointer* m_pChasePointer = { nullptr };

	_float2 m_vPointerPosition{};
	_float3 m_vPointerScale{};

	_float2 m_vChasePosition{};
	_float3 m_vChaseScale{};

	_bool m_bReset{};

	_int m_iIndex{};
	_int	m_iTrailIndex{};
	vector<_float2> m_Trail;

	_int	m_iChaseiTrailIndex{};
	vector<_float2> m_ChaseTrail;
	vector<_vector> m_MoveLine;

public:
	static CSpellLearn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
