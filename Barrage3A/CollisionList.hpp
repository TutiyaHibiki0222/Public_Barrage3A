#pragma once
#include <memory>
#define COLLISIONLIST_REALLOCSIZE	100
template < class T >
class CollisionList {
public:
    CollisionList() : root_(0), pos_(0), mallocSize_(0) {
        root_ = (T**)malloc(0);
    }
    ~CollisionList() {
        reflesh();
    }

    size_t getSize() {
        return pos_;
    }

    T** getRootPtr() {
        return root_;
    }

    void resetPos() {
        pos_ = 0;
    }

    void wright(T* obj1, T* obj2) {
        if (pos_ >= mallocSize_) {
            root_ = (T**)realloc(root_, sizeof(T*) * (mallocSize_ + COLLISIONLIST_REALLOCSIZE));
            mallocSize_ += COLLISIONLIST_REALLOCSIZE;
        }
        root_[pos_++] = obj1;
        root_[pos_++] = obj2;
    }

    void reflesh() {
        if (root_) {
            free(root_);
        }
    }

private:
    T** root_;		// リストルートポインタ
    size_t	pos_;
    size_t	mallocSize_;
};