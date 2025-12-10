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
	void	Set_PreFrameDraw() { m_bCurFrameDraw = false; }

private:
	ID3D11Query*		m_pQuery[5] = {};

	_bool				m_bPreFrameDraw = { true };
	_bool				m_bCurFrameDraw = {};

	_uint				m_iNumCullFrame = {};
	_uint				m_iCountFrame = {};
	_uint				m_iGetFrameIndex = {};
	_uint				m_iCheckFrame = {};

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
