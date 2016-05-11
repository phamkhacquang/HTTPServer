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
char* getFileSizeString(char *filename) {
	char* returnValue = (char*)calloc(1024, 1);
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		return "0 KB";
	}
	fseek(f, 0, SEEK_END);
	int64_t filesize = ftell(f);
	if (filesize < 1024) {
		sprintf(returnValue, "%d Byte", filesize);
		return returnValue;
	}
	if (filesize < 1024*1024) {
		sprintf(returnValue, "%d KB", filesize/1024);
		return returnValue;
	}
	if (filesize < 1024*1024*1024) {
		sprintf(returnValue, "%d MB", filesize/(1024*1024));
		return returnValue;
	}
}

bool sendFile(char *filename, SOCKET socket)
{
	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		return false;
	}
	fseek(f, 0, SEEK_END);
	long filesize = ftell(f);
	rewind(f);
	if (filesize == EOF)
		return false;
	if (filesize > 0)
	{
		char* buffer = new char[filesize];
		fread(buffer, 1, filesize, f);
		send(socket, buffer, filesize, 0);
		free(buffer);
	}
	fclose(f);	
	return true;
}
char* readTextFile(char *filename) {
	FILE *f = fopen(filename, "rt");
	if (f == NULL) {
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	long filesize = ftell(f);
	rewind(f);
	if (filesize == EOF)
		return false;
	if (filesize > 0)
	{
		char *buffer = (char*)calloc(filesize, 1);
		char *getstr = (char*)calloc(1024, 1);
		while (!feof(f)) {
			fgets(getstr, 1024, f);
			sprintf(buffer + strlen(buffer), "%s", getstr);
		}
		fclose(f);
		return buffer;
	}
	return NULL;
}
const char* getFilenameExt(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";	
	return dot + 1;
}
char* add_info(struct sockaddr_in &client) {
	char* buffer = (char*)calloc(1024, 1);
	char *connected_ip = inet_ntoa(client.sin_addr);
	int connected_port = ntohs(client.sin_port);
	sprintf(buffer, "Client information: IP: %s, on PORT:%d<br>", connected_ip, connected_port);
	sprintf(buffer + strlen(buffer), "Server information: IP: %s, on PORT:%d<br>", getMyIp(), PORT);
	return buffer;
}
const char* top = readTextFile("C:\\web/top.txt");
const char* bot = readTextFile("C:\\web/bot.txt");