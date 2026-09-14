#include "extension.h"
#include "context.h"

CAsyncSocketContext::CAsyncSocketContext(IPluginContext *pContext)
{
	m_pContext = pContext;

	m_pHost = NULL;
	m_Port = -1;

	m_Deleted = false;
	m_PendingCallback = false;
	m_Pending = false;
	m_Server = false;

	m_pSocket = NULL;
	m_pStream = NULL;

	m_pConnectCallback = NULL;
	m_pErrorCallback = NULL;
	m_pDataCallback = NULL;

	m_PendingCloseCount = 0;
}

CAsyncSocketContext::~CAsyncSocketContext()
{
	if(m_pHost)
		free(m_pHost);

	m_pConnectCallback = NULL;
	m_pErrorCallback = NULL;
	m_pDataCallback = NULL;

	m_Deleted = true;
}

// Client
void CAsyncSocketContext::Connected()
{
	m_PendingCallback = false;
	if(!m_pConnectCallback)
	{
		return;
	}

	m_pConnectCallback->PushCell(m_Handle);
	//m_pConnectCallback->Execute(NULL);
}

// Server
void CAsyncSocketContext::OnConnect(CAsyncSocketContext *pSocketContext)
{
    m_PendingCallback = false;
    if (!m_pConnectCallback)
	{
        return;
	}

    m_pConnectCallback->PushCell(pSocketContext->m_Handle);
    //m_pConnectCallback->Execute(NULL);
}

void CAsyncSocketContext::OnError(int error)
{
	m_PendingCallback = false;
    if (!m_pErrorCallback)
	{
        return;
	}

	m_pErrorCallback->PushCell(m_Handle);
	m_pErrorCallback->PushCell(error);
	m_pErrorCallback->PushString(uv_err_name(error));
	//m_pErrorCallback->Execute(NULL);
}

void CAsyncSocketContext::OnData(char* data, ssize_t size)
{
	m_PendingCallback = false;
	if (!m_pDataCallback)
	{
    	return;
	}

	m_pDataCallback->PushCell(m_Handle);
	m_pDataCallback->PushString(data);
	m_pDataCallback->PushCell(size);
	//m_pDataCallback->Execute(NULL);
}

bool CAsyncSocketContext::SetConnectCallback(funcid_t function)
{
    m_pConnectCallback = m_pContext->GetFunctionById(function);
    return m_pConnectCallback ? true : false;
}

bool CAsyncSocketContext::SetErrorCallback(funcid_t function)
{
    m_pErrorCallback = m_pContext->GetFunctionById(function);
    return m_pErrorCallback ? true : false;
}

bool CAsyncSocketContext::SetDataCallback(funcid_t function)
{
    m_pDataCallback = m_pContext->GetFunctionById(function);
    return m_pDataCallback ? true : false;
}
