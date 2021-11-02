#pragma once

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified
#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed

#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26488) // Do not dereference a potentially null pointer : '...'. '...' was null at line ... (lifetime.1).
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all(c.21).
#pragma warning(disable: 26456) // Operator '...' hides a non - virtual operator '...' (c.128).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid (lifetime.4).
#pragma warning(disable: 26401) // Do not delete a raw pointer that is not an owner<T>(i.11).
#pragma warning(disable: 26482) // Only index into arrays using constant expressions(bounds.2).
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...' (c.128).
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
#pragma warning(disable: 26458) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).

#pragma warning(push)
#pragma warning(disable: 4464) // relative include path contains '..'
#include "../../nitro_pch.h"
#pragma warning(pop)

#pragma warning(disable: 26400) // Do not assign the result of an allocation or a function call with an owner<T> return value to a raw pointer, use owner<T> instead(i.11).
#pragma warning(disable: 26052) // Potentially unconstrained access using expression '...' ...
