#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;
public:
	void			  Add_Light(_uint _iCurrentLevel, class CLight* _pLight);
	void			  Add_Light_Group(_uint _iCurrentLevel, class CLight* _pLight);
	void			  Delete_Light(_uint _iCurrentLevel, class CLight* _pLight);
	void			  Light_Clear(_uint _iCurrentLevel);
	const LIGHT_DESC* Get_Light_Info(_uint _iCurrentLevel ,_uint _iLightIndex);

public:
	HRESULT Render_Lights(_uint iCurrentLevel, class CShader* pShader, class CVIBuffer* pVIBuffer);

private:
	HRESULT Initialize(_uint iNumLevels);

private:
	_uint				 m_iLevelNumber = {};
	list<class CLight*>* m_Lights = { nullptr };
	list<class CLight*>* m_FrameLights = { nullptr };
public:
	static CLight_Manager* Create(_uint iNumLevel);
	virtual void Free() override;
};

NS_END
