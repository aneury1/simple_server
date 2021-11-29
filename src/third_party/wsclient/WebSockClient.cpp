
#ifdef TODO

#include<websockclient/WebSockClient.h>
#include<httpc/HTTPClientString.h>
#include <httpc/HTTPClientAuth.h>
#include <websockclient/WebSockClientUtils.h>

static UINT32 WebSockIntrnBuildMaskedFrame(
	BOOL pFin,
	WEBSOCK_OPCODE pOpCode,
	const UCHAR* pMask,
	BOOL pMaskedFlag,
	const UCHAR* pData,
	UINT32 pDataLen,
	UCHAR* pOutFrame);

UINT32 WebSockSendRequest(HTTP_SESSION_HANDLE pSession,
	CHAR* pUrl,           //  [IN] The requested URL
	VOID* pData,          //  [IN] Data to post to the server
	UINT32 nDataLength,   //  [IN] Length of posted data
	BOOL TotalLength,     //  [IN]
	UINT32 nTimeout,      //  [IN] Operation timeout
	UINT32 nClientPort)   //  [IN] Client side port 0 for none
{

	UINT32          nRetCode;               // Function call return code
	UINT32          nBytes;                 // Bytes counter (socket operations)
	UINT32          nUrlLength;             // Length of the given Url
	P_HTTP_SESSION  pHTTPSession = NULL;    // Session pointer
	CHAR            ContentLength[32];

	do
	{

		// Cast the handle to our internal structure and check the pointers validity
		pHTTPSession = (P_HTTP_SESSION)pSession;
		if (!pHTTPSession)
		{
			nRetCode = HTTP_CLIENT_ERROR_INVALID_HANDLE;
			break;
		}

#ifdef _WEBSOCK_DEBUGGING_
		if (pHTTPSession->pDebug)
		{
			pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "Url: %s", pUrl);
		}
#endif

		// Set the operation timeout counters
		pHTTPSession->HttpCounters.nActionStartTime = HTTPIntrnSessionGetUpTime();
		// 0 makes us use the default defined value
		pHTTPSession->HttpCounters.nActionTimeout = HTTP_TIMEOUT(nTimeout);
		// Store the cliebt port for later usage
		pHTTPSession->HttpConnection.HttpClientPort = nClientPort;

		// Parse the URL
		nUrlLength = strlen(pUrl);
		nRetCode = WebSockIntrnSetURL(pHTTPSession, pUrl, nUrlLength);
		if (nRetCode != HTTP_CLIENT_SUCCESS)
		{
#ifdef _WEBSOCK_DEBUGGING_
			if (pHTTPSession->pDebug)
			{
				pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "WebSockIntrnSetURL != HTTP_CLIENT_SUCCESS");
			}
#endif
			break;
		}

		// Create the default headers
		// Add the "Host" header. we should handle a special case of port incorporated within the host name.
		if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLANDPORT) != HTTP_CLIENT_FLAG_URLANDPORT)
		{
			// The case where we don't have the port
			if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession,
				"Host", 4,
				pHTTPSession->HttpUrl.UrlHost.pParam,
				pHTTPSession->HttpUrl.UrlHost.nLength)) != HTTP_CLIENT_SUCCESS)
			{
#ifdef _WEBSOCK_DEBUGGING_
				if (pHTTPSession->pDebug)
				{
					pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "HTTPIntrnHeadersAdd(Host) != HTTP_CLIENT_SUCCESS");
				}
