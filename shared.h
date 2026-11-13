#pragma once

template <typename T>
class SharedPtr{
private:
    T* ptr;
    int* ref_count;
public:
    SharedPtr(T* pointer = nullptr) : ptr(pointer), ref_count(new int(1)){}
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count){
        ++(*ref_count);
    }
    ~SharedPtr(){
        if(ref_count != nullptr && --(*ref_count) == 0){
            delete ptr;
            delete ref_count;
        }
    }
    SharedPtr& operator=(const SharedPtr& other){
        if(this != &other){
            if(--(*ref_count)==0){
                delete ptr;
                delete ref_count;
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            ++(*ref_count);
        }
        return *this;
    }
    SharedPtr(SharedPtr&& other) : ptr(other.ptr), ref_count(other.ref_count){
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if(this != &other){
            if(ref_count != nullptr && --(*ref_count)==0){
                delete ptr;
                delete ref_count;
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }
    T& operator*() const{return *ptr;}
    T* operator->() const{return ptr;}
    T* get() const{return ptr;}

    void reset(T* pointer = nullptr){
        if(ptr == pointer){
            return;
        }
        if(ref_count != nullptr && --(*ref_count) == 0){
            delete ptr;
            delete ref_count;
        }
        ptr = pointer;
        ref_count = new int(1);
    } 
};

template <typename T>
class SharedPtr<T[]>{
private:
    T* ptr;
    int* ref_count;
public:
    SharedPtr(T* pointer = nullptr) : ptr(pointer), ref_count(new int(1)){}
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count){
        ++(*ref_count);
    }
    ~SharedPtr(){
        if(ref_count != nullptr && --(*ref_count) == 0){
            delete[] ptr;
            delete ref_count;
        }
    }
    SharedPtr& operator=(const SharedPtr& other){
        if(this != &other){
            if(--(*ref_count)==0){
                delete[] ptr;
                delete ref_count;
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            ++(*ref_count);
        }
        return *this;
    }

    SharedPtr(SharedPtr&& other) : ptr(other.ptr), ref_count(other.ref_count){
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if(this != &other){
            if(ref_count != nullptr && --(*ref_count)==0){
                delete[] ptr;
                delete ref_count;
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }
    T& operator[](int index) const{return ptr[index];}
    T* get() const{return ptr;}

    void reset(T* pointer = nullptr){
        if(ptr == pointer){
            return;
        }
        if(ref_count != nullptr && --(*ref_count) == 0){
            delete[] ptr;
            delete ref_count;
        }
        ptr = pointer;
        ref_count = new int(1);
    }
};