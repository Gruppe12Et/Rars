#ifndef GLH_EXTENSIONS
#define GLH_EXTENSIONS

#include <string.h>
#include <stdio.h>

#ifdef _WIN32
# include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _WIN32
# include <GL/wglext.h>
#endif

#define CHECK_MEMORY(ptr) \
    if (NULL == ptr) { \
		printf("Error allocating memory in file %s, line %d\n", __FILE__, __LINE__); \
		exit(-1); \
	}

#ifdef GLH_EXT_SINGLE_FILE
#  define GLH_EXTENSIONS_SINGLE_FILE  // have to do this because glh_genext.h unsets GLH_EXT_SINGLE_FILE
#endif

#include "glh_genext.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef GLH_EXTENSIONS_SINGLE_FILE
static char *unsupportedExts = NULL;

static int ExtensionExists(const char* extName, const char* sysExts)
{
    char *padExtName = (char*)malloc(strlen(extName) + 2);
    strcat(strcpy(padExtName, extName), " ");

	if (0 == strcmp(extName, "GL_VERSION_1_2")) {
		const char *version = (const char*)glGetString(GL_VERSION);
		if (strstr(version, "1.0") == version || strstr(version, "1.1") == version) {
			return FALSE;
		} else {
			return TRUE;
		}
	}
    if (strstr(sysExts, padExtName)) {
		free(padExtName);
        return TRUE;
    } else {
		free(padExtName);
        return FALSE;
    }
}

static const char* EatWhiteSpace(const char *str)
{
	for (; *str && (' ' == *str || '\t' == *str || '\n' == *str); str++);
	return str;
}

static const char* EatNonWhiteSpace(const char *str)
{
	for (; *str && (' ' != *str && '\t' != *str && '\n' != *str); str++);
	return str;
}

int glh_init_extensions(const char *origReqExts)
{
	// Length of requested extensions string
	unsigned reqExtsLen;
	char *reqExts;
	// Ptr for individual extensions within reqExts
	char *reqExt;
	int success = TRUE;

	// build space-padded extension string
	static char *sysExts = NULL;
	if (NULL == sysExts) {
		const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
		int sysExtsLen = strlen(extensions);
		const char *winsys_extensions = 0;		
		int winsysExtsLen = 0;
#ifdef _WIN32
		{
			PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 0;
			wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
			if(wglGetExtensionsStringARB)
			{
				winsys_extensions = wglGetExtensionsStringARB(wglGetCurrentDC());
				winsysExtsLen = strlen(winsys_extensions);
			}
		}
#endif
		// Add 2 bytes, one for padding space, one for terminating NULL
		sysExts = (char*)malloc(sysExtsLen + winsysExtsLen + 3);
		CHECK_MEMORY(sysExts);
		strcpy(sysExts, extensions);
		sysExts[sysExtsLen] = ' ';
		sysExts[sysExtsLen + 1] = 0;
    if( winsys_extensions )
    {
		  strcat(sysExts, winsys_extensions);
    }
		sysExts[sysExtsLen + 1 + winsysExtsLen] = ' ';
		sysExts[sysExtsLen + 1 + winsysExtsLen + 1] = 0;
	}

	if (NULL == origReqExts)
		return TRUE;
	reqExts = strdup(origReqExts);
	reqExtsLen = strlen(reqExts);
	if (NULL == unsupportedExts) {
		unsupportedExts = (char*)malloc(reqExtsLen + 1);
	} else if (reqExtsLen > strlen(unsupportedExts)) {
		unsupportedExts = (char*)realloc(unsupportedExts, reqExtsLen + 1);
	}
	CHECK_MEMORY(unsupportedExts);
	*unsupportedExts = 0;

	// Parse requested extension list
	for (reqExt = reqExts;
		(reqExt = (char*)EatWhiteSpace(reqExt)) && *reqExt;
		reqExt = (char*)EatNonWhiteSpace(reqExt))
	{
		char *extEnd = (char*)EatNonWhiteSpace(reqExt);
		char saveChar = *extEnd;
		*extEnd = (char)0;
		if (!ExtensionExists(reqExt, sysExts) ||
			!glh_init_extension(reqExt)) {
			// add reqExt to end of unsupportedExts
			strcat(unsupportedExts, reqExt);
			strcat(unsupportedExts, " ");
			success = FALSE;
		}
		*extEnd = saveChar;
	}
	free(reqExts);
	return success;
}

const char* glh_get_unsupported_extensions()
{
	return (const char*)unsupportedExts;
}

#else
int glh_init_extensions(const char *origReqExts);
const char* glh_get_unsupported_extensions();
#endif /* GLH_EXT_SINGLE_FILE */

#ifdef __cplusplus
}
#endif

#endif /* GLH_EXTENSIONS */
