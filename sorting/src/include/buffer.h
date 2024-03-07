#ifndef BUFFER_H
#define BUFFER_H

#include <queue>

using namespace std;

template <typename Rec>
class Buffer
{
    priority_queue<Rec, vector<Rec>, greater<Rec>> m_min_heap;
    priority_queue<Rec, vector<Rec>, less<Rec>> m_max_heap;
    size_t m_max_size;
    int m_sorting_order;

public:
    Buffer(size_t size, int order) : m_max_size(size), m_sorting_order(order) {}

    /**
     * @brief Pushes a record onto the buffer
     *
     * If the sorting order is ascending (1), min-heap is used. Otherwise max-heap is used
     *
     * @param val The record to be pushed.
     * @return True if the record was successfully pushed, false if the buffer is full.
     */
    bool push(const Rec &val)
    {
        if (m_sorting_order == 1)
        {
            if (m_min_heap.size() < m_max_size)
            {
                m_min_heap.push(val);
                return true;
            }
        }
        else
        {
            if (m_max_heap.size() < m_max_size)
            {
                m_max_heap.push(val);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Returns the top element of the buffer.
     *
     * @return The top element based on the sorting order.
     */
    Rec top() const
    {
        if (m_sorting_order == 1)
        {
            return m_min_heap.top();
        }
        else
        {
            return m_max_heap.top();
        }
    }

    /**
     * @brief Removes the top element from the buffer.
     */
    void pop()
    {
        if (m_sorting_order == 1)
        {
            m_min_heap.pop();
        }
        else
        {
            m_max_heap.pop();
        }
    }

    /**
     * @brief Checks if the buffer is empty.
     *
     * @return True if the buffer is empty, otherwise false.
     */
    bool empty() const
    {
        if (m_sorting_order == 1)
        {
            return m_min_heap.empty();
        }
        else
        {
            return m_max_heap.empty();
        }
    }
};

#endif