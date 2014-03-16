#include <vector>
#include <iostream>
#include <stdint.h>
#include <cassert>

template <class RandomAccessIterator>
RandomAccessIterator bwtEncode(RandomAccessIterator first, RandomAccessIterator last);
void bwtDecode();

//implementation
//template <class RandomAccessIterator>
struct Node{
    Node* parent;
    Node* left;
    Node* right;
};


template <class RandomAccessIterator>
RandomAccessIterator bwtEncode(RandomAccessIterator first, RandomAccessIterator last){
    using namespace std;
    class encodeHelper{
        vector<Node> tree;
        RandomAccessIterator first;
        RandomAccessIterator last;
        uint64_t originalSize;

        //void treeAdd(Node* root, Node* newNode);
        public:
        encodeHelper(RandomAccessIterator first, RandomAccessIterator last){
            originalSize = last-first;
            tree.resize(originalSize+1);
            Node* root = &tree.back();
            for(int i = originalSize - 1; i >= 0; i--){
                treeAdd(root, &tree[i]);
            }
        }
        private:
        void treeAdd(Node* root, Node* newNode){
            bool isLesser = compare(newNode, root);
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
            }
            return true;
        }
    };
    encodeHelper eh(first, last);
    return first;
}
void bwtDecode(){return;};

