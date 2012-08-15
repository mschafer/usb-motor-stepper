#ifndef UMS_DLL_hpp
#define UMS_DLL_hpp

/* Cmake will define ums_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define ums_EXPORTS when
building a DLL on windows.
*/
// We are using the Visual Studio Compiler and building Shared libraries

#if defined (_WIN32) 
  // VS will warn about every boost and std template not having dll interface w/o these
  #pragma warning( disable: 4251 )
  #pragma warning( disable: 4275 )
  #if defined(ums_EXPORTS)
    #define  UMS_API __declspec(dllexport)
  #else
    #define  UMS_API __declspec(dllimport)
  #endif
#else
 #define UMS_API
#endif

#endif
