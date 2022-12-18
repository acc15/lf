#include <cassert>
#include <algorithm>

#include "bitset.hpp"

namespace leafsync
{

bitset::bitset(): _size(0), _blocks() {
}

bitset::bitset(const bitset& copy): _size(copy._size), _blocks(copy._blocks) {
}

bitset::bitset(bitset&& move): _size(move._size), _blocks(std::move(move._blocks)) {
    move._size = 0;
}

void bitset::resize(bitset::size_type n) {
    size_type new_blocks = min_blocks(n);
    if (_blocks.size() < new_blocks) {
        _blocks.resize(new_blocks);
    }
    _size = n;
}

void bitset::set(bitset::size_type i, bool value) {
    if (value) {
        bit_block(i) |= bit_mask(i);
    } else {
        bit_block(i) &= ~bit_mask(i);
    }
}

bool bitset::get(bitset::size_type i) const {
    assert(i < _size);
    return (bit_block(i) & bit_mask(i)) != 0;
}

bool bitset::operator[](bitset::size_type i) const {
    return get(i);
}

bitset& bitset::operator=(const bitset& other) {
    _size = other._size;
    _blocks = other._blocks;
    return *this;
}

bitset& bitset::operator=(bitset&& other) {
    _size = other._size;
    _blocks = std::move(other._blocks);
    other._size = 0;
    return *this;
}

bitset::size_type bitset::size() const {
    return _size;
}

const std::vector<bitset::block_type>& bitset::blocks() const {
    return _blocks;
}

std::vector<bitset::block_type>& bitset::blocks() {
    return _blocks;
}

constexpr bitset::size_type bitset::min_blocks(bitset::size_type n) {
    return (n + bits_per_block - 1) / bits_per_block;
}

constexpr bitset::block_type bitset::bit_mask(bitset::size_type i) {
    return static_cast<block_type>(1 << (i % bits_per_block));
}

bitset::block_type& bitset::bit_block(bitset::size_type i) {
    assert(i < _size);
    return _blocks[i / bits_per_block];
}

const bitset::block_type& bitset::bit_block(bitset::size_type i) const {
    return const_cast<bitset*>(this)->bit_block(i);
}

}