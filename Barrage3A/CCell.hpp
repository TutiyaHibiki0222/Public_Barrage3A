/*
    CCell.hpp

    作成日         : 2025/05/09
    最終変更日     : 2025/05/09
*/
#pragma once
#include <memory>
#include "TreeData.hpp"

template<class T>
class CCell
{
protected:
    TreeData<T>* tree;
public:
    CCell() : tree(0)
    {}

    virtual ~CCell()
    {
        if (tree)
            ResetLink(tree);
    }

    void ResetLink(TreeData<T>* spOFT)
    {
        if (spOFT->pNext != 0) {
            ResetLink(spOFT->pNext);
        }
        spOFT = 0;
    }

    bool Push(TreeData<T>* spOFT)
    {
        if (spOFT == 0) return false;	// 無効オブジェクトは登録しない
        if (spOFT->m_pCell == this) return false;	// 2重登録チェック
        if (tree == 0) {
            tree = spOFT;	// 空間に新規登録
        }
        else
        {
            // 最新OFTオブジェクトを更新
            spOFT->m_spNext = tree;
            tree->m_spPre = spOFT;
            tree = spOFT;
        }
        spOFT->pCell = this;	// 空間を登録
        return true;
    }

    TreeData<T>* GetFirstObj() {
        return tree;
    }

    bool OnRemove(TreeData<T>* pRemoveObj)
    {
        if (tree == pRemoveObj) {
            // 次のオブジェクトに挿げ替え
            if (tree != NULL)
                tree = tree->pNext;
        }
        return true;
    }
};