#pragma once
#include "Base.h"
#ifdef _DEBUG
#include <d3d11_1.h>
#include <wrl/client.h>

NS_BEGIN(Engine)
class CRenderEventDebugger
{
public:
    HRESULT Initialize(ID3D11DeviceContext* pContext)
    {
        if (pContext == nullptr) {
            return E_INVALIDARG;
        }

        return pContext->QueryInterface( __uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<void**>(m_pUserAnnotation.ReleaseAndGetAddressOf())
        );
    }

    ID3DUserDefinedAnnotation* GetUserDefinedAnnotation()
    {
        return m_pUserAnnotation.Get();
    }

private:
    Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> m_pUserAnnotation;
};

class CRenderEventScope
{
public:
    CRenderEventScope(ID3DUserDefinedAnnotation* pUserDefinedAnnotation, const wchar_t* pEventName)
        : m_pUserAnnotation(pUserDefinedAnnotation)
        , m_bIsEventStart(false)
    {
        if (m_pUserAnnotation != nullptr && pEventName != nullptr)
        {
            if (m_pUserAnnotation->GetStatus() != 0)
            {
                m_pUserAnnotation->BeginEvent(pEventName);
                m_bIsEventStart = true;
            }
        }
    }

    ~CRenderEventScope()
    {
        if (m_pUserAnnotation != nullptr && m_bIsEventStart)
        {
            m_pUserAnnotation->EndEvent();
        }
    }

    CRenderEventScope(const CRenderEventScope&) = delete;
    CRenderEventScope& operator=(const CRenderEventScope&) = delete;

private:
    ID3DUserDefinedAnnotation* m_pUserAnnotation;
    _bool m_bIsEventStart;
};

#define EVENTSCOPE_(message)    CRenderEventScope __EventScope(m_AnnotationHelper.GetUserDefinedAnnotation(), TEXT(message));

NS_END

#endif // _DEBUG\

#ifndef _DEBUG
#define EVENTSCOPE_(message)
#endif // !_DEBUG