#endif
				break;
			}
		}
		else
		{
			// We have the port so use a deferent element
			if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Host", 4, pHTTPSession->HttpUrl.UrlHost.pParam,
				(pHTTPSession->HttpUrl.UrlPort.pParam - pHTTPSession->HttpUrl.UrlHost.pParam) - 1)) != HTTP_CLIENT_SUCCESS)
			{
				break;
			}
		}
		// We are in a post request without knowing the total length in advance so return error or use chunking
		if (pHTTPSession->HttpHeaders.HttpVerb == VerbPost && TotalLength == FALSE)
		{
			// If the user specified the chunked flag
			if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_SEND_CHUNKED) == HTTP_CLIENT_FLAG_SEND_CHUNKED)
			{
				// Add the  Transfer-Encoding:  header
				if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Transfer-Encoding", 17, "chunked", 7)) != HTTP_CLIENT_SUCCESS)
				{
					break;
				}
			}
			else
			{
				// Not a supported operation - unknown length
				nRetCode = HTTP_CLIENT_ERROR_HEADER_NO_LENGTH;
				break;
			}
		}

		// Add the "User-Agent" header
		if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "User-Agent", 10, HTTP_CLIENT_DEFAULT_AGENT, strlen(HTTP_CLIENT_DEFAULT_AGENT))) != HTTP_CLIENT_SUCCESS)
		{
			break;
		}
		// Add the "Keep-Alive" header (if requested by the caller)
		if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_KEEP_ALIVE) == HTTP_CLIENT_FLAG_KEEP_ALIVE)
		{
			if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_USINGPROXY) != HTTP_CLIENT_FLAG_USINGPROXY)
			{
				// No proxy keep alive:
				if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Connection", 10, "Keep-Alive", 10)) != HTTP_CLIENT_SUCCESS)
				{
					break;
				}
			}
			else // proxy keep alive
			{
				if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Proxy-Connection", 15, "Keep-Alive", 10)) != HTTP_CLIENT_SUCCESS)
				{
					break;
				}
			}
		}

		// If we know the total length in advance (only when posting)
		if (pHTTPSession->HttpHeaders.HttpVerb == VerbPost && TotalLength == TRUE)
		{
			// set the total content length header
			pHTTPSession->HttpHeadersInfo.nHTTPPostContentLength = nDataLength; // Store for later usage
			memset(ContentLength, 0, 32);
			IToA(ContentLength, nDataLength); // Convert the buffer length to a string value
			if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Content-Length", 14, ContentLength, strlen(ContentLength))) != HTTP_CLIENT_SUCCESS)
			{
				break;
			}
		}

		// Add the "Cache control" header (if requested by the caller)
		if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_NO_CACHE) == HTTP_CLIENT_FLAG_NO_CACHE)
		{
			if ((nRetCode = HTTPIntrnHeadersAdd(pHTTPSession, "Cache-Control", 13, "no-cache", 8)) != HTTP_CLIENT_SUCCESS)
			{
				break;
			}
		}


		// Now we can connect to the remote server and send the leading request followed by the HTTP headers
		// Check for timeout
		if (HTTPIntrnSessionEvalTimeout(pHTTPSession) == TRUE)
		{
#ifdef _WEBSOCK_DEBUGGING_
			if (pHTTPSession->pDebug)
			{
				pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "HTTPIntrnSessionEvalTimeout(pHTTPSession) == TRUE");
			}
#endif
			nRetCode = HTTP_CLIENT_ERROR_SOCKET_TIME_OUT;
			break;
		}
		//  Handle connection close message (reconnect)
		if (pHTTPSession->HttpHeadersInfo.Connection == FALSE)
		{
			// Gracefully close the connection and set the socket as invalid
			if (pHTTPSession->HttpConnection.HttpSocket != HTTP_INVALID_SOCKET)
			{
				HTTPIntrnConnectionClose(pHTTPSession);
			}
			// Connect to the remote server (or proxy)
			nRetCode = HTTPIntrnConnectionOpen(pHTTPSession);
			if (nRetCode != HTTP_CLIENT_SUCCESS)
			{
				break;
			}
		}

		// Send the request along with the rest of the headers
		if (pHTTPSession->HttpCredentials.CredAuthSchema != AuthSchemaNone ||
			pHTTPSession->HttpProxy.ProxyAuthSchema != AuthSchemaNone) // If we have to authenticate we should use the HEAD verb
		{
			if (HTTP_CLIENT_ALLOW_HEAD_VERB) // HEAD should not be ussed if not defined
			{
				if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, VerbHead)) != HTTP_CLIENT_SUCCESS)
				{
					break;
				}
				// Set the state flag
				pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_HEAD_SENT;
			}
			else
			{
				// Simply use the verb that was set by the caller without changing to HEAD
				if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, pHTTPSession->HttpHeaders.HttpVerb)) != HTTP_CLIENT_SUCCESS)
				{
#ifdef _WEBSOCK_DEBUGGING_
					if (pHTTPSession->pDebug)
					{
						pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "HTTPIntrnHeadersSend() != HTTP_CLIENT_SUCCESS");
					}
