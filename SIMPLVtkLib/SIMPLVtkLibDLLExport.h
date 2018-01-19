/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _SIMPLVtkLibdllexport_h_
#define _SIMPLVtkLibdllexport_h_


#if defined (_MSC_VER)
#pragma warning(disable: 4267)
#pragma warning(disable: 4800) /*  warning C4800: 'double' : forcing value to bool 'true' or 'false' */
#endif


/* Cmake will define SIMPLVtkLib_EXPORTS on Windows when it
configures to build a shared library. If you are going to use
another build system on windows or create the visual studio
projects by hand you need to define SIMPLVtkLib_EXPORTS when
building the MXADatModel DLL on windows.
*/

#if defined (SIMPLVtkLib_BUILT_AS_DYNAMIC_LIB)

#if defined (SIMPLVtkLib_EXPORTS)  /* Compiling the MXA DLL/Dylib */
#if defined (_MSC_VER)  /* MSVC Compiler Case */
#define  SIMPLVtkLib_EXPORT __declspec(dllexport)
#elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
#define SIMPLVtkLib_EXPORT __attribute__ ((visibility("default")))
#endif
#else  /* Importing the DLL into another project */
#if defined (_MSC_VER)  /* MSVC Compiler Case */
#define  SIMPLVtkLib_EXPORT __declspec(dllimport)
#elif (__GNUC__ >= 4)  /* GCC 4.x has support for visibility options */
#define SIMPLVtkLib_EXPORT __attribute__ ((visibility("default")))
#endif
#endif
#endif

/* If SIMPLVtkLib_EXPORT was never defined, define it here */
#ifndef SIMPLVtkLib_EXPORT
#define SIMPLVtkLib_EXPORT
#endif

#if 0
#if defined (_WIN32) || defined __CYGWIN__

#if defined (DREAM3D_BUILT_AS_DYNAMIC_LIB)
#if defined(SIMPLVtkLib_EXPORTS)
#define  SIMPLVtkLib_EXPORT __declspec(dllexport)
#else
#define  SIMPLVtkLib_EXPORT __declspec(dllimport)
#endif /* SIMPLVtkLib_EXPORTS */

#else
#define SIMPLVtkLib_EXPORT
#endif
#elif __GNUC__ >= 4
#define FLOW_DLL __attribute__ ((visibility("default")))
#define DLL_LOCAL  __attribute__ ((visibility("hidden")
#else /* defined (_WIN32) && defined (DREAM3D_BUILD_SHARED_LIBS)  */
#define SIMPLVtkLib_EXPORT
#endif
#endif

#endif /* _SIMPLVtkLib_DLL_EXPORT_H_ */

