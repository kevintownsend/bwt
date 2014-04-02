#include <vector>
#include <map>
#include <iostream>
#include <stdint.h>
#include <cassert>
namespace townsend {
    namespace algorithm {
        template <typename RandomAccessIterator>
        RandomAccessIterator bwtEncode(RandomAccessIterator first, RandomAccessIterator last);
        template <typename RandomAccessIterator>
        void bwtDecode(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator key);
    }
}
//implementation
//template <class RandomAccessIterator>


template <class RandomAccessIterator>
RandomAccessIterator townsend::algorithm::bwtEncode(RandomAccessIterator first, RandomAccessIterator last){
    using namespace std;
    class encodeHelper{
        struct Node{
            //typename RandomAccessIterator::value_type value;
            typename RandomAccessIterator::value_type value;
            Node* parent;
            Node* left;
            Node* right;
        };
        vector<Node> tree;
        RandomAccessIterator first;
        RandomAccessIterator last;
        uint64_t originalSize;
        public:
        RandomAccessIterator key;

        //void treeAdd(Node* root, Node* newNode);
        public:
        encodeHelper(RandomAccessIterator first, RandomAccessIterator last){
            this->first = first;
            this->last = last;
            originalSize = last-first;
            tree.resize(originalSize+1);
            for(int64_t i = 0; i < originalSize; i++){
                tree[i].value = first[i];
            }
            Node* root = &tree.back();
            for(int i = originalSize - 1; i >= 0; i--){
                treeAdd(root, &tree[i]);
            }
            tmp = first;
            dfs(root);
        }
        private:
        void treeAdd(Node* root, Node* newNode){
            assert(root != NULL);
            assert(newNode != NULL);
            bool isLesser = compare(newNode, root);
            if(isLesser){
                if(root->left == NULL){
                    root->left = newNode;
                    newNode->parent = root;
                }else
                    treeAdd(root->left, newNode);
            }else{
                if(root->right == NULL){
                    root->right = newNode;
                    newNode->parent = root;
                }else
                    treeAdd(root->right, newNode);
            }
        }
        bool compare(Node* left, Node* right){
            uint64_t leftIndex = left - &tree[0];
            uint64_t rightIndex = right - &tree[0];
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
            if(&tree.front() == n){
                key = tmp;
            }else{
                *tmp = (n-1)->value;
                tmp++;
            }

            dfs(n->right);
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
    for(auto& p : sums){
        cummulativeSum[p.first] = sum;
        sum += p.second;
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
