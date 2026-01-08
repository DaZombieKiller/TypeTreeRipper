#pragma once
#include "common.hpp"
#include "MemLabelId.hpp"

template<Revision R, Variant V>
struct dynamic_array
{
    template<typename T>
    struct type
    {
    private:
        T *m_data = nullptr;
        MemLabelId<R, V> m_label;
        size_t m_size = 0;
        size_t m_capacity = 0;
    public:
        T *data()
        {
            return m_data;
        }

        T const *data() const
        {
            return m_data;
        }

        size_t size() const
        {
            return m_size;
        }

        size_t capacity() const
        {
            return m_capacity;
        }

        T &operator[](size_t index)
        {
            return m_data[index];
        }

        T const &operator[](size_t index) const
        {
            return m_data[index];
        }
    };
};
