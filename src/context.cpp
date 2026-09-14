#include "extension.h"
#include "context.h"

const ParamType g_ConnectCbParams[] = {Param_Cell};
const ParamType g_ErrorCbParams[] = {Param_Cell, Param_Cell, Param_String};
const ParamType g_DataCbParams[] = {Param_Cell, Param_String, Param_Cell};

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

	if (m_pConnectCallback)
		forwards->ReleaseForward(m_pConnectCallback);

	if (m_pErrorCallback)
		forwards->ReleaseForward(m_pErrorCallback);

	if (m_pDataCallback)
		forwards->ReleaseForward(m_pDataCallback);

	m_Deleted = true;
}

// Client
void CAsyncSocketContext::Connected()
{
	m_PendingCallback = false;
	if(!m_pConnectCallback)
		return;

	m_pConnectCallback->PushCell(m_Handle);
	m_pConnectCallback->Execute(NULL);
}

// Server
void CAsyncSocketContext::OnConnect(CAsyncSocketContext *pSocketContext)
{
    m_PendingCallback = false;
    if (!m_pConnectForward || m_pConnectForward->GetFunctionCount() == 0)
        return;

    m_pConnectForward->PushCell(pSocketContext->m_Handle);
    m_pConnectForward->Execute();
}

void CAsyncSocketContext::OnError(int error)
{
	m_PendingCallback = false;
    if (!m_pErrorCallback || m_pErrorCallback->GetFunctionCount() == 0)
        return;

	m_pErrorCallback->PushCell(m_Handle);
	m_pErrorCallback->PushCell(error);
	m_pErrorCallback->PushString(uv_err_name(error));
	m_pErrorCallback->Execute(NULL);
}

void CAsyncSocketContext::OnData(char* data, ssize_t size)
{
	m_PendingCallback = false;
    if (!m_pDataCallback || m_pDataCallback->GetFunctionCount() == 0)
        return;

	m_pDataCallback->PushCell(m_Handle);
	m_pDataCallback->PushString(data);
	m_pDataCallback->PushCell(size);
	m_pDataCallback->Execute(NULL);
}

bool CAsyncSocketContext::SetConnectCallback(funcid_t function)
{
    if (!m_pConnectForward)
	{
        m_pConnectForward = forwards->CreateForwardEx(NULL, ET_Ignore, 1, g_ConnectCbParams);
	}

    IPluginFunction *fn = m_pContext->GetFunctionById(function);
    if (!fn)
        return false;

    return m_pConnectForward->AddFunction(fn);
}

bool CAsyncSocketContext::SetErrorCallback(funcid_t function)
{
    if (!m_pErrorCallback)
	{
        m_pErrorCallback = forwards->CreateForwardEx(NULL, ET_Ignore, 3, g_ErrorCbParams);
	}

    IPluginFunction *fn = m_pContext->GetFunctionById(function);
    if (!fn)
        return false;

    return m_pErrorCallback->AddFunction(fn);
}

bool CAsyncSocketContext::SetDataCallback(funcid_t function)
{
	if (!m_pDataCallback)
	{
        m_pDataCallback = forwards->CreateForwardEx(NULL, ET_Ignore, 3, g_DataCbParams);
	}

    IPluginFunction *fn = m_pContext->GetFunctionById(function);
    if (!fn)
        return false;

    return m_pDataCallback->AddFunction(fn);
}
