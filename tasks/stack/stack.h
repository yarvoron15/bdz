#ifndef STACK_H_
#define STACK_H_


#include <cstdint>

struct Node {
    int32_t value;
    Node* previous;
    Node(int32_t value, Node* previous);
};

class Stack {
private:
    Node* head_;
    int size_;

public:
    Stack();

    ~Stack();

    void push(int32_t value);

    void pop();

    int32_t top() const;

    int32_t size() const;

    void clear();

    bool empty() const;
};


#endif // STACK_H_
