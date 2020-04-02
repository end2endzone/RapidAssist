/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef RA_MACROS_H
#define RA_MACROS_H

#include "rapidassist/config.h"

#define SAFE_DELETE(var)        {if (var) delete   var; var = NULL;}
#define SAFE_DELETE_ARRAY(var)  {if (var) delete[] var; var = NULL;}

//Function deprecation support.
//See https://stackoverflow.com/questions/295120/c-mark-as-deprecated
#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

#ifdef _MSC_VER

/// <summary>
/// Disables a Visual Studio warning code.
/// </summary>
/// <example> 
/// The following code example shows an example of how SAFE_WARNING_DISABLE and SAFE_WARNING_RESTORE are expected to be used.
/// <code>
///   SAFE_WARNING_DISABLE(4244);
///   float a = 5.0;
///   float b = 2.45f;
///   int c = a/b;  //warning C4244: 'initializing' : conversion from 'float' to 'int', possible loss of data
///   SAFE_WARNING_RESTORE();
/// </code>
/// </example>
#define SAFE_WARNING_DISABLE(value) __pragma( warning(push) ) __pragma( warning(disable: value)) 

/// <summary>
/// Returns the Visual Studio warning code back to its previous value.
/// </summary>
#define SAFE_WARNING_RESTORE() __pragma( warning(pop) )

#else
#   define SAFE_WARNING_DISABLE(value) ;
#   define SAFE_WARNING_RESTORE() ;
#endif


#endif //RA_MACROS_H