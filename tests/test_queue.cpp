#include <gtest/gtest.h>

#include "ThreadSafeQueue.h"

#include <chrono>
#include <string>
#include <thread>

using namespace vehiclesim;

TEST(QueueTests, PreservesPushOrder) {
    ThreadSafeQueue<int> queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);

    int value = 0;
    EXPECT_TRUE(queue.waitPop(value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(queue.waitPop(value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(queue.waitPop(value));
    EXPECT_EQ(value, 3);
}

TEST(QueueTests, DrainsRemainingItemsBeforeReportingClosed) {
    ThreadSafeQueue<std::string> queue;
    queue.push("first");
    queue.close();

    std::string value;
    EXPECT_TRUE(queue.waitPop(value));
    EXPECT_EQ(value, "first");
    EXPECT_FALSE(queue.waitPop(value));
    EXPECT_TRUE(queue.isClosed());
}

TEST(QueueTests, IgnoresPushesAfterClose) {
    ThreadSafeQueue<int> queue;
    queue.close();
    queue.push(42);

    int value = 0;
    EXPECT_FALSE(queue.waitPop(value));
}

TEST(QueueTests, CloseUnblocksWaitingConsumer) {
    ThreadSafeQueue<int> queue;
    bool receivedValue = true;

    std::thread consumer([&queue, &receivedValue] {
        int value = 0;
        receivedValue = queue.waitPop(value);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    queue.close();
    consumer.join();

    EXPECT_FALSE(receivedValue);
}
