#include "node.h"

#ifndef fusion_h
#define fusion_h

class fusion{
    public:
    int w;
    int max_keys;
    node * root;
    bool isleaf;

    fusion();
    void initialize();
    void insert(int x);
    int successor(int x);

    private:
    void insert(node * root,int x,int pos);
    void split_children(node * n,int pos);
    void leaf_insert(node * n,int x);
    int successor(node * n,int x);
};

#endif // fusion_h