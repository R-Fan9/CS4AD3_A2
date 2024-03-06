#ifndef BUFFER_H
#define BUFFER_H

#include <queue>

using namespace std;

template <typename Rec>
class Buffer
{
    priority_queue<Rec, vector<Rec>, greater<Rec>> min_heap;
    priority_queue<Rec, vector<Rec>, less<Rec>> max_heap;
    size_t max_size;
    int sorting_order;

public:
    Buffer(size_t size, int order) : max_size(size), sorting_order(order) {}

    bool push(const Rec &val)
    {
        if (sorting_order == 1)
        {
            if (min_heap.size() < max_size)
            {
                min_heap.push(val);
                return true;
            }
        }
        else
        {
            if (max_heap.size() < max_size)
            {
                max_heap.push(val);
                return true;
            }
        }
        return false;
    }

    Rec top() const
    {
        if (sorting_order == 1)
        {
            return min_heap.top();
        }
        else
        {
            return max_heap.top();
        }
    }

    void pop()
    {
        if (sorting_order == 1)
        {
            min_heap.pop();
        }
        else
        {
            max_heap.pop();
        }
    }

    bool empty() const
    {
        if (sorting_order == 1)
        {
            return min_heap.empty();
        }
        else
        {
            return max_heap.empty();
        }
    }
};

#endif