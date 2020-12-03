// Copyright (c) 2020 by Apex.AI Inc. All rights reserved.
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

#include "test.hpp"

#include "iceoryx_utils/concurrent/resizeable_lockfree_queue.hpp"
using namespace ::testing;

//*********************************************************
// Test the added functionaility of ResizeableLockFreeQueue
// to change the capacity (setCapacity).
// The remaining functionality is identical to LockFreeQueue
// and tested in test_lockfree_queue.cpp (as a typed test).
//*********************************************************

namespace
{
// use a non-POD type for testing (just a boxed version of int)
struct Integer
{
    Integer(uint64_t value = 0)
        : value(value)
    {
    }

    uint64_t value{0};

    // so that it behaves like an int for comparison purposes
    operator uint64_t() const
    {
        return value;
    }
};

template <typename T>
class ResizeableLockFreeQueueTest : public ::testing::Test
{
  protected:
    ResizeableLockFreeQueueTest()
    {
    }

    ~ResizeableLockFreeQueueTest()
    {
    }

    void SetUp()
    {
    }

    void TearDown()
    {
    }

    void fillQueue(uint64_t start = 0)
    {
        uint64_t element{start};
        for (uint64_t i = 0; i < queue.capacity(); ++i)
        {
            queue.tryPush(element);
            element++;
        }
    }

