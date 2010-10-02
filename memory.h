/**
 * MEMORY.H - memory allocation : help to find memory leaks
 *
 * @author    Marc Gueury
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#ifndef __MEMORY_H
#define __MEMORY_H

#ifdef WIN32
  #ifdef _DEBUG
    typedef const char * LPCSTR;
    void * __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
    void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);
    #define new new(__FILE__, __LINE__)
  #endif
#endif

#endif // __MEMORY_H