#endif
					break;
				}
				// This the case where the caller know the total length to receive in advance
				// and he wishes to send the data right away
				if (pHTTPSession->HttpHeaders.HttpVerb == VerbPost && TotalLength == TRUE)
				{

					// Send the data
					nBytes = nDataLength;
					if ((nRetCode = HTTPIntrnSend(pHTTPSession, (CHAR*)pData, &nBytes)) != HTTP_CLIENT_SUCCESS)
					{
#ifdef _WEBSOCK_DEBUGGING_
						if (pHTTPSession->pDebug)
						{
							pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "HTTPIntrnSend() != HTTP_CLIENT_SUCCESS");
						}
#endif
						break;
					}
					// Set the session state
					pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_POST_SENT;
				}

			}

			// Retrive and analyze the Headers
			if ((nRetCode = HTTPIntrnHeadersReceive(pHTTPSession, nTimeout)) != HTTP_CLIENT_SUCCESS)
			{
				break;
			}
			if (pHTTPSession->HttpHeadersInfo.nHTTPStatus != 401 && pHTTPSession->HttpHeadersInfo.nHTTPStatus != 407)
			{

				nRetCode = HTTP_CLIENT_ERROR_AUTH_MISMATCH;
				break;
			}
			// Authenticate
			if ((nRetCode = HTTPIntrnAuthenticate(pHTTPSession)) != HTTP_CLIENT_SUCCESS)
			{
				break;
			}

		}
		else
		{
			// No authentication use the verb that was requested by the caller
			if ((nRetCode = HTTPIntrnHeadersSend(pHTTPSession, pHTTPSession->HttpHeaders.HttpVerb)) != HTTP_CLIENT_SUCCESS)
			{
#ifdef _WEBSOCK_DEBUGGING_
				if (pHTTPSession->pDebug)
				{
					pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "HTTPIntrnHeadersSend() != HTTP_CLIENT_SUCCESS");
				}
#endif
				break;
			}
		}
		// This the case where the caller know the total length to receive in advance
		// and he wishes to send the data right away
		if (pHTTPSession->HttpHeaders.HttpVerb == VerbPost && TotalLength == TRUE)
		{

			// Send the data
			nBytes = nDataLength;
			if ((nRetCode = HTTPIntrnSend(pHTTPSession, (CHAR*)pData, &nBytes)) != HTTP_CLIENT_SUCCESS)
			{
#ifdef _WEBSOCK_DEBUGGING_
				if (pHTTPSession->pDebug)
				{
					pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "HTTPIntrnSend() != HTTP_CLIENT_SUCCESS");
				}
#endif
				break;
			}
			// Set the session state
			pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_POST_SENT;
		}

	} while (0);



	return nRetCode;
}