    using Queue = T;
    Queue queue;
};

template <size_t Capacity>
using IntegerQueue = iox::concurrent::ResizeableLockFreeQueue<Integer, Capacity>;

template <size_t Capacity>
using IntQueue = iox::concurrent::ResizeableLockFreeQueue<uint64_t, Capacity>;

typedef ::testing::Types<IntegerQueue<1>, IntegerQueue<10>, IntQueue<10>> TestQueues;


/// we require TYPED_TEST since we support gtest 1.8 for our safety targets
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
TYPED_TEST_CASE(ResizeableLockFreeQueueTest, TestQueues);
#pragma GCC diagnostic pop

TEST(ResizeableLockFreeQueueTest, maxCapacityIsConsistent)
{
    using Queue = IntegerQueue<37>;
    EXPECT_EQ(Queue::maxCapacity(), 37);
}

TYPED_TEST(ResizeableLockFreeQueueTest, initialCapacityIsMaximalbyDefault)
{
    using Queue = typename TestFixture::Queue;
    auto& q = this->queue;
    EXPECT_EQ(q.capacity(), q.maxCapacity());
    EXPECT_EQ(q.capacity(), Queue::maxCapacity());
}

TYPED_TEST(ResizeableLockFreeQueueTest, constructWithMaxCapacity)
{
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    typename TestFixture::Queue q(MAX_CAP);
    EXPECT_EQ(q.capacity(), q.maxCapacity());
}

TYPED_TEST(ResizeableLockFreeQueueTest, constructWithMoreThanMaxCapacitySaturatesAtMaxCapacity)
{
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    typename TestFixture::Queue q(MAX_CAP + 1);
    EXPECT_EQ(q.capacity(), q.maxCapacity());
}

TYPED_TEST(ResizeableLockFreeQueueTest, constructWithNoCapacity)
{
    typename TestFixture::Queue q(0);
    EXPECT_EQ(q.capacity(), 0);
}

TYPED_TEST(ResizeableLockFreeQueueTest, constructWithHalfOfMaxCapacity)
{
    constexpr auto cap = TestFixture::Queue::MAX_CAPACITY / 2;
    typename TestFixture::Queue q(cap);
    EXPECT_EQ(q.capacity(), cap);
}

TYPED_TEST(ResizeableLockFreeQueueTest, decreaseCapacityToZeroOneByOne)
{
    auto& q = this->queue;
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;

    auto i = MAX_CAP;
    while (i > 0)
    {
        EXPECT_TRUE(q.setCapacity(--i));
        ASSERT_EQ(q.capacity(), i);
    }
}

TYPED_TEST(ResizeableLockFreeQueueTest, increaseToMaxCapacity)
{
    typename TestFixture::Queue q(0);
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    EXPECT_EQ(q.capacity(), 0);
    EXPECT_TRUE(q.setCapacity(MAX_CAP));
    EXPECT_EQ(q.capacity(), MAX_CAP);
}

TYPED_TEST(ResizeableLockFreeQueueTest, increaseToMaxCapacityOneByOne)
{
    typename TestFixture::Queue q(0);
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    EXPECT_EQ(q.capacity(), 0);

    for (uint64_t i = 0; i < MAX_CAP;)
    {
        EXPECT_TRUE(q.setCapacity(++i));
        ASSERT_EQ(q.capacity(), i);
    }
}

TYPED_TEST(ResizeableLockFreeQueueTest, setCapacityToZero)
{
    auto& q = this->queue;
    EXPECT_TRUE(q.setCapacity(0));
    EXPECT_EQ(q.capacity(), 0);
}

TYPED_TEST(ResizeableLockFreeQueueTest, setCapacityToOne)
{
    auto& q = this->queue;
    EXPECT_TRUE(q.setCapacity(1));
    EXPECT_EQ(q.capacity(), 1);
}

TYPED_TEST(ResizeableLockFreeQueueTest, setCapacityToMaxCapacity)
{
    typename TestFixture::Queue q(0);
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    EXPECT_TRUE(q.setCapacity(MAX_CAP));
    EXPECT_EQ(q.capacity(), MAX_CAP);
}

TYPED_TEST(ResizeableLockFreeQueueTest, setCapacityToHalfOfMaxCapacityAndFillIt)
{
    auto& q = this->queue;
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    uint64_t newCap = MAX_CAP / 2;
    EXPECT_EQ(q.setCapacity(newCap), true);
    EXPECT_EQ(q.capacity(), newCap);

    uint64_t element = 0;
    while (q.tryPush(element++))
        ;

    EXPECT_EQ(q.capacity(), newCap);
    EXPECT_EQ(q.size(), newCap);
    EXPECT_EQ(element, newCap + 1);
}

TYPED_TEST(ResizeableLockFreeQueueTest, setCapacityFromHalfOfMaxCapacityToMaxCapacity)
{
    auto& q = this->queue;
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    uint64_t cap = MAX_CAP / 2;
    EXPECT_EQ(q.setCapacity(cap), true);
    EXPECT_EQ(q.capacity(), cap);

    uint64_t element = 0;
    while (q.tryPush(element++))
        ;

    EXPECT_EQ(q.capacity(), cap);
    EXPECT_EQ(q.size(), cap);
    EXPECT_EQ(element, cap + 1);

    EXPECT_EQ(q.setCapacity(MAX_CAP), true);
    EXPECT_EQ(q.capacity(), MAX_CAP);
    EXPECT_EQ(q.size(), cap);

    --element; // compensate the last failed tryPush
    while (q.tryPush(element++))
        ;

    // we want to find all elements we pushed
    for (element = 0; element < MAX_CAP; ++element)
    {
        auto result = q.pop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), element);
    }
}

TYPED_TEST(ResizeableLockFreeQueueTest, setCapacityOfFullQueueToHalfOfMaxCapacity)
{
    auto& q = this->queue;
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;
    uint64_t cap = MAX_CAP / 2;

    uint64_t element = 0;
    while (q.tryPush(element++))
        ;
    EXPECT_EQ(q.capacity(), MAX_CAP);
    EXPECT_EQ(q.size(), MAX_CAP);

    EXPECT_TRUE(q.setCapacity(cap));
    EXPECT_EQ(q.capacity(), cap);
    EXPECT_EQ(q.size(), cap);

    if (cap == 0)
    {
        return;
    }
    // the least recent values are removed due to the capacity being decreased
    for (element = cap; element < MAX_CAP; ++element)
    {
        auto result = q.pop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), element);
    }
}

