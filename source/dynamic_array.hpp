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
        // The lowest bit in m_capacity is used as an "external pointer" check.
        // We set this when allocating memory ourselves, so that on reallocation it does not get used internally
        // and crash the engine.
        explicit type(MemLabelId<R, V> const &label)
            : m_label(label), m_capacity(1)
        {
        }

        explicit type(MemLabelId<R, V> const &label, const size_t initialSize)
            : m_label(label), m_size(initialSize), m_capacity(initialSize << 1 | 1)
        {
            m_data = static_cast<T*>(malloc(sizeof(T) * m_size));
            ::memset(static_cast<void*>(m_data), 0, sizeof(T) * m_size);
        }

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
