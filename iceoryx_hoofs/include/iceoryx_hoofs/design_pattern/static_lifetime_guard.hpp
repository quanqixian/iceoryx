// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
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

#ifndef IOX_HOOFS_DESIGN_STATIC_LIFETIME_GUARD_HPP
#define IOX_HOOFS_DESIGN_STATIC_LIFETIME_GUARD_HPP

#include <atomic>
#include <type_traits>

namespace iox
{
namespace design_pattern
{
/// @brief Manages a static instance of type T in a way so that each
/// existing StaticLifetimeGuard prevents the destruction of
/// the instance.
/// 1. instance() creates a static guard itself and hence has static lifetime
/// 2. any static StaticLifetimeGuard G created before that prolongs the lifetime
/// of the instance at least until G is destroyed
/// @tparam T the type of the instance to be guarded
///
/// @note dtor, ctor and copy ctor and instance are thread-safe
///
/// @code
/// // instance will be destroyed after guard
/// // (or later if there are guards preceding guard in construction order)
///
/// static StaticLifetimeGuard<T> guard;
/// static T& instance = StaticLifetimeGuard<T>::instance();
/// @endcode
template <typename T>
class StaticLifetimeGuard
{
  public:
    StaticLifetimeGuard() noexcept;

    StaticLifetimeGuard(const StaticLifetimeGuard&) noexcept;

    ~StaticLifetimeGuard() noexcept;

    // move and assignment have no purpose since the objects have no state,
    // copy exists to support passing/returning a value object
    StaticLifetimeGuard(StaticLifetimeGuard&&) = delete;
    StaticLifetimeGuard& operator=(const StaticLifetimeGuard&) = delete;
    StaticLifetimeGuard& operator=(StaticLifetimeGuard&&) = delete;

    /// @brief Construct the instance to be guarded with constructor arguments.
    /// @param args constructor arguments
    /// @return reference to the constructed instance or the existing instance
    /// if it already exists
    /// @note creates an implicit static StaticLifetimeGuard to ensure
    /// the instance is destroyed in the static destruction phase
    template <typename... Args>
    static T& instance(Args&&... args) noexcept;

    /// @brief Set the instance life time count.
    /// @param count value to be set
    /// @return previous count value
    /// @note This can be used to additionally extend or shorten the instance lifetime,
    /// This has to be done carefully, to ensure destruction or prevent early destruction.
    /// It is useful for testing purposes.
    static uint64_t setCount(uint64_t count);

    /// @brief Get the current count value.
    /// @return current count value
    static uint64_t count();

  private:
    using storage_t = typename std::aligned_storage_t<sizeof(T), alignof(T)>;

    static constexpr uint32_t UNINITIALIZED{0};
    static constexpr uint32_t INITALIZING{1};
    static constexpr uint32_t INITALIZED{2};

    // NOLINTJUSTIFICATION these static variables are private and mutability is required
    // NOLINTBEGIN (cppcoreguidelines-avoid-non-const-global-variables)
    static storage_t s_storage;
    static std::atomic<uint64_t> s_count;
    static std::atomic<uint32_t> s_instanceState;
    static T* s_instance;
    // NOLINTEND (cppcoreguidelines-avoid-non-const-global-variables)

    std::atomic<uint64_t> m_value{1};

    static void destroy();
};

// NOLINTJUSTIFICATION these static variables are private and mutability is required
// NOLINTBEGIN (cppcoreguidelines-avoid-non-const-global-variables)
template <typename T>
typename StaticLifetimeGuard<T>::storage_t StaticLifetimeGuard<T>::s_storage;
template <typename T>
std::atomic<uint64_t> StaticLifetimeGuard<T>::s_count{0};
template <typename T>
std::atomic<uint32_t> StaticLifetimeGuard<T>::s_instanceState{UNINITIALIZED};
template <typename T>
T* StaticLifetimeGuard<T>::s_instance{nullptr};
// NOLINTEND (cppcoreguidelines-avoid-non-const-global-variables)

} // namespace design_pattern
} // namespace iox

#include "iceoryx_hoofs/internal/design_pattern/static_lifetime_guard.inl"

#endif // IOX_HOOFS_DESIGN_STATIC_LIFETIME_GUARD_HPP
