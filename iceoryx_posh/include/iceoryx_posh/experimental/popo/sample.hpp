// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
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

#ifndef IOX_EXPERIMENTAL_POSH_POPO_SAMPLE_HPP
#define IOX_EXPERIMENTAL_POSH_POPO_SAMPLE_HPP

#include "iceoryx_posh/mepoo/chunk_header.hpp"
#include "iceoryx_utils/cxx/unique_ptr.hpp"

namespace iox
{
namespace popo
{
template <typename T>
class Sample
{
  public:
    Sample(cxx::unique_ptr<T>&& samplePtr) noexcept;

    /// Creates an empty sample.
    Sample(std::nullptr_t) noexcept;

    Sample(const Sample<T>&) = delete;
    Sample<T>& operator=(const Sample<T>&) = delete;

    Sample<T>& operator=(Sample<T>&& rhs);
    Sample(Sample<T>&& rhs);
    ~Sample();

    ///
    /// @brief operator = Clear the sample.
    /// @return
    ///
    Sample<T>& operator=(std::nullptr_t) noexcept;

    ///
    /// @brief operator -> Transparent access to the underlying pointer.
    /// @return
    ///
    T* operator->() const noexcept;

    ///
    /// @brief allocation Access to the memory allocated to the sample.
    /// @return
    ///
    T* get() const noexcept;

    ///
    /// @brief header Retrieve the header of the underlying memory chunk used by the sample.
    /// @return The ChunkHeader of the underlying memory chunk.
    ///
    const mepoo::ChunkHeader* header() const noexcept;

  protected:
    cxx::unique_ptr<T> m_samplePtr{nullptr};
};

} // namespace popo
} // namespace iox

#include "iceoryx_posh/experimental/internal/popo/sample.inl"

#endif // IOX_EXPERIMENTAL_POSH_POPO_SAMPLE_HPP
