#ifndef ECSLNT_SPARSE_SET_INCLUDED
#define ECSLNT_SPARSE_SET_INCLUDED

#include <vector>


namespace ecslnt
{
    using ID = unsigned; // TEMP: should let the user decide

    template <typename T>
    class SparseSet
    {
        // TODO
        // base functionality: inserting, removing, random access, iterating
        // custom vector-like types and memory management
        // emplace add
        // add qualifiers where appropriate (constexpr, nodiscard, noexcept...)
    public:
        SparseSet();

        class Iterator;
        class ConstIterator;

        bool contains(ID id) const;
        Iterator find(ID id) const;

        // NOTE: these should probably return an enum but i don't know how to
        // name it
        bool insert(ID id, const T& value);
        bool insert(ID id, T&& value);
        bool remove(ID id);
        bool set_or_insert(ID id, const T& value);
        bool set_or_insert(ID id, T&& value);

        T& operator[](ID index);
        const T& operator[](ID index) const;
        Iterator begin();
        ConstIterator begin() const;
        ConstIterator cbegin() const;
        Iterator end();
        ConstIterator end() const;
        ConstIterator cend() const;
    private:
        // TEMP: these will not stay as vectors
        std::vector<ID> sparse_array; // should be PagedArray
        std::vector<ID> packed_array;
        std::vector<T> value_array; // might be better as a no realloc array
    };
}

#endif
