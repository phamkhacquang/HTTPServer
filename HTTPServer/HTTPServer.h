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
char* getFileSizeString(char *filename, char *folder) {
	char* returnValue = (char*)calloc(1024, 1);
	char* fullPath = (char*)calloc(1024, 1);
	sprintf(fullPath, "C:%s/%s", folder, filename);
	FILE *f = fopen(fullPath, "rb");
	free(fullPath);
	if (f == NULL) {
		return "N/A";
	}
	fseek(f, 0, SEEK_END);
	int64_t filesize = ftell(f);
	if (filesize < 1024) {
		sprintf(returnValue, "%d Byte", filesize);
		return returnValue;
	}
	if (filesize < 1024 * 1024) {
		sprintf(returnValue, "%d KB", filesize / 1024);
		return returnValue;
	}
	if (filesize < 1024 * 1024 * 1024) {
		sprintf(returnValue, "%d MB", filesize / (1024 * 1024));
		return returnValue;
	}
}
char* getNumberOfFolderSubItem(char* path, char* folderName) {
	char* fullPath = (char*)calloc(1024, 1);
	char* returnValue = (char*)calloc(1024, 1);
	sprintf(fullPath, "C:%s/%s\\*.*", path, folderName);
	WIN32_FIND_DATAA FDATA;
	HANDLE hFind = FindFirstFileA(fullPath, &FDATA);
	int count = 0;
	do {
		count++;
	} while (FindNextFileA(hFind, &FDATA));
	free(fullPath);
	count -= 2;//Bo 2 folder la "." va ".."
	if (count < 0) {
		return "N/A";
	}
	if (count == 0) {
		return "Empty folder";
	}
	if (count == 1) {
		return "1 item";
	}
	if (count > 1) {
		sprintf(returnValue, "%d items", count);
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
	fclose(f);
	return NULL;
}
char* getFilenameExt(const char *filename) {
	char *tmp = (char*)calloc(1024, 1);
	strcpy(tmp, filename);
	char *dot = strrchr(tmp, '.');
	if (!dot || dot == tmp) return "";
	for (int i = 0; dot[i]; i++) {
		dot[i] = tolower(dot[i]);
	}
	return dot + 1;
}
char* add_info(struct sockaddr_in &client) {
	char* buffer = (char*)calloc(1024, 1);
	char *connected_ip = inet_ntoa(client.sin_addr);
	int connected_port = ntohs(client.sin_port);
	sprintf(buffer, "Client IP: %s, PORT:%d", connected_ip, connected_port);
	return buffer;
}
char* substr(char* s, int x, int y) {
	char* buffer = (char*)calloc(1024, 1);
	for (int i = x; i < y; i++) {
		buffer[i - x] = s[i];
	}
	return buffer;
}
char* add_breadcrumbs(char* path) {
	char* buffer = (char*)calloc(1024, 1);
	sprintf(buffer, "<div class=\"breadcrumbs\">");
	sprintf(buffer + strlen(buffer), "<a href=\"%s\"><span>C:</span></a>", getLink(""));

	int x = 1;
	for (int i = 1; i <= strlen(path); i++) {
		if ((path[i] == '/') || i == strlen(path)) {
			char* fn = (char*)calloc(1024, 1);
			char* fp = (char*)calloc(1024, 1);
			fn = substr(path, x, i);
			fp = substr(path, 0, i);//fp + 1 de bo qua ki tu "\"
			sprintf(buffer + strlen(buffer), "<span class = \"arrow\"> \\ </span><a href=\"%s\"><span>%s</span></a>", getLink(fp + 1), fn);
			x = i + 1;
			free(fn);
			free(fp);
		}
	}

	sprintf(buffer + strlen(buffer), "</div>");
	return buffer;
}
const char* top = readTextFile("C:\\web/top.txt");
const char* bot = readTextFile("C:\\web/bot.txt");