UINT32 WebSockIntrnSetURL(P_HTTP_SESSION pHTTPSession,
	CHAR* pUrl,         // [IN] a null terminated string containing the Url we should retrieve
	UINT32 nUrlLength)  // [IN] The length the Url string
{

	UINT32         nUrlOffset;      // Offset in bytes within the Url string
	HTTP_URL* pUrlPtr;        // a Pointer to the Url structure (within the global session structure)
	CHAR* pPtr;           // a Pointer for the Url port (Used in the parsing process)
	CHAR           UrlPort[16];     // a temporary byte array for the Url port conversion operation (string to number)

	// Check for the session pointer validity
	if (!pHTTPSession)
	{
		return HTTP_CLIENT_ERROR_INVALID_HANDLE;
	}


	// Get the length of the Url
	nUrlLength = strlen(pUrl);

	// Check if it is not longer than the permitted length
	if ((nUrlLength + 1) > HTTP_CLIENT_MAX_URL_LENGTH)
	{
		return HTTP_CLIENT_ERROR_LONG_INPUT;
	}

	// Point the local pointer on the global structure
	pUrlPtr = &pHTTPSession->HttpUrl;

	// Copy to the internal buffer
	memcpy(pHTTPSession->HttpUrl.Url, pUrl, nUrlLength);
	nUrlOffset = 0;

	// Get the Url base ("http" or "https")
	if (HTTPStrSearch(pUrlPtr->Url, ":", nUrlOffset, nUrlLength, &pUrlPtr->UrlBsee) == FALSE)
	{
		return HTTP_CLIENT_ERROR_BAD_URL;
	}
	// Increase the offset parameter
	nUrlOffset += pUrlPtr->UrlBsee.nLength;
	// If we can parse the string "HTTPS" we can assume a secured session
	if (HTTPStrInsensitiveCompare(pUrlPtr->UrlBsee.pParam, "wss", pUrlPtr->UrlBsee.nLength) == TRUE)
	{
		// Set the secured flags on the session
		pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_URLHTTPS;
		pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_SECURE;
		// ToDo: Init TLS (GetProtocol)


#ifdef _WEBSOCK_DEBUGGING_
		if (pHTTPSession->pDebug)
		{
			pHTTPSession->pDebug(__FUNCTION__, NULL, 0, "SSL Protected Url %s", pUrl);
		}
#endif
	}
	else // it should be "http"
	{
		if (HTTPStrInsensitiveCompare(pUrlPtr->UrlBsee.pParam, "ws", pUrlPtr->UrlBsee.nLength) == FALSE)
		{
			return HTTP_CLIENT_ERROR_BAD_URL; // cOULD NOT DETECT http or https prefix

		}
	}
	// Look for standard Url elements
	if (HTTPStrSearch(pUrlPtr->Url, "://", nUrlOffset, 3, 0) == FALSE)
	{
		return HTTP_CLIENT_ERROR_BAD_URL; // Could not detect "://"
	}
	// Increase the offset parameter
	nUrlOffset += 3;

	// Get the host name
	if (HTTPStrSearch(pUrlPtr->Url, "/", nUrlOffset, (nUrlLength - nUrlOffset), &pUrlPtr->UrlHost) == FALSE)
	{
		pUrlPtr->Url[nUrlLength] = '/';
		nUrlLength++;
		if (HTTPStrSearch(pUrlPtr->Url, "/", nUrlOffset, (nUrlLength - nUrlOffset), &pUrlPtr->UrlHost) == FALSE)
		{
			return HTTP_CLIENT_ERROR_BAD_URL;
		}
	}

	nUrlOffset += pUrlPtr->UrlHost.nLength;

	// Do we have the port within the hostname?
	if (HTTPStrSearch(pUrlPtr->Url, ":",
		(nUrlOffset - pUrlPtr->UrlHost.nLength),
		pUrlPtr->UrlHost.nLength,
		&pUrlPtr->UrlPort) == TRUE)
	{
		if ((pUrlPtr->UrlHost.nLength - pUrlPtr->UrlPort.nLength) < 10)
		{
			// To-Do: check the actual port length before the memcpy
			pUrlPtr->UrlPort.pParam += pUrlPtr->UrlPort.nLength + 1;
			memcpy(UrlPort, pUrlPtr->UrlPort.pParam, 15);
			pUrlPtr->UrlPort.nLength = 0;
			pPtr = UrlPort;
			while (*pPtr && pPtr++)
			{

				pUrlPtr->UrlPort.nLength++;
				if (*pPtr == '/')
				{
					*pPtr = 0;
					pUrlPtr->nPort = (UINT16)atol(UrlPort);
					pHTTPSession->HttpFlags = pHTTPSession->HttpFlags | HTTP_CLIENT_FLAG_URLANDPORT;
					break;
				}
			}
		}
		else
		{
			// Port too big
			return HTTP_CLIENT_ERROR_BAD_URL;
		}
	}

	// Get the request body
	pUrlPtr->UrlRequest.pParam = pUrlPtr->Url + nUrlOffset;
	pUrlPtr->UrlRequest.nLength = nUrlLength - nUrlOffset;

	// If we got SSL url with no port we should set the default ssl port
	if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLHTTPS) == HTTP_CLIENT_FLAG_URLHTTPS)
	{
		if ((pHTTPSession->HttpFlags & HTTP_CLIENT_FLAG_URLANDPORT) != HTTP_CLIENT_FLAG_URLANDPORT)
		{
			pHTTPSession->HttpUrl.nPort = HTTP_CLIENT_DEFAULT_SSL_PORT;

		}

	}

	// Set the state flag
	pHTTPSession->HttpState = pHTTPSession->HttpState | HTTP_CLIENT_STATE_URL_PARSED;

	return HTTP_CLIENT_SUCCESS;

}


