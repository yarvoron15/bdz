#include "stack.h"

Node::Node(int32_t value, Node* previous) : value(value), previous(previous) {
}

Stack::Stack() : head_(nullptr), size_(0) {
}

Stack::~Stack() {
    clear();
}

void Stack::push(int32_t value) {
    head_ = new Node(value, head_);
    ++size_;
}

void Stack::pop() {
    if (head_ == nullptr) {
        return;
    }

    Node* old_head = head_;
    head_ = head_->previous;
    delete old_head;
    --size_;
}

int32_t Stack::top() const {
    return head_->value;
}

int32_t Stack::size() const {
    return size_;
}

void Stack::clear() {
    while (head_ != nullptr) {
        pop();
    }
}

bool Stack::empty() const {
    return size_ == 0;
}
