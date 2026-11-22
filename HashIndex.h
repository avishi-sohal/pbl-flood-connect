#ifndef HASHINDEX_H
#define HASHINDEX_H

#include <vector>

struct ReportNode;   // defined in EmergencyReport.h

// Hash table with chaining: report ID -> pointer to its node in the linked list.
// Average lookup time is O(1) instead of walking the whole list.
class HashIndex {
private:
    struct Entry { int id; ReportNode* node; };
    std::vector< std::vector<Entry> > buckets;

    int bucketOf(int id) const { return id % (int)buckets.size(); }   // hash function

public:
    HashIndex() : buckets(31) {}

    void insert(int id, ReportNode* node) {
        Entry e; e.id = id; e.node = node;
        buckets[bucketOf(id)].push_back(e);
    }
    ReportNode* find(int id) const {
        const std::vector<Entry>& b = buckets[bucketOf(id)];
        for (size_t i = 0; i < b.size(); i++)
            if (b[i].id == id) return b[i].node;
        return NULL;
    }
    void erase(int id) {
        std::vector<Entry>& b = buckets[bucketOf(id)];
        for (size_t i = 0; i < b.size(); i++)
            if (b[i].id == id) { b.erase(b.begin() + i); return; }
    }
    void clear() {
        for (size_t i = 0; i < buckets.size(); i++) buckets[i].clear();
    }
};

#endif
