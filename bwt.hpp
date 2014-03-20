#include <vector>
#include <iostream>
#include <stdint.h>
#include <cassert>

template <class RandomAccessIterator>
RandomAccessIterator bwtEncode(RandomAccessIterator first, RandomAccessIterator last);
void bwtDecode();

//implementation
//template <class RandomAccessIterator>


template <class RandomAccessIterator>
RandomAccessIterator bwtEncode(RandomAccessIterator first, RandomAccessIterator last){
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
            cerr << "here" << endl;
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
            cerr << "compare in tree" << endl;
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
            //cerr << first[n - &tree[0]] << endl;
            cerr << "in order" << endl;
            if(&tree.front() == n)
                cerr << "found key" << endl;
            else{
                cerr << "error: " << endl;
                cerr << "n: " << n->value << endl;
                cerr << "n - 1: " << (n-1)->value << endl;
                *tmp = (n-1)->value;
                tmp++;
            }

            dfs(n->right);
        }
    };
    encodeHelper eh(first, last);
    return first;
}
void bwtDecode(){return;};

