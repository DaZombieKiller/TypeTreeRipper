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
        explicit type(MemLabelId<R, V> const &label)
            : m_label(label), m_capacity(1)
        {
        }

        explicit type(MemLabelId<R, V> const &label, const size_t initialSize)
            : m_label(label)
        {
            assign_external(new T[initialSize], initialSize);
        }

        void assign_external(T* ptr, const size_t size)
        {
            // The lowest bit in m_capacity is used as an "external pointer" check.
            // We set this when allocating memory ourselves, so that on reallocation it does not get used internally
            // and crash the engine.

            m_data = ptr;
            m_size = size;
            m_capacity = (size << 1) | 1;
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
