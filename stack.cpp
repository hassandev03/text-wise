#include <iostream>
using namespace ::std;

class Stack
{
private:
    struct Node
    {
        string data;
        Node *link = nullptr;
    };
    Node *top = nullptr;

    void displayUnderFlowMessage()
    {
        cout << "Stack Underflow!" << endl;
    }
    int size = 0;

public:
    Stack()
    {
    }

    Stack(string data)
    {
        push(data);
    }

    bool isEmpty()
    {
        return (top == nullptr);
    }

    int getSize()
    {
        return size;
    }

    void push(string data)
    {
        Node *newNode = new Node;
        newNode->data = data;
        if (isEmpty())
            top = newNode;
        else
        {
            newNode->link = top;
            top = newNode;
        }
        size++;
    }

    string pop()
    {
        if (isEmpty())
        {
            displayUnderFlowMessage();
            return "";
        }
        else
        {
            Node *temp = top;
            string poppedValue = temp->data;
            top = top->link;

            delete temp;
            temp = nullptr;

            size--;
            return poppedValue;
        }
    }

    string peek()
    {
        if (isEmpty())
        {
            displayUnderFlowMessage();
            return "";
        }
        else
            return top->data;
    }

    void clear() {
        while (!isEmpty())
            pop();
    }

};
