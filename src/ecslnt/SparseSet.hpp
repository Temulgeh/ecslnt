#ifndef ECSLNT_SPARSE_SET_INCLUDED
#define ECSLNT_SPARSE_SET_INCLUDED

#include <algorithm>
#include <optional>
#include <utility>
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
        using Iterator = typename std::vector<T>::iterator;
        using ConstIterator = typename std::vector<T>::const_iterator;

        bool contains(ID id) const;

        // NOTE: these should probably return an enum but i don't know how to
        // name it
        bool insert(ID id, const T& value);
        bool insert(ID id, T&& value);
        bool set_or_insert(ID id, const T& value);
        bool set_or_insert(ID id, T&& value);
        bool remove(ID id);

        T& operator[](ID id);
        const T& operator[](ID id) const;
        // NOTE: interface inconsistent with STL, should maybe be renamed
        std::optional<T&> at(ID id);
        std::optional<const T&> at(ID id) const;

        // TODO: add iterators that also return the ID
        Iterator begin();
        ConstIterator begin() const;
        ConstIterator cbegin() const;
        Iterator end();
        ConstIterator end() const;
        ConstIterator cend() const;
    private:
        std::optional<ID> get_index_in_packed(ID id) const;
        void swap(ID index1, ID index2);
        bool insert_forward(ID id, T&& value);
        bool set_or_insert_forward(ID id, T&& value);
        void insert_raw(ID id, T&& value);

        // TEMP: these will not stay as vectors
        std::vector<ID> sparse_array; // should be PagedArray
        std::vector<ID> packed_array;
        std::vector<T> value_array; // might be better as a no realloc array
    };


    // id refers to id in the sparse array, whereas index refers to id in the
    // packed array.
    // TODO: find better names!!!!


    template <typename T>
    bool SparseSet<T>::contains(ID id) const
    {
        return get_index_in_packed(id).has_value();
    }

    template <typename T>
    bool SparseSet<T>::insert(ID id, const T& value)
    {
        return insert_forward(id, value);
    }

    template <typename T>
    bool SparseSet<T>::insert(ID id, T&& value)
    {
        return insert_forward(id, std::move(value));
    }

    template <typename T>
    bool SparseSet<T>::set_or_insert(ID id, const T& value)
    {
        return set_or_insert_forward(id, value);
    }

    template <typename T>
    bool SparseSet<T>::set_or_insert(ID id, T&& value)
    {
        return set_or_insert_forward(id, std::move(value));
    }

    template <typename T>
    bool SparseSet<T>::remove(ID id)
    {
        std::optional<ID> index_in_packed = get_index_in_packed(id);
        if (!index_in_packed)
            return false;
        // TODO: this will need to also remove unused pages in the paged array
        // when we'll actually use a paged array
        swap(*index_in_packed, packed_array.size() - 1);
        packed_array.pop_back();
        value_array.pop_back();
    }

    template <typename T>
    T& SparseSet<T>::operator[](ID id)
    {
        // this throws so it's consistent with c++ :3
        return value_array[*get_index_in_packed(id)];
    }

    // I lOVE BOILERPLATE I LOVE C++ BOILERPLATE
    template <typename T>
    const T& SparseSet<T>::operator[](ID id) const
    {
        return value_array[*get_index_in_packed(id)];
    }

    template <typename T>
    std::optional<T&> SparseSet<T>::at(ID id)
    {
        std::optional<ID> index_in_packed = get_index_in_packed(id);
        if (!index_in_packed)
            return std::nullopt;
        return packed_array[*index_in_packed];
    }

    // I LOVE BOILERPLATE I LOVE C++ BOILERPLATE
    template <typename T>
    std::optional<const T&> SparseSet<T>::at(ID id) const
    {
        std::optional<ID> index_in_packed = get_index_in_packed(id);
        if (!index_in_packed)
            return std::nullopt;
        return packed_array[*index_in_packed];
    }

    // boilerplate part 3
    template <typename T>
    typename SparseSet<T>::Iterator SparseSet<T>::begin()
    {
        return packed_array.begin();
    }

    template <typename T>
    typename SparseSet<T>::ConstIterator SparseSet<T>::begin() const
    {
        return packed_array.cbegin();
    }

    template <typename T>
    typename SparseSet<T>::ConstIterator SparseSet<T>::cbegin() const
    {
        return packed_array.cbegin();
    }

    template <typename T>
    typename SparseSet<T>::Iterator SparseSet<T>::end()
    {
        return packed_array.end();
    }

    template <typename T>
    typename SparseSet<T>::ConstIterator SparseSet<T>::end() const
    {
        return packed_array.cend();
    }

    template <typename T>
    typename SparseSet<T>::ConstIterator SparseSet<T>::cend() const
    {
        return packed_array.cend();
    }

    template <typename T>
    std::optional<ID> SparseSet<T>::get_index_in_packed(ID id) const
    {
        if (id > sparse_array.size())
            return std::nullopt;
        ID index_in_packed = sparse_array[id];
        if (index_in_packed > packed_array.size() ||
            packed_array[index_in_packed] != id
        )
            return std::nullopt;
        return std::make_optional(index_in_packed);
    }

    template <typename T>
    void SparseSet<T>::swap(ID index1, ID index2)
    {
        ID id1 = packed_array[index1];
        ID id2 = packed_array[index2];
        std::swap(sparse_array[id1], sparse_array[id2]);
        std::swap(packed_array[index1], packed_array[index2]);
        std::swap(value_array[index1], value_array[index2]);
    }

    // NOTE: is this necessary? since everything is already templated; needs
    // testing
    template <typename T>
    bool SparseSet<T>::insert_forward(ID id, T&& value)
    {
        if (contains(id))
            return false;
        insert_raw(id, value);
        return true;
    }

    template <typename T>
    bool SparseSet<T>::set_or_insert_forward(ID id, T&& value)
    {
        std::optional<ID> index_in_packed = get_index_in_packed(id);
        if (index_in_packed)
        {
            value_array[*index_in_packed] = std::forward(value);
            return true;
        }
        insert_raw(id, std::forward(value));
        return false;
    }

    template <typename T>
    void SparseSet<T>::insert_raw(ID id, T&& value)
    {
        // terribly inefficient, but the vector will be replaced by a paged
        // array
        if (id >= sparse_array.size())
            sparse_array.resize(id + 1);
        sparse_array[id] = packed_array.size();
        packed_array.push_back(id);
        value_array.push_back(std::forward(value));
    }
}

#endif