UINT32 WebSockClientOpenRequest(HTTP_CLIENT_SESSION_FLAGS Flags)
{
	HTTP_SESSION_HANDLE pHTTP;
	CHAR nounceBase64[64] = { '\0' };
	CHAR nounce[33] = { '\0' };
	INT32 nRetCode;

	pHTTP = HTTPClientOpenRequest(Flags);

	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, "Upgrade", "websocket", FALSE)) != HTTP_CLIENT_SUCCESS)
		return HTTP_CLIENT_ERROR_SOCKET_CANT_SET;

	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, "Connection", "Upgrade", FALSE)) != HTTP_CLIENT_SUCCESS)
		return HTTP_CLIENT_ERROR_SOCKET_CANT_SET;

	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, "Sec-WebSocket-Version", WEBSOCK_WEBSOCKET_VERSION, FALSE)) != HTTP_CLIENT_SUCCESS)
		return HTTP_CLIENT_ERROR_SOCKET_CANT_SET;

	HTTPDigestGenerateCNonce(nounce);
	HTTPBase64Encoder((unsigned char*)nounceBase64, (const unsigned char*)nounce, 16);

	if ((nRetCode = HTTPClientAddRequestHeaders(pHTTP, "Sec-WebSocket-Key", nounceBase64, FALSE)) != HTTP_CLIENT_SUCCESS)
		return HTTP_CLIENT_ERROR_SOCKET_CANT_SET;

	if ((nRetCode = HTTPClientSetVerb(pHTTP, VerbGet)) != HTTP_CLIENT_SUCCESS)
	{
		return HTTP_CLIENT_ERROR_SOCKET_CANT_SET;
	}

	return pHTTP;
}

UINT32 WebSockMaskData(
	const UCHAR* pOriginal,
	UINT32 pOriginalLength,
	const UCHAR* pMaskKey,
	UCHAR* pOutMaskedData)
{
	if (pOriginalLength <= 0)
		return (UINT32)0;

	for (UINT32 i = 0; i < pOriginalLength; i++)
	{
		int j = i % 4;
		pOutMaskedData[i] = pOriginal[i] ^ pMaskKey[j];
	}
	return pOriginalLength;
}

UINT32 WebSockParseFrame(const UCHAR* pFrameData, WEBSOCK_FRAME_HEADER1* header, UCHAR* pOutData)
{
	//unsigned long long extended_len2 = 0;
	UINT32 index = 2;
	UINT16 extended_len = 0;
	if (pOutData == NULL)
		return 0;

	header->fin = (pFrameData[0] & 0x1 << 7) >> 7;
	header->opcode = pFrameData[0] & 0xF;
	header->masked = (pFrameData[1] & 0x80) >> 7;
	header->len = pFrameData[1] & 0x7F;

	if (header->len == 126)
	{
		memcpy((void*)&extended_len, (void*)&pFrameData[index], 2);
		extended_len = extended_len = swapEndianess16(extended_len);
		index += 2;
	}
	/*else if (header->len > 126)
	{
		memcpy((void*)&extended_len2, (void*)&pFrameData[index], 8);
		extended_len2 = swapEndianess64(extended_len2);
		index += 8;
	}*/
	else
		extended_len = header->len;

	if (header->masked) {
		memcpy((void*)&header->maskkey, (void*)&pFrameData[index], 4);
		index += 4;
	}

	if (pOutData)
		memcpy((void*)pOutData, (void*)&pFrameData[index], extended_len);

	return extended_len;
}

