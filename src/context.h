#ifndef ASYNC_SOCKET_CONTEXT_H
#define ASYNC_SOCKET_CONTEXT_H

#include <atomic>
#include <stdlib.h>
#include <uv.h>

#include "smsdk_ext.h"

class CAsyncSocketContext
{
public:
	IPluginContext *m_pContext;
	Handle_t m_Handle;

	std::atomic<bool> m_Deleted{false};
	bool m_PendingCallback;
	bool m_Pending;
	bool m_Server;

	char *m_pHost;
	int m_Port;

	char *m_pClientIP;

	IChangeableForward *m_pConnectCallback;
	IChangeableForward *m_pErrorCallback;
	IChangeableForward *m_pDataCallback;

	uv_getaddrinfo_t m_Resolver;
	uv_tcp_t *m_pSocket;
	uv_stream_t *m_pStream;

	// UV thread only. Number of libuv operations still holding this context (each
	// handle being closed, plus an in-flight address lookup), and whether the plugin
	// handle is gone. The context is handed to the game thread for deletion once the
	// handle is destroyed and the last reference is released.
	int m_UvRefs;
	bool m_DeleteRequested;

	CAsyncSocketContext(IPluginContext *plugin);
	~CAsyncSocketContext();

	void Connected();
	void OnConnect(CAsyncSocketContext *pSocketContext);

	void OnError(int error);

	void OnData(char *data, ssize_t size);

	bool SetConnectCallback(funcid_t function);
	bool SetErrorCallback(funcid_t function);
	bool SetDataCallback(funcid_t function);
};

#endif
