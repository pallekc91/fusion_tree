
#include "include/fusion.h"
#include <math.h>
#include<iostream>
using namespace std;

fusion::fusion(){
    this->w = sizeof(int)*8;
    this->max_keys = pow(this->w,0.2);
    if(this->max_keys < 2){
        cout << "word size was : " << this->w << endl;
        cout << "Minimum supported word size : 32" << endl;
        abort();
    }
    this->root = new node(max_keys);
}

void fusion::initialize(){
    this->root->process();
}

//bottom up
void fusion::insert(int x){
    insert(this->root,x,0);
}

//bottom up
void fusion::insert(node * n,int x,int pos){
    if(n->isleaf){
        leaf_insert(n,x);
        if(n->number_of_values > this->max_keys){
            if(n->parent == NULL){
                node * a = new node(this->max_keys);
                a->parent = NULL;
                a->number_of_values = 1;
                a->children.insert(a->children.begin(),n);
                n->parent = a;
                a->isleaf = false;
                
                int break_at = n->number_of_values/2;
                a->values.insert(a->values.begin(),n->values[break_at]);

                node * b = new node(this->max_keys);
                a->children.insert(a->children.begin()+1,b);
                b->parent = a;
                for(int i=0;i<this->max_keys-break_at;i++){
                    b->values.insert(b->values.begin()+i,n->values[n->number_of_values-break_at+i]);
                }
                b->number_of_values = this->max_keys-break_at;
                n->number_of_values = n->number_of_values - b->number_of_values - 1;
                b->isleaf = true;
                this->root = a;
            } else {
                split_children(n->parent,pos);
            }
        }
    } else {
        int i=0;
        for(;i<n->number_of_values;i++){
            if(x == n->values[i]){
                return;
            }
            if(x < n->values[i]){
                break;
            }
        }
        insert(n->children[i],x,i);
        if(n->number_of_values > max_keys){
            if(n->parent == NULL){
                node * a = new node(max_keys);
                a->parent = NULL;
                a->isleaf = false;
                a->number_of_values = 1;

                a->children.insert(a->children.begin(),n);
                n->parent = a;

                int break_at = n->number_of_values/2;
                a->values.insert(a->values.begin(),n->values[break_at]);

                node * b = new node(max_keys);
                a->children.insert(a->children.begin()+1,b);
                b->parent = a;
                b->isleaf = n->isleaf;
                for(int j=0;j<this->max_keys-break_at;j++){
                    b->values.insert(b->values.begin()+j,n->values[n->number_of_values-break_at+j]);
                }
                b->number_of_values = max_keys-break_at;
                int n_prev_values = n->number_of_values;
                n->number_of_values = n->number_of_values - b->number_of_values - 1;
                this->root = a;
                for(int j=n->number_of_values+1,k=0;j<n_prev_values+1;j++,k++){
                    b->children.insert(b->children.begin()+k,n->children[j]);
                    b->children[k]->parent = b;
                }
            } else {
                split_children(n->parent,pos);
            }
        }
    }
}


//bottom up
void fusion::split_children(node * n,int pos){
    node * a = new node(this->max_keys);
    node * b = n->children[pos];
    int break_at = b->number_of_values / 2 ;
    for(int i=0;i<this->max_keys-break_at;i++){
        a->values.insert(a->values.begin()+i,b->values[b->number_of_values-break_at+i]);
    }
    if(!b->isleaf){
        for(int i=break_at+1,j=0;i<b->number_of_values+1;i++,j++){
            a->children.insert(a->children.begin()+j,b->children[i]);
            a->children[j]->parent = a;
        }
    }
    a->isleaf = b->isleaf;
    a->number_of_values = this->max_keys-break_at;
    b->number_of_values = b->number_of_values - a->number_of_values - 1;
    int prev_values_n = n->number_of_values;
    leaf_insert(n,b->values[break_at]);
    for(int i = prev_values_n;i==pos;i--){
        n->children.insert(n->children.begin()+i+1,n->children[i]);
    }
    n->children.insert(n->children.begin()+pos+1,a);
    a->parent = n;
}

int fusion::successor(int x){
    return successor(this->root,x);
}

void fusion::leaf_insert(node * n,int x){
    int insert_at = 0;
    for(;insert_at< n->number_of_values ;insert_at++){
        if(x == n->values[insert_at]){
            return;
        }
        if(x < n->values[insert_at]){
            break;
        }
    }
    for(int i = n->number_of_values-1;i>insert_at;i--){
        n->values.insert(n->values.begin()+i+1,n->values[i]);
    }
    n->values.insert(n->values.begin()+insert_at,x);
    n->number_of_values = n->number_of_values +1;
}

int fusion::successor(node *n,int x){
    int i = n->parallel_compare(x);
    int msb = 0;
    if(i < n->number_of_values){
        int pos_match = x ^ n->values[i];
        msb = __builtin_clz(pos_match);
    }
    if(i != 0){
        int pos_match_prev = x ^ n->values[i-1];
        msb = max(msb,__builtin_clz(pos_match_prev));
    }
    //making the largest value before x
    msb = (sizeof(int)*8) - msb - 1;
    int temp_mask = (int(1) << msb) - 1;
    int temp_mask_ = (int(1) << msb) ^ int(-1);
    int result = x | temp_mask;

    if((int(1) <<  msb) & x){
        result &= temp_mask_;
    } else {
        result &= int(-1) ^ temp_mask; //((E(1) << (y))-1);
        result |= (int(1)<<msb);
    }
    int j = n->parallel_compare(result);
    //miniature testcase and correction if fails
    if(n->number_of_values != j && x > n->values[j]) {
        j++;
    }
    if(n->isleaf){
        if(j >= n->number_of_values){
            return -1;
        }
        return n->values[j];
    }
    int successor_from_child = successor(n->children[j],x);
    if(successor_from_child == -1){
        if(j >= n->number_of_values){
            return successor_from_child;
        } else {
            return n->values[j];
        }
    } else {
        return successor_from_child;
    }
}