#include <catch2/catch_shim.hpp>

#include "lru_cache.h"

#include <random>

TEST_CASE("Basic")
{
    LRUCache cache(3);

    SECTION("Empty operations")
    {
        REQUIRE(cache.size() == 0);
        REQUIRE(cache.capacity() == 3);
        REQUIRE(!cache.get("nothing").has_value());
        REQUIRE(cache.erase("nothing") == false);
    }

    SECTION("Basic put and get")
    {
        REQUIRE(cache.put("1", 100) == true);
        REQUIRE(cache.size() == 1);

        auto value = cache.get("1");

        REQUIRE(value.has_value());
        REQUIRE(*value == 100);
    }

    SECTION("Value updates preserves order")
    {
        cache.put("1", 1);
        cache.put("2", 2);
        cache.put("3", 3);

        // update existing key
        REQUIRE(cache.put("1", 100) == false);  // false because key exists
        REQUIRE(cache.get("1").value() == 100);

        // key '1' should now be most recent
        cache.put("4", 4);  // should pop key '2' (oldest)

        REQUIRE(!cache.get("2").has_value());
        REQUIRE(cache.get("1").has_value());
        REQUIRE(cache.get("3").has_value());
        REQUIRE(cache.get("4").has_value());
    }
}

TEST_CASE("Pop operation")
{
    SECTION("Exact capacity behavior")
    {
        LRUCache cache(2);

        cache.put("a", 1);
        cache.put("b", 2);

        REQUIRE(cache.size() == 2);

        cache.put("c", 3);  // should pop 'a'

        REQUIRE(!cache.get("a").has_value());
        REQUIRE(cache.get("b").value() == 2);
        REQUIRE(cache.get("c").value() == 3);
    }

    SECTION("Access pattern affects to pop")
    {
        LRUCache cache(3);

        cache.put("a", 1);
        cache.put("b", 2);
        cache.put("c", 3);

        // makes 'a' most recently used
        cache.get("a");

        // add new element - should evict 'b' (least recently used)
        cache.put("d", 4);

        REQUIRE(!cache.get("b").has_value());
        REQUIRE(cache.get("a").has_value());
        REQUIRE(cache.get("c").has_value());
        REQUIRE(cache.get("d").has_value());
    }

    SECTION("Complex access pattern")
    {
        LRUCache cache(4);

        cache.put("a", 1);
        cache.put("b", 2);
        cache.put("c", 3);
        cache.put("d", 4);

        // access sequence: 'b', 'c', 'd', 'a'
        cache.get("b");
        cache.get("c");
        cache.get("d");
        cache.get("a");

        // now order should be: 'a', 'd', 'c', 'b'
        cache.put("e", 5);  // should pop 'b'

        REQUIRE(!cache.get("b").has_value());
        REQUIRE(cache.get("a").has_value());
        REQUIRE(cache.get("c").has_value());
        REQUIRE(cache.get("d").has_value());
        REQUIRE(cache.get("e").has_value());
    }
}

