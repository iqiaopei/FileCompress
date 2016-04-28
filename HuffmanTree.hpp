#include<assert.h>
#include"Heap.h"

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode<T>* _left;
	HuffmanTreeNode<T>* _right;
	HuffmanTreeNode<T>* _parent;

	T _weight; //权值

	HuffmanTreeNode(const T& x)
		:_weight(x)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;

	template<class T>
	struct NodeCompare  //结点权值比较，因为权值类型的问题，定义仿函数
	{
		bool operator()(Node* l, Node* r)
		{
			return l->_weight < r->_weight;
		}
	};
public:
	HuffmanTree()
		:_root(NULL)
	{}

	Node* GetRootNode()
	{
		return _root;
	}

	void CreateTree(const T* a, size_t size, const T& invalid)
	{
		assert(a);

		Heap<Node*, NodeCompare<T>> minHeap;
		//1.让所有数据都变为结点
		//先把数据结点存入堆中，每次取两个小的出来，再把和放入堆中，循环
		//直到堆里只剩一个结点，说明树已建好，剩下的那个结点即为哈夫曼树的根节点
		for (size_t i = 0; i < size; ++i)
		{
			if (a[i] != invalid)
			{
				Node* node = new Node(a[i]);
				minHeap.Push(node);
			}
		}

		while (minHeap.Size() > 1)
		{
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();

			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;

			minHeap.Push(parent);
		}
		_root = minHeap.Top();
	}


private:
	Node* _root;
};
void TestHuffmanTree()
{
	int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	HuffmanTree<int> t;
	t.CreateTree(a, sizeof(a) / sizeof(int), -1);
}