
#include "queue.h"

Node::Node(int32_t value, Node* next) : value(value), next(next) {
}

Queue::Queue() : front_(nullptr), tail_(nullptr), size_(0) {
}

Queue::~Queue() {
    clear();
}

void Queue::push(int32_t value) {
    Node* node = new Node(value, nullptr);
    if (tail_ == nullptr) {
        front_ = node;
        tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
    }
    ++size_;
}

void Queue::pop() {
    if (front_ == nullptr) {
        return;
    }

    Node* old_front = front_;
    front_ = front_->next;
    delete old_front;
    --size_;

    if (front_ == nullptr) {
        tail_ = nullptr;
    }
}

int32_t Queue::front() const {
    return front_->value;
}

int32_t Queue::size() const {
    return size_;
}

void Queue::clear() {
    while (front_ != nullptr) {
        pop();
    }
}

bool Queue::empty() const {
    return size_ == 0;
}
