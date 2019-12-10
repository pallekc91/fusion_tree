#include "include/node.h"
#include "include/utils.h"
#include<iostream>

node::node(int mx_keys)
{   
    max_keys = mx_keys;
    for(int i = 0;i<max_keys+1 ; i++){
        children.insert(children.begin()+i, NULL);
    }
    isleaf = true;
    number_of_values = 0;
    parent = NULL;
}

void node::process(){
    
    if(number_of_values == 0){
        return;
    }
    b_bits = important_bits(values,number_of_values);
    m_bits = get_mbits(b_bits);
    bm_bits.insert(bm_bits.begin(),b_bits.begin(),b_bits.end());
    bm_bits.insert(bm_bits.end(),m_bits.begin(),m_bits.end());
    if(b_bits.size() == 0){
        sketch_len = 1;
    } else {
        sketch_len = b_bits.back() + m_bits.back() - b_bits.front() + m_bits.front();
        sketch_len = sketch_len ? sketch_len : 1;
    }
    for(int j = 0; j < number_of_values; ++j){
        int sketch_ = get_approx_sketch(values[number_of_values-j-1]);
        approx_sketch |= sketch_ | ( (int(1)<<sketch_len) << j*(sketch_len+1));
        sketch_mask_msb |= (int(1)<<sketch_len) << j*(sketch_len+1);
        to_repeat |= 1 << j * (sketch_len);
    }
    for(int i =0;i < number_of_values+1;i++){
        if(children[i] != NULL)
            children[i]->process();
    }
}

int node::get_approx_sketch(int x){
    int x_prime = x & get_mask(b_bits);
    int ret = x_prime * get_mask(m_bits);
    ret = ret & get_mask(bm_bits);
    return ret;
}

int node::parallel_compare(int x){
    int sketch_x = get_approx_sketch(x);
    int sketch_lengthened = sketch_x * to_repeat;//01101 -> 01101 01101 01101 ...
    int result =  approx_sketch - sketch_lengthened;
    result &= sketch_mask_msb;
    int msb = (sizeof(int)*8) - __builtin_clz(result)-1;
    if(msb < 0){
        return number_of_values;
    } else {
        int indx = msb / (sketch_len+1);
        return number_of_values - indx - 1;
    }
}