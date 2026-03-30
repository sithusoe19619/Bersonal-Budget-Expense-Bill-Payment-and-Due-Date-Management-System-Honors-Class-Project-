#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include "CategoryInfo.h"

class HashMap {
private:
    struct Entry {
        std::string key;
        CategoryInfo value;
        bool occupied;
        bool isDeleted;
        Entry() : occupied(false), isDeleted(false) {}
    };

    Entry* buckets;
    int capacity;
    int size;

    int hash(const std::string& key) const;
    void resize();

public:
    HashMap(int initialCapacity = 16);
    ~HashMap();
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

    void insert(const std::string& key, const CategoryInfo& value);
    CategoryInfo* get(const std::string& key); // WARNING: pointer invalidated if insert() triggers resize()
    void remove(const std::string& key);
    bool contains(const std::string& key) const;
};

#endif // HASHMAP_H
