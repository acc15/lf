#include <cassert>
#include <algorithm>

#include "bitset.hpp"

namespace leafsync
{

bitset::bitset(): 
    _size(0), 
    _blocks(0), 
    _buf(nullptr) 
{
}

bitset::bitset(const bitset& other): 
    _size(other._size), 
    _blocks(min_blocks(_size)), 
    _buf(_blocks == 0 ? nullptr : new block_type[_blocks]) 
{
    std::copy(other._buf, other._buf + _blocks, _buf);
}

bitset::bitset(bitset&& other): 
    _size(other._size), 
    _blocks(other._blocks), 
    _buf(other._buf) 
{
    other._buf = nullptr;
    other._size = 0;
    other._blocks = 0;
}

bitset::~bitset() {
    free_buf();
}

void bitset::resize(bitset::size_type n) {
    size_type new_blocks = min_blocks(n);
    if (_blocks < new_blocks) {
        if (_buf == nullptr) {
            _buf = new block_type[new_blocks]();
        } else {
            block_type* new_buf = new block_type[new_blocks];
            std::copy(_buf, _buf + _blocks, new_buf);
            delete [] _buf;
            _buf = new_buf;

            std::fill(_buf + _blocks, _buf + new_blocks, 0);
        }
        _blocks = new_blocks;
    }
    _size = n;
}

void bitset::set(bitset::size_type i, bool value) {
    assert(i < _size);
    block_type& b = _buf[i / bits_per_block];
    block_type v = 1 << (i % bits_per_block);
    if (value) {
        b |= v;
    } else {
        b &= ~v;
    }
}

bool bitset::get(bitset::size_type i) const {
    assert(i < _size);
    return (_buf[i / bits_per_block] & (1 << (i % bits_per_block))) != 0;
}

bool bitset::operator[](bitset::size_type i) const {
    return get(i);
}

bitset& bitset::operator=(const bitset& other) {
    resize(other._size);
    std::copy(other._buf, other._buf + _blocks, _buf);
    return *this;
}

bitset& bitset::operator=(bitset&& other) {
    free_buf();
    _size = other._size;
    _blocks = other._blocks;
    _buf = other._buf;
    other._buf = nullptr;
    other._blocks = 0;
    other._size = 0;
    return *this;
}

bitset::size_type bitset::size() const {
    return _size;
}

bitset::size_type bitset::blocks() const {
    return _blocks;
}

const bitset::block_type* bitset::buf() const {
    return _buf;
}

bitset::block_type* bitset::buf() {
    return _buf;
} 

bitset::size_type bitset::min_blocks(bitset::size_type n) {
    return (n + bits_per_block - 1) / bits_per_block;
}

bitset::size_type bitset::bit_mask(bitset::size_type i) {
    return 1 << (i % bits_per_block);
}

void bitset::free_buf() {
    if (_buf != nullptr) {
        delete [] _buf;
    }
}

}