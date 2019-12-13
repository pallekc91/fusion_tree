#include "include/node.h"
#include "include/utils.h"
#include<iostream>

node::node(int mx_keys)
{   
    this->max_keys = mx_keys;
    for(int i = 0;i<this->max_keys+1 ; i++){
        this->children.insert(this->children.begin()+i, NULL);
    }
    this->isleaf = true;
    this->number_of_values = 0;
    this->parent = NULL;
}

void node::process(){
    
    if(this->number_of_values == 0){
        return;
    }
    if(this->number_of_values > 1){
        cout << endl;
        this->b_bits = important_bits(this->values,this->number_of_values);
        this->m_bits = get_mbits(this->b_bits);
        bm_bits.insert(bm_bits.begin(),b_bits.begin(),b_bits.end());
        bm_bits.insert(bm_bits.end(),m_bits.begin(),m_bits.end());
        if(b_bits.size() == 0){
            this->sketch_len = 1;
        } else {
            this->sketch_len = b_bits.back() + m_bits.back() - b_bits.front() + m_bits.front();
            this->sketch_len = this->sketch_len ? this->sketch_len : 1;
        }
        for(int j = 0; j < number_of_values; ++j){
            int sketch_ = get_approx_sketch(this->values[this->number_of_values-j-1]);
            this->approx_sketch |= sketch_ | ( (int(1)<<this->sketch_len) << j*(this->sketch_len+1));
            this->sketch_mask_msb |= (int(1)<<this->sketch_len) << j*(this->sketch_len+1);
            this->to_repeat |= 1 << j * (this->sketch_len);
        }
    }
    for(int i =0;i < this->number_of_values+1;i++){
        if(this->children[i] != NULL)
            this->children[i]->process();
    }
}

int node::get_approx_sketch(int x){
    int x_prime = x & get_mask(this->b_bits);
    int ret = x_prime * get_mask(this->m_bits);
    ret = ret & get_mask(this->bm_bits);
    return ret;
}

int node::parallel_compare(int x){
    if(this->number_of_values == 1){
        return x > this->values[0];
    }
    int sketch_x = get_approx_sketch(x);
    int sketch_lengthened = sketch_x * this->to_repeat;//01101 -> 01101 01101 01101 ...
    int result =  this->approx_sketch - sketch_lengthened;
    result &= this->sketch_mask_msb;
    int msb = (sizeof(int)*8) - __builtin_clz(result)-1;
    if(msb < 0){
        return this->number_of_values;
    } else {
        int indx = msb / (this->sketch_len+1);
        return this->number_of_values - indx - 1;
    }
}