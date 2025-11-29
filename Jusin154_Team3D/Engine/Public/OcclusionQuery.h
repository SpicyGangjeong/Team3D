#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL COcclusionQuery final : public CComponent
{
private:
	COcclusionQuery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COcclusionQuery(const COcclusionQuery& rhs);
	virtual ~COcclusionQuery() = default;

public:
	void	Begin_Query();
	_bool	isDraw();
	void	End_Query();

private:
	ID3D11Query*		m_pQuery = { nullptr };

	_bool				m_bPreFrameDraw = {};
	_bool				m_bThisFrameDraw = {};
	_uint				m_NumCullFrame = {};

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static COcclusionQuery* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