// note this is one of the most general cases and necessary to test:
// decreasing the capacity starting with a partially filled queue and checking whether the last values
// remain (and the others are removed)
TYPED_TEST(ResizeableLockFreeQueueTest, DecreaseCapacityOfAPartiallyFilledQueue)
{
    auto& q = this->queue;
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;

    iox::cxx::vector<int, MAX_CAP> removedElements;
    using element_t = typename TestFixture::Queue::element_t;
    auto removeHandler = [&](const element_t& value) { removedElements.emplace_back(std::move(value)); };

    uint64_t cap = MAX_CAP / 2;

    EXPECT_TRUE(q.setCapacity(cap));
    EXPECT_EQ(q.capacity(), cap);

    uint64_t element = 0;
    while (q.tryPush(element++))
        ;

    EXPECT_EQ(q.capacity(), cap);
    EXPECT_EQ(q.size(), cap);

    auto cap2 = cap + MAX_CAP / 4; // roughly 3 quarters of max (integer division)
    EXPECT_TRUE(q.setCapacity(cap2));

    EXPECT_EQ(q.capacity(), cap2);
    EXPECT_EQ(q.size(), cap);

    auto cap3 = cap2 - cap; // roughly a quarter of max

    EXPECT_TRUE(q.setCapacity(cap3, removeHandler));
    EXPECT_EQ(q.capacity(), cap3);
    EXPECT_EQ(q.size(), cap3);

    // cap3 elements remain, the first cap - cap3 elements are removed

    // were the least recent elements removed?
    EXPECT_EQ(removedElements.size(), cap - cap3);
    element = 0;
    for (auto& removedElement : removedElements)
    {
        EXPECT_EQ(removedElement, element++);
    }

    // are the remaining elements correct? (i.e. we did not remove too many elements)
    for (element = cap - cap3; element < cap; ++element)
    {
        auto result = q.pop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), element);
    }

    // refill to verify the capacity can really be used

    element = 0;
    while (q.tryPush(element++))
        ;

    for (element = 0; element < cap3; ++element)
    {
        auto result = q.pop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), element);
    }
}

TYPED_TEST(ResizeableLockFreeQueueTest, DecreaseCapacityOfAPartiallyFilledQueueWithRemoveHandler)
{
    using element_t = typename TestFixture::Queue::element_t;
    auto& q = this->queue;
    constexpr auto MAX_CAP = TestFixture::Queue::MAX_CAPACITY;

    iox::cxx::vector<int, MAX_CAP> removedElements;
    auto removeHandler = [&](const element_t& value) { removedElements.push_back(std::move(value)); };

    uint64_t cap = MAX_CAP / 2;

    EXPECT_TRUE(q.setCapacity(cap));
    EXPECT_EQ(q.capacity(), cap);

    uint64_t element = 0;
    while (q.tryPush(element++))
        ;

    EXPECT_EQ(q.capacity(), cap);
    EXPECT_EQ(q.size(), cap);

    auto cap2 = cap + MAX_CAP / 4; // roughly 3 quarters of max (integer division)
    EXPECT_TRUE(q.setCapacity(cap2));

    EXPECT_EQ(q.capacity(), cap2);
    EXPECT_EQ(q.size(), cap);

    auto cap3 = cap2 - cap; // roughly a quarter of max

    EXPECT_TRUE(q.setCapacity(cap3, removeHandler));
    EXPECT_EQ(q.capacity(), cap3);
    EXPECT_EQ(q.size(), cap3);

    // cap3 elements remain, the first cap - cap3 elements are removed

    // were the least recent elements removed?
    EXPECT_EQ(removedElements.size(), cap - cap3);
    element = 0;
    for (auto& removedElement : removedElements)
    {
        EXPECT_EQ(removedElement, element++);
    }

    // are the remaining elements correct? (i.e. we did not remove too many elements)
    for (element = cap - cap3; element < cap; ++element)
    {
        auto result = q.pop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), element);
    }

    // refill to verify the capacity can really be used

    element = 0;
    while (q.tryPush(element++))
        ;

    for (element = 0; element < cap3; ++element)
    {
        auto result = q.pop();
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), element);
    }
}

} // namespace
