#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <iostream>
#include <vector>
#include <list>
#include <utility>

class HashTable {
private:

    uint size;

    std::vector<std::list<std::pair<uint, void *>>*> table;

    uint hash(uint);

public:

    HashTable(uint);

    ~HashTable();

    void insert(uint, void *);

    std::list<std::pair<uint, void *>> findBucket(uint);
    std::list<std::pair<uint, void *>> findSameID(uint);
};

#endif