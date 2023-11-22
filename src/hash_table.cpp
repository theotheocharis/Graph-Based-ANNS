#include "../include/hash_table.hpp"
#include "../include/image.hpp"

using namespace std;

const unsigned long m = 4294967291; // m = (2^32) - 5

inline uint HashTable::hash(uint ID) {
    return ID % this->size;
}


HashTable::HashTable(uint size) {
    this->size = size;
    table.reserve(size);

    for (uint i = 0; i < size; i++) {
        table[i] = new list<pair<uint, void *>>;
    }
}

HashTable::~HashTable() {
    for (uint i = 0; i < this->size; i++) {
        delete table[i];
    }
}

void HashTable::insert(uint ID, void *data) {
    uint bucket = this->hash(ID);
    pair<uint, void *> entry = pair<uint, void *>(ID, data);
    table[bucket]->push_back(entry);
}


list<pair<uint, void *>> HashTable::findBucket(uint ID) {
    uint bucket = this->hash(ID);
    return *(this->table[bucket]);
}

list<pair<uint, void *>> HashTable::findSameID(uint ID) {
    uint bucket = this->hash(ID);
    list<pair<uint, void *>> sameID;

    for (auto &it : *(this->table[bucket])) {
        if (it.first == ID) {
            sameID.push_back(it);
        }
    }

    return sameID;
}