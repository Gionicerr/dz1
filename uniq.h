#pragma once

template <typename T>
class UniquePtr{
private:
    T* ptr;
public:
    UniquePtr(T* pointer = nullptr) : ptr(pointer){}
    ~UniquePtr(){
        delete ptr;
    }
    
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) : ptr(other.ptr){
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        if(this!=&other){
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const{return *ptr;}
    T* get() const{return ptr;}
    T* release(){
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* pointer = nullptr){
        if(ptr != pointer){
            delete ptr;
            ptr = pointer;
        }
    }
};

template <typename T>
class UniquePtr<T[]>{
private:
    T* ptr;
public:
    UniquePtr(T* pointer = nullptr) : ptr(pointer){}
    ~UniquePtr(){
        delete[] ptr;
    }
    
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) : ptr(other.ptr){
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        if(this!=&other){
            delete[] ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator[](int index) const{return ptr[index];}
    T* get() const{return ptr;}
    T* release(){
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* pointer = nullptr){
        if(ptr != pointer){
            delete[] ptr;
            ptr = pointer;
        }
    }
};