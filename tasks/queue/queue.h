#ifndef QUEUE_H_
#define QUEUE_H_


#include <cstdint>

struct Node {
    int32_t value;
    Node* next;
    Node(int32_t value, Node* next);
};

class Queue {
private:
    Node* front_;
    Node* tail_;
    int32_t size_;

public:
    Queue();

    ~Queue();

    void push(int32_t value);

    void pop();

    int32_t front() const;

    int32_t size() const;

    void clear();

    bool empty() const;
};


#endif // QUEUE_H_
