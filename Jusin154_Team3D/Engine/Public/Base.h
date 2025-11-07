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

private:
	_uint		m_iRefCnt = {};

public:
	virtual void Free();
};

NS_END