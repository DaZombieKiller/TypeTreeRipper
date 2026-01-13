#pragma once
#include "common.hpp"
#include "MemLabelId.hpp"

template<Revision R, Variant V>
struct dynamic_array_traits;

DECLARE_REVISION(dynamic_array_traits, Revision::V2018_3_0);

template<Revision R, Variant V>
struct dynamic_array_traits
{
    // We set this when allocating memory ourselves, so that on reallocation it does not get used internally
    // and crash the engine.
    static constexpr size_t k_reference_bit = ~((~0zu << 1) >> 1);
    static constexpr size_t k_capacity_shift = 0;
};

DEFINE_REVISION(struct, dynamic_array_traits, Revision::V2018_3_0)
{
    static constexpr size_t k_reference_bit = 1;
    static constexpr size_t k_capacity_shift = 1;
};

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
            : m_label(label), m_capacity(dynamic_array_traits<R, V>::k_reference_bit)
        {
        }

        explicit type(MemLabelId<R, V> const &label, const size_t initialSize)
            : m_label(label)
        {
            assign_external(new T[initialSize], initialSize, initialSize);
        }

        void assign_external(T* ptr, const size_t size, const size_t capacity)
        {
            m_data = ptr;
            m_size = size;
            m_capacity = (capacity << dynamic_array_traits<R, V>::k_capacity_shift) | dynamic_array_traits<R, V>::k_reference_bit;
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
            return (m_capacity & ~dynamic_array_traits<R, V>::k_reference_bit) >> dynamic_array_traits<R, V>::k_capacity_shift;
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
