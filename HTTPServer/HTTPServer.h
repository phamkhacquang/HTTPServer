#pragma once

#include "resource.h"
#include <winsock2.h>
#include <stdio.h>
#include <string> 
const int PORT = 8080;
char * replaceStr(const char *string, const char *substr, const char *replacement) {
	char *tok = NULL;
	char *newstr = NULL;
	char *oldstr = NULL;
	/* if either substr or replacement is NULL, duplicate string a let caller handle it */
	if (substr == NULL || replacement == NULL) return strdup(string);
	newstr = strdup(string);
	while ((tok = strstr(newstr, substr))) {
		oldstr = newstr;
		newstr = (char*)malloc(strlen(oldstr) - strlen(substr) + strlen(replacement) + 1);
		/*failed to alloc mem, free old string and return NULL */
		if (newstr == NULL) {
			free(oldstr);
			return NULL;
		}
		memcpy(newstr, oldstr, tok - oldstr);
		memcpy(newstr + (tok - oldstr), replacement, strlen(replacement));
		memcpy(newstr + (tok - oldstr) + strlen(replacement), tok + strlen(substr), strlen(oldstr) - strlen(substr) - (tok - oldstr));
		memset(newstr + strlen(oldstr) - strlen(substr) + strlen(replacement), 0, 1);
		free(oldstr);
	}
	return newstr;
}

char* getMyIp()
{
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		return "";
	}

	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
		return "";
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		return inet_ntoa(addr);
	}
}

char* getLink(char* filePath) {
	char* newstr = (char*)calloc(1024, 1);
	sprintf(newstr, "http://%s:%d/%s", getMyIp(), PORT, filePath);
	return newstr;
}

char* getLink(int PORT) {
	char* newstr = (char*)calloc(1024, 1);
	sprintf(newstr, "http://%s:%d/", getMyIp(), PORT);
	return newstr;
}

int sendFile(char *filename, SOCKET socket) {

	FILE *file = fopen(filename, "r");
	if (!file) return 0;

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (size <= 0) return 0;

	char* lenStrSend = NULL;
	char* lenStrType = NULL;
	char* contentType = "text/html";
	lenStrSend = (char*)calloc(1024, 1);
	lenStrType = (char*)calloc(1024, 1);
	sprintf(lenStrSend, "Content - length: %d\n", size);
	sprintf(lenStrType, "Content-Type: %s\n\n", contentType);
	send(socket, "HTTP/1.1 200 OK\n", 16, 0);
	send(socket, lenStrSend, strlen(lenStrSend), 0);
	send(socket, lenStrType, strlen(lenStrType), 0);

	char* buffer = (char*)calloc(size, 1);
	fread(buffer, size, 1, file);
	send(socket, buffer, size, 0);
	fclose(file);
	free(buffer);
	free(lenStrSend);
	free(lenStrType);
	closesocket(socket);
	return size;
}