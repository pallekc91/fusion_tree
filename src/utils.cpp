#include "include/utils.h"


vector<int> important_bits(vector<int> keys,int number_of_values){
    
    set<int> ret;
    for(int i = 0;i< number_of_values;i++){
        for(int j = 0;j< number_of_values;j++){
            int xored = keys[i] ^ keys[j];
            int count = 0;
            while(xored != 0){
                int number_of_trailing_zeros = __builtin_ctz(xored);
                ret.insert(number_of_trailing_zeros+1+count);
                xored = xored >> (number_of_trailing_zeros+1);
                count = count + number_of_trailing_zeros+1;
            }
        }
    }

    vector<int> imp_bits;
    for(auto e : ret) imp_bits.push_back(e);
    return imp_bits;
}

int get_mask(vector<int> bits){
    int ret = 0;
    for(auto e : bits){
        ret |= (1 << (e-1));
    }
    return ret;
}

vector<int> get_mbits(vector<int> b_bits){
    int r = b_bits.size();
    vector<int> m_bits;
    int val = 0;
    bool flag = false;
    for(int t = 0;t < r; t++){
        while(!flag){
            for(int j = 0;j < t; j++){
                for(int k = 0;k < r;k++){
                    for(int l = 0;l<r;l++){
                        if(val == (m_bits[j] - b_bits[k] + b_bits[l])){
                            if(!flag){
                                val++;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
            flag = true;
        }
        int final_val = sizeof(int)*8 - b_bits[t] + t*pow(r, 3);
        final_val = (final_val/pow(r, 3)) * pow(r, 3);
        final_val += val;
        m_bits.push_back(final_val);
    }
    return m_bits;
}