TEST_CASE("Pinning")
{
    LRUCache cache(3);

    SECTION("Pin protects from pop")
    {
        cache.put("pinned", 1);
        cache.put("1", 2);
        cache.put("2", 3);

        REQUIRE(cache.pin("pinned") == true);

        // this should pop '1' (oldest unpinned)
        cache.put("new", 4);

        REQUIRE(!cache.get("1").has_value());
        REQUIRE(cache.get("pinned").has_value());
        REQUIRE(cache.get("2").has_value());
        REQUIRE(cache.get("new").has_value());
    }

    SECTION("Multiple pins behavior")
    {
        cache.put("pinned1", 1);
        cache.put("pinned2", 2);
        cache.put("3", 3);

        cache.pin("pinned1");
        cache.pin("pinned2");

        REQUIRE(cache.put("1", 4) == true);
        REQUIRE(!cache.get("3").has_value());
        REQUIRE(cache.get("1").has_value());

        REQUIRE(cache.put("2", 5) == true);
        REQUIRE(!cache.get("1").has_value());
        REQUIRE(cache.get("2").has_value());

        REQUIRE(cache.get("pinned1").has_value());
        REQUIRE(cache.get("pinned2").has_value());
    }

    SECTION("Pin/unpin lifecycle")
    {
        cache.put("key", 1);

        REQUIRE(cache.pin("key") == true);
        REQUIRE(cache.pin("key") == false);  // already pinned

        REQUIRE(cache.unpin("key") == true);
        REQUIRE(cache.unpin("key") == false);  // already unpinned

        cache.put("a", 2);
        cache.put("b", 3);
        cache.put("c", 4);  // should pop 'key'

        REQUIRE(!cache.get("key").has_value());
    }

    SECTION("Pin affects LRU order")
    {
        cache.put("a", 1);
        cache.put("b", 2);
        cache.put("c", 3);

        cache.pin("b");
        cache.get("a");

        // should pop 'c', not 'a'
        cache.put("d", 4);

        REQUIRE(!cache.get("c").has_value());
        REQUIRE(cache.get("a").has_value());
        REQUIRE(cache.get("b").has_value());
        REQUIRE(cache.get("d").has_value());
    }
}

TEST_CASE("Merging")
{
    SECTION("Basic merge")
    {
        LRUCache cache1(5);

        cache1.put("1", 1);
        cache1.put("2", 2);
        cache1.pin("1");

        LRUCache cache2(3);

        cache2.put("3", 3);
        cache2.put("4", 4);

        cache1.merge(cache2);

        REQUIRE(cache1.size() == 4);
        REQUIRE(*cache1.get("1") == 1);
        REQUIRE(*cache1.get("2") == 2);
        REQUIRE(*cache1.get("3") == 3);
        REQUIRE(*cache1.get("4") == 4);
    }

    SECTION("Merge with duplicates")
    {
        LRUCache cache1(3);

        cache1.put("common", 100);
        cache1.put("unique1", 1);

        LRUCache cache2(3);

        cache2.put("common", 200);  // different value
        cache2.put("unique2", 2);

        cache1.merge(cache2);

        // original value should be preserved
        REQUIRE(cache1.get("common").value() == 100);
        REQUIRE(cache1.get("unique1").has_value());
        REQUIRE(cache1.get("unique2").has_value());
        REQUIRE(cache1.size() == 3);
    }

    SECTION("Merge with pop")
    {
        LRUCache cache1(3);

        cache1.put("a", 1);

        LRUCache cache2(3);

        cache2.put("b", 2);
        cache2.put("c", 3);
        cache2.put("d", 4);

        cache1.merge(cache2);  // should pop one element

        REQUIRE(cache1.size() == 3);
        REQUIRE(!cache1.get("a").has_value());  // 'a' was popped (oldest)
        REQUIRE(cache1.get("b").has_value());
        REQUIRE(cache1.get("c").has_value());
        REQUIRE(cache1.get("d").has_value());
    }

    SECTION("Self-merge")
    {
        LRUCache cache(3);

        cache.put("key", 1);

        cache.merge(cache);  // should do nothing

        REQUIRE(cache.size() == 1);
        REQUIRE(cache.get("key").value() == 1);
    }
}

TEST_CASE("Edge cases")
{
    SECTION("Capacity=0 cache")
    {
        LRUCache cache(0);

        REQUIRE(cache.put("key", 1) == false);
        REQUIRE(!cache.get("key").has_value());
        REQUIRE(cache.erase("key") == false);
        REQUIRE(cache.size() == 0);
    }

    SECTION("Capacity=1 cache")
    {
        LRUCache cache(1);

        REQUIRE(cache.put("first", 1) == true);
        REQUIRE(cache.get("first").value() == 1);

        REQUIRE(cache.put("second", 2) == true);  // pops 'first'
        REQUIRE(!cache.get("first").has_value());
        REQUIRE(cache.get("second").value() == 2);
    }

    SECTION("Mass operations")
    {
        LRUCache cache(1000);

        for (int i = 0; i < 1000; ++i)
        {
            REQUIRE(cache.put("key" + std::to_string(i), i) == true);
        }

        REQUIRE(cache.size() == 1000);

        for (int i = 0; i < 100; ++i)
        {
            auto val = cache.get("key" + std::to_string(i * 10));
            REQUIRE(val.has_value());
            REQUIRE(*val == i * 10);
        }

        for (int i = 1000; i < 1100; ++i)
        {
            cache.put("key" + std::to_string(i), i);
        }

        REQUIRE(cache.size() == 1000);

        // first 100 should be popped (except those accessed)
        int found = 0;
        for (int i = 0; i < 100; ++i)
        {
            if (cache.get("key" + std::to_string(i)).has_value())
            {
                found++;
            }
        }
        REQUIRE(found < 20);  // most should be popped
    }
}

TEST_CASE("Error Conditions")
{
    LRUCache cache(2);

    SECTION("Invalid operations")
    {
        REQUIRE(cache.pin("nothing") == false);
        REQUIRE(cache.unpin("nothing") == false);
        REQUIRE(cache.erase("nothing") == false);

        cache.put("key", 1);

        REQUIRE(cache.pin("key") == true);
        REQUIRE(cache.pin("key") == false);  // already pinned
        REQUIRE(cache.unpin("key") == true);
        REQUIRE(cache.unpin("key") == false);  // already unpinned
    }

    SECTION("Put on full cache with only pinned")
    {
        cache.put("pinned1", 1);
        cache.put("pinned2", 2);
        cache.pin("pinned1");
        cache.pin("pinned2");

        REQUIRE(cache.put("new", 3) == false);
        REQUIRE(!cache.get("new").has_value());
    }
}

TEST_CASE("Memory Management")
{
    SECTION("Clear operation")
    {
        LRUCache cache(3);

        cache.put("a", 1);
        cache.put("b", 2);
        cache.pin("a");

        cache.clear();

        REQUIRE(cache.size() == 0);
        REQUIRE(!cache.get("a").has_value());
        REQUIRE(!cache.get("b").has_value());

        // should be reusable after clear
        REQUIRE(cache.put("new", 100) == true);
        REQUIRE(cache.get("new").value() == 100);
    }
}

TEST_CASE("Complex Scenarios")
{
    SECTION("Mixed pinned and unpinned pops")
    {
        LRUCache cache(4);

        cache.put("a", 1);
        cache.put("b", 2);
        cache.put("c", 3);
        cache.put("d", 4);

        cache.pin("b");
        cache.pin("d");

        cache.get("a");
        cache.get("c");

        cache.put("e", 5);
        cache.put("f", 6);

        REQUIRE(!cache.get("a").has_value());  // popped
        REQUIRE(!cache.get("c").has_value());  // popped
        REQUIRE(cache.get("b").has_value());   // pinned
        REQUIRE(cache.get("d").has_value());   // pinned
        REQUIRE(cache.get("e").has_value());
        REQUIRE(cache.get("f").has_value());
    }

    SECTION("Random operation stress test")
    {
        LRUCache cache(50);
        std::vector<std::string> keys;

        for (int i = 0; i < 100; ++i)
        {
            keys.push_back("key" + std::to_string(i));
        }

        std::random_device rd;
        std::mt19937 gen(rd());

        for (int op = 0; op < 1000; ++op)
        {
            int action = gen() % 100;
            const std::string& key = keys[gen() % keys.size()];

            if (action < 40)
            {  // 40% put
                cache.put(key, op);
            }
            else if (action < 70)
            {  // 30% get
                cache.get(key);
            }
            else if (action < 85)
            {  // 15% pin
                cache.pin(key);
            }
            else if (action < 95)
            {  // 10% unpin
                cache.unpin(key);
            }
            else
            {  // 5% erase
                cache.erase(key);
            }

            REQUIRE(cache.size() <= cache.capacity());
        }

        // cache should still be functional
        REQUIRE(cache.put("final", 999) == true);
        REQUIRE(cache.get("final").value() == 999);
    }
}
