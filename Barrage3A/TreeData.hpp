/*
    TreeData.hpp

    ì¬“ú         : 2025/05/09
    ÅI•ÏX“ú     : 2025/05/09
*/
#pragma once
#include <memory>
#include "CCell.hpp"


template<class T>
class CCell;

template<class T>
class TreeData
{
public:
    CCell<T>* pCell;
    T* obj;
    TreeData<T>* pPre;
    TreeData<T>* pNext;
    int id;
public:
    TreeData(int id = 0) {
        id = id;
        pCell   = NULL;
        obj     = NULL;
        pPre    = NULL;
        pNext   = NULL;
    }
    virtual ~TreeData()
    {
    }
public:
    bool Remove() {
        if (!pCell)
            return false;

        if (pCell->OnRemove(this))
            return false;

        if (pPre != 0)
        {
            pPre->pNext = pNext;
        }
        if (pNext != 0)
        {
            pNext->pPre = pPre;
        }
        pPre = 0;
        pNext = 0;
        pCell = NULL;
        return true;
    }
};