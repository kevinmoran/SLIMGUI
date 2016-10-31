#pragma once
#include <stdlib.h>
#include <assert.h>

class DynArray{
    int _capacity;
public: 
    int size;
    float* data;

    DynArray(){
        init();
    }
    ~DynArray(){
        destroy();
    }
    void push(float x){
        if(size==_capacity){
            if(_capacity==0) init();
            else reserve(_capacity*2);
        }
        size++;
        data[size-1] = x;
    }
    //Note: my idea for reserve and resize was to compute an intelligent
    //new size when growing the array. However, no really useful approach
    //was apparent so now just do it the simple way and rely on user code
    //to choose nice values for s (e.g. push() always doubles the array)
    void reserve(size_t s){
        if(s>_capacity){
            _capacity = s;
            data = (float*)realloc(data, _capacity*sizeof(float));
        }
    }
    void resize(size_t s){
        size = s;
        _capacity = s;
        if(s!=0) data = (float*)realloc(data, _capacity*sizeof(float));
        else free(data); //if(s==0) realloc is undefined (may return NULL ptr)
    }
    void clear(){
        memset(data, 0, _capacity*sizeof(float));
        size = 0;
    }
    void init(){
        size = 0;
        _capacity = 8;
        data = (float*)malloc(_capacity*sizeof(float));
    }
    void destroy(){
        size = 0;
        _capacity = 0;
        free(data);
        data = NULL;
    }
    void print(){
        if(size==0) return;
        printf("(\n");
        for(int i=0; i<size; i++){
            printf("%f ", data[i]);
        }
        printf("\n)\n");
    }
    float& operator[](int rhs){
        assert(rhs<size);
        return data[rhs];
    }
    int capacity(){
        return _capacity;
    }
};
