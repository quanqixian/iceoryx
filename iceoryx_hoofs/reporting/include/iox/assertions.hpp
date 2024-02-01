// Copyright (c) 2023 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_HOOFS_REPORTING_ASSERTIONS_HPP
#define IOX_HOOFS_REPORTING_ASSERTIONS_HPP

#include "iox/error_reporting/configuration.hpp"
#include "iox/error_reporting/error_forwarding.hpp"

#include "iox/error_reporting/source_location.hpp"

// ***
// * Define public assertion API
// ***

// NOLINTBEGIN(cppcoreguidelines-macro-usage) source location requires macros

// The following macros are statements (not expressions).
// This is important, as it enforces correct use to some degree.
// For example they cannot be used as function arguments and must be terminated with a ';'.
//
// Note: once source location becomes available without macro usage this could (and arguably should)
// be transformed into a function API.

/// @brief calls panic handler and does not return
/// @param message message to be forwarded
/// @note could actually throw if desired without breaking control flow asssumptions
#define IOX_PANIC(message) iox::er::forwardPanic(IOX_CURRENT_SOURCE_LOCATION, message)

//************************************************************************************************
//* For documentation of intent, defensive programming and debugging
//*
//* There are no error codes/errors required here on purpose, as it would make the use cumbersome.
//* Instead a special internal error type is used.
//************************************************************************************************

/// @brief only for debug builds: report fatal assert violation if expr evaluates to false
/// @note for conditions that should not happen with correct use
/// @param expr boolean expression that must hold
/// @param message message to be forwarded in case of violation
#define IOX_ASSERT(expr, message)                                                                                      \
    if (iox::er::Configuration::CHECK_ASSERT && !(expr))                                                               \
    {                                                                                                                  \
        iox::er::forwardFatalError(iox::er::Violation::createAssertViolation(),                                        \
                                   iox::er::ASSERT_VIOLATION,                                                          \
                                   IOX_CURRENT_SOURCE_LOCATION,                                                        \
                                   message);                                                                           \
    }                                                                                                                  \
    [] {}() // the empty lambda forces a semicolon on the caller side

/// @brief report fatal enforce violation if expr evaluates to false
/// @note for conditions that may actually happen during correct use
/// @param expr boolean expression that must hold
/// @param message message to be forwarded in case of violation
#define IOX_ENFORCE(expr, message)                                                                                     \
    if (!(expr))                                                                                                       \
    {                                                                                                                  \
        iox::er::forwardFatalError(iox::er::Violation::createEnforceViolation(),                                       \
                                   iox::er::ENFORCE_VIOLATION,                                                         \
                                   IOX_CURRENT_SOURCE_LOCATION,                                                        \
                                   message); /* @todo iox-#1032 add strigified 'expr' as '#expr' */                    \
    }                                                                                                                  \
    [] {}() // the empty lambda forces a semicolon on the caller side

/// @brief panic if control flow reaches this code at runtime
#define IOX_UNREACHABLE()                                                                                              \
    iox::er::forwardPanic(IOX_CURRENT_SOURCE_LOCATION, "Reached code that was supposed to be unreachable.")


/// @todo iox-#1032 replace IOX_EXPECTS and IOX_ENSURES with IOX_ENFORCE
// implementing C++ Core Guideline, I.6. Prefer Expects
// see:
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Ri-expects
// NOLINTJUSTIFICATION array decay: needed for source code location, safely wrapped in macro
// NOLINTBEGIN(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define IOX_EXPECTS(condition) IOX_ENFORCE(condition, "")
// NOLINTEND(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)

// NOLINTJUSTIFICATION array decay: needed for source code location, safely wrapped in macro
// NOLINTBEGIN(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define IOX_EXPECTS_WITH_MSG(condition, msg) IOX_ENFORCE(condition, msg)
// NOLINTEND(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)

// implementing C++ Core Guideline, I.8. Prefer Ensures
// see:
// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Ri-ensures
// NOLINTJUSTIFICATION array decay: needed for source code location, safely wrapped in macro
// NOLINTBEGIN(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define IOX_ENSURES(condition) IOX_ENFORCE(condition, "")
// NOLINTEND(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)

// NOLINTJUSTIFICATION array decay: needed for source code location, safely wrapped in macro
// NOLINTBEGIN(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#define IOX_ENSURES_WITH_MSG(condition, msg) IOX_ENFORCE(condition, msg)
// NOLINTEND(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay)


// NOLINTEND(cppcoreguidelines-macro-usage)

#endif // IOX_HOOFS_REPORTING_ASSERTIONS_HPP