UINT32 WebSockBuildFrame(BOOL pFin, WEBSOCK_OPCODE pOpCode, const UCHAR* pData, UINT32 pDataLen, UCHAR* pOutFrame)
{
#if 0
	UCHAR frameData[255] = { '\0' };
	UCHAR fin_rsv123 = 0x00;
	UCHAR len_mask = 0x00;

	if (pDataLen < 0 || pOutFrame == NULL)
		return 0;

	fin_rsv123 ^= pFin << 7;
	fin_rsv123 ^= pOpCode;
	len_mask = pDataLen;
	len_mask ^= 0 << 7;

	frameData[0] = fin_rsv123;
	frameData[1] = len_mask;

	memcpy((void*)&frameData[2], (void*)pData, pDataLen);
	frameData[2 + pDataLen] = 0;

	memcpy(pOutFrame, frameData, 2 + pDataLen);

	return 2 + pDataLen;
#endif
	return WebSockIntrnBuildMaskedFrame(
		pFin,
		pOpCode,
		NULL,
		FALSE,
		pData,
		pDataLen,
		pOutFrame
	);
}

UINT32 WebSockBuildMaskedFrame(BOOL pFin, WEBSOCK_OPCODE pOpCode, UCHAR* pMask, const UCHAR* pData, UINT32 pDataLen, UCHAR* pOutFrame)
{
#if 0
	UCHAR frameData[255] = { '\0' };
	UCHAR fin_rsv123 = 0x00;
	UCHAR len_mask = 0x00;

	if (pDataLen < 0 || pOutFrame == NULL)
		return 0;

	fin_rsv123 ^= pFin << 7;
	fin_rsv123 ^= pOpCode;
	len_mask = pDataLen;
	len_mask ^= 1 << 7;

	frameData[0] = fin_rsv123;
	frameData[1] = len_mask;

	memcpy((void*)&frameData[2], (void*)pData, pDataLen);
	frameData[2 + pDataLen] = 0;

	memcpy(pOutFrame, frameData, 2 + pDataLen);

	return 2 + pDataLen;
#endif
	return WebSockIntrnBuildMaskedFrame(
		pFin,
		pOpCode,
		pMask,
		TRUE,
		pData,
		pDataLen,
		pOutFrame);
}

UINT32 WebSockIntrnBuildMaskedFrame(
	BOOL pFin,
	WEBSOCK_OPCODE pOpCode,
	const UCHAR* pMask,
	BOOL pMaskedFlag,
	const UCHAR* pData,
	UINT32 pDataLen,
	UCHAR* pOutFrame)
{
	UCHAR frameData[255] = { '\0' };
	UCHAR maskedData[255] = { '\0' };
	//unsigned long long extended_len2 = 0;
	UINT16 extended_len = 0;
	UCHAR fin_rsv123 = 0x00;
	UCHAR len_mask = 0x00;
	UINT32 index = 2;

	if (pDataLen < 0 || pOutFrame == NULL)
		return 0;

	fin_rsv123 ^= pFin << 7;
	fin_rsv123 ^= pOpCode;
	len_mask = pDataLen;
	len_mask ^= pMaskedFlag << 7;

	frameData[0] = fin_rsv123;
	frameData[1] = len_mask;

	if (pDataLen >= 126)
	{
		len_mask = 126;
		len_mask ^= pMaskedFlag << 7;
		frameData[1] = len_mask;
		extended_len = pDataLen;
		extended_len = swapEndianess16(extended_len);
		memcpy((void*)&frameData[index], (void*)&extended_len, 2);
		index += 2;
	}
	/*else if (pDataLen > 65535)
	{
		len_mask = 127;
		len_mask ^= pMaskedFlag << 7;
		frameData[1] = len_mask;
		extended_len2 = pDataLen;
		extended_len2 = swapEndianess64(extended_len2);
		memcpy((void*)&frameData[index], (void*)&extended_len2, 8);
		index += 8;
	}*/

	if (pMaskedFlag)
	{
		memcpy((void*)&frameData[index], (void*)pMask, 4);
		index += 4;
		WebSockMaskData(pData, pDataLen, pMask, maskedData);
		maskedData[pDataLen] = 0;
		memcpy((void*)&frameData[index], (void*)maskedData, pDataLen);
	}
	else
		memcpy((void*)&frameData[index], (void*)pData, pDataLen);

	index += pDataLen;
	frameData[index] = 0;

	memcpy(pOutFrame, frameData, index);

	return index;
}



#endif