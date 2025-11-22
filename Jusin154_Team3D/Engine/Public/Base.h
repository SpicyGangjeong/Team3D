#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{

protected:
	CBase();
	virtual ~CBase() = default;

public:
	_uint AddRef();
	_uint Release();

protected:
	atomic<_uint>	m_iRefCnt = { 0 };

public:
	virtual void Free();
};

NS_END
