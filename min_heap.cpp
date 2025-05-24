#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

struct Node
{
    char data;
    int freq = 0;
    Node *left = nullptr;
    Node *right = nullptr;

    Node(char d, int f) { data = d, freq = f; }
};

class MinHeap
{
private:
    vector<Node *> heap;

    // Helper functions
    int parentIndex(int i)
    {
        return (i - 1) / 2;
    }

    int leftChildIndex(int parent)
    {
        return (2 * parent) + 1;
    }

    int rightChildIndex(int parent)
    {
        return (2 * parent) + 2;
    }

    void swap(int a, int b)
    {
        Node *temp = heap[a];
        heap[a] = heap[b];
        heap[b] = temp;
    }

    void shiftUp(int index)
    {
        while (index > 0 && heap[parentIndex(index)]->freq >= heap[index]->freq)
        {
            swap(index, parentIndex(index));
            index = parentIndex(index);
        }
    }

    void shiftDown(int index)
    {
        int left = leftChildIndex(index);
        int right = rightChildIndex(index);
        int smallest = index;

        if (left < heap.size() && heap[left]->freq < heap[smallest]->freq)
            smallest = left;

        if (right < heap.size() && heap[right]->freq < heap[smallest]->freq)
            smallest = right;

        if (smallest != index)
        {
            swap(index, smallest);
            shiftDown(smallest);
        }
    }

public:
    MinHeap() {}

    bool isEmpty()
    {
        return (heap.size() == 0);
    }
    
    int size()
    {
        return heap.size();
    }

    void insert(Node *node)
    {
        heap.push_back(node);
        shiftUp(heap.size() - 1);
    }

    Node *remove()
    {
        if (isEmpty())
        {
            cout << "Heap is empty" << endl;
            return new Node('\0', -1);
        }

        // Store the root element (minimum element)
        Node *root = heap[0];

        // Replace root with the last element
        heap[0] = heap.back();

        // Remove the last element
        heap.pop_back();

        // Restore heap property if the heap is not empty
        if (!heap.empty())
            shiftDown(0);

        // Return the minimum element
        return root;
    }

    // Function to get the minimum element without removing it
    Node *top()
    {
        if (isEmpty())
        {
            cout << "Heap is empty" << endl;
            return new Node('\0', -1);
        }

        return heap[0];
    }
};
