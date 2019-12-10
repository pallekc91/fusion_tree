#ifndef node_h
#define node_h

#include <vector>
#include <math.h>
using namespace std;
#define u 100000
#define W log2(u)

class node{
    public:

    int max_keys;
    vector<int> values; //should be sorted
    vector<node*> children;
    node * parent;
    bool isleaf;
    int number_of_values;
    vector<int> b_bits;
    vector<int> m_bits;
    vector<int> bm_bits;
    int approx_sketch;
    int sketch_mask_msb;
    int sketch_len;
    int to_repeat;//ones at lsb in an int spaced at the length of sketch

    node(int max_keys);
    void process();
    int get_approx_sketch(int x);
    int parallel_compare(int x);

};

#endif //node_h