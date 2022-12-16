#pragma once

#include <limits>

namespace leafsync {

class bitset {
public:
    typedef unsigned int size_type;
    typedef unsigned char block_type;

    bitset();
    bitset(const bitset& other);
    bitset(bitset&& other);
    ~bitset();

    void resize(size_type n);
    void set(size_type i, bool value);
    bool get(size_type i) const;

    bool operator[](size_type i) const;
    bitset& operator=(const bitset& other);
    bitset& operator=(bitset&& other);

    size_type size() const;
    size_type blocks() const;

    const block_type* buf() const; 
    block_type* buf(); 

private:
    static constexpr size_type bits_per_block = std::numeric_limits<block_type>::digits;

    static size_type min_blocks(size_type n);
    static size_type bit_mask(size_type i);
    void free_buf();

    size_type _size;
    size_type _blocks;
    block_type* _buf;

};

}