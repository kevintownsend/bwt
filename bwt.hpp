/**
 * Copyright 2014 Kevin Townsend
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
namespace townsend {
    namespace algorithm {
        template <typename RandomAccessIterator>
        RandomAccessIterator bwtEncode(RandomAccessIterator first, RandomAccessIterator last);
        template <typename RandomAccessIterator>
        void bwtDecode(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator key);
    }
}

template <class RandomAccessIterator>
RandomAccessIterator townsend::algorithm::bwtEncode(RandomAccessIterator first, RandomAccessIterator last){
    using namespace std;
    class encodeHelper{
        struct Node{
            typename RandomAccessIterator::value_type value;
            Node* parent;
            Node* left;
            Node* right;
            int treeHeight;
        };
        RandomAccessIterator first;
        RandomAccessIterator last;
        uint64_t originalSize;
        Node* tree;
        public:
        RandomAccessIterator key;

        public:
        encodeHelper(RandomAccessIterator first, RandomAccessIterator last){
            this->first = first;
            this->last = last;
            originalSize = last-first;
            tree = new Node[originalSize+1];
            for(int64_t i = 0; i < originalSize; i++){
                tree[i].value = first[i];
            }
            Node* root = tree + originalSize;
            for(int i = originalSize - 1; i >= 0; i--){
                root=treeAdd(root, &tree[i]);
            }
            tmp = first;
            dfs(root);
            checkHeights(root);
        }
        ~encodeHelper(){
            delete [] tree;
        }
        private:
        Node* treeAdd(Node* root, Node* newNode){
            assert(root != NULL);
            assert(newNode != NULL);
            bool isLesser = compare(newNode, root);
            if(isLesser){
                if(root->left == NULL){
                    root->left = newNode;
                    newNode->parent = root;
                    newNode->treeHeight = 1;
                }else
                    treeAdd(root->left, newNode);
            }else{
                if(root->right == NULL){
                    root->right = newNode;
                    newNode->parent = root;
                    newNode->treeHeight = 1;
                }else
                    treeAdd(root->right, newNode);
            }
            setHeight(root);
            return avlBalance(root);
        }

        bool compare(Node* left, Node* right){
            size_t leftIndex = left - &tree[0];
            size_t rightIndex = right - &tree[0];
            if(leftIndex == originalSize)
                return false;
            if(rightIndex == originalSize)
                return true;
            RandomAccessIterator leftIterator = first + leftIndex;
            RandomAccessIterator rightIterator = first + rightIndex;
            if(*leftIterator < *rightIterator)
                return true;
            else if(*leftIterator > *rightIterator)
                return false;
            else{
                assert(*leftIterator == *rightIterator);
                return compareInTree(left + 1, right + 1);
            }
            return true;
        }
        bool compareInTree(Node* left, Node* right){
            vector<bool> leftPath;
            Node* current = left;
            while(current->parent != NULL){
                if(current == current->parent->left)
                    leftPath.push_back(0);
                else{
                    assert(current == current->parent->right);
                    leftPath.push_back(1);
                }
                current = current->parent;
            }
            vector<bool> rightPath;
            current = right;
            while(current->parent != NULL){
                if(current == current->parent->left)
                    rightPath.push_back(0);
                else{
                    assert(current == current->parent->right);
                    rightPath.push_back(1);
                }
                current = current->parent;
            }
            while(rightPath.size() != 0 && leftPath.size() != 0){
                bool leftStep = leftPath.back();
                leftPath.pop_back();
                bool rightStep = rightPath.back();
                rightPath.pop_back();
                if(leftStep < rightStep)
                    return true;
                else if(leftStep > rightStep)
                    return false;
                assert(leftStep == rightStep);
            }
            assert(!(rightPath.size() == 0 && leftPath.size() == 0));
            if(leftPath.size() == 0){
                assert(rightPath.size () != 0);
                return rightPath.back();
            }else{
                return !leftPath.back();
            }
        }
        RandomAccessIterator tmp;
        void dfs(Node* n){
            if(n == NULL)
                return;
            dfs(n->left);
            if(tree == n){
                key = tmp;
            }else{
                *tmp = (n-1)->value;
                tmp++;
            }

            dfs(n->right);
        }
        void checkHeights(Node* n){
            int leftHeight=0;
            int rightHeight=0;
            if(n->left != NULL){
                checkHeights(n->left);
                leftHeight=n->left->treeHeight;
            }
            if(n->right != NULL){
                checkHeights(n->right);
                rightHeight=n->right->treeHeight;
            }
            assert(n->treeHeight == max(leftHeight,rightHeight) + 1);
        }
        void setHeight(Node* n){
            int leftHeight=0;
            int rightHeight=0;
            if(n->left != NULL){
                leftHeight=n->left->treeHeight;
            }
            if(n->right != NULL){
                rightHeight=n->right->treeHeight;
            }
            n->treeHeight = max(leftHeight,rightHeight) + 1;
        }
        Node* avlBalance(Node* n){
            while(abs(childrenHeightDiff(n)) > 1){
                if(childrenHeightDiff(n) < 1){
                    if(childrenHeightDiff(n->left) > 0)
                        rotLeft(n->left);
                    n=rotRight(n);
                }else{
                    assert(childrenHeightDiff(n) > 1);
                    if(childrenHeightDiff(n->right) > 0)
                        rotLeft(n->right);
                    n=rotLeft(n);
                }
            }
            return n;
            
        }
        int childrenHeightDiff(Node* n){
            int leftHeight=0;
            int rightHeight=0;
            if(n->left != NULL){
                leftHeight=n->left->treeHeight;
            }
            if(n->right != NULL){
                rightHeight=n->right->treeHeight;
            }
            return rightHeight - leftHeight;
        }
        Node* rotLeft(Node* n){
            Node* p = n->parent;
            assert(n->right != NULL);
            Node* r = n->right;
            Node* f = r->left;
            r->left = n;
            n->parent = r;
            n->right = f;
            if(f != NULL)
                f->parent=n;
            r->parent=p;
            if(p!=NULL){
                if(p->left==n)
                    p->left=r;
                else{
                    assert(p->right==n);
                    p->right=r;}
            }
            setHeight(r->left);
            setHeight(r);
            return r;
        }
        Node* rotRight(Node* n){
            Node* p = n->parent;
            assert(n->left != NULL);
            Node* l = n->left;
            Node* f = l->right;
            l->right = n;
            n->parent = l;
            n->left = f;
            if(f != NULL)
                f->parent=n;
            l->parent=p;
            if(p!=NULL){
                if(p->right==n)
                    p->right=l;
                else{
                    assert(p->left==n);
                    p->left=l;}
            }
            setHeight(l->right);
            setHeight(l);
            return l;
        }
    };
    encodeHelper eh(first, last);
    return eh.key;
}
template<class RandomAccessIterator>
void townsend::algorithm::bwtDecode(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator key){
    using namespace std;
    vector<typename RandomAccessIterator::value_type> bwtEncoded(first, last);
    typename RandomAccessIterator::value_type empty;
    bwtEncoded.insert(bwtEncoded.begin() + (key-first), empty);
    RandomAccessIterator bwtEncodedKey = bwtEncoded.begin() + (key - first);
    map<typename RandomAccessIterator::value_type,uint64_t> sums;
    map<typename RandomAccessIterator::value_type,uint64_t> cummulativeSum;
    uint64_t keyIndex = key - first;
    uint64_t sum = 0;
    for(auto i = first; i != last; i++){
        sums[*i]++;
    }
    for(auto it = sums.begin();it != sums.end();it++){
        cummulativeSum[it->first] = sum;
        sum += it->second;
    }
    vector<uint64_t> fmIndex;
    for(auto it = bwtEncoded.begin(); it != bwtEncoded.end(); it++){
        if(it == bwtEncodedKey){
            fmIndex.push_back(last-first);
        }else{
            uint64_t index = cummulativeSum[*it];
            fmIndex.push_back(index);
            cummulativeSum[*it]++;
        }
    }
    auto tmp = last - 1;
    for(auto trav = bwtEncoded.end()-1; trav != bwtEncodedKey; trav = bwtEncoded.begin() + fmIndex[trav - bwtEncoded.begin()]){
        *tmp-- = *trav;
    }
    assert(tmp+1 == first);
    return;
}
