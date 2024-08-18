#include "dynamicbitset.h"
#include <stdexcept>

namespace BmpZipper {

DynamicBitset::DynamicBitset()
    : m_size(0)
{
}

DynamicBitset::DynamicBitset(const std::size_t blocksCount, const block_t blockValue)
    : m_buffer(blocksCount, blockValue), m_size(0)
{
}

DynamicBitset::DynamicBitset(std::vector<block_t> && source) noexcept
    : m_buffer(std::move(source)), m_size(m_buffer.size() * BITS_PER_BLOCK)
{
}

DynamicBitset::DynamicBitset(const block_t* blockPtr, const std::size_t numBlocks)
    : m_buffer(blockPtr, blockPtr + numBlocks), m_size(numBlocks * BITS_PER_BLOCK)
{
}

void DynamicBitset::set(const std::size_t bitIndex, const bool value)
{
   const auto [quot, rem] = div(static_cast<int>(bitIndex), BITS_PER_BLOCK);
    if(empty())
        m_buffer.resize(1, 0x00);
    if(quot >= numBlocks())
        m_buffer.resize(quot << 1, 0x00); // Resize bitset x2 for required bit

    m_buffer[quot] |= static_cast<std::uint8_t>(value) << rem;
    if(bitIndex >= m_size)
        m_size = bitIndex + 1;
}

void DynamicBitset::clear()
{
    m_buffer.clear();
    m_size = 0;
}

void DynamicBitset::shrinkToFit()
{
    const std::size_t numBlocksRequired = getNumBlocksRequired(m_size);
    m_buffer.resize(numBlocksRequired);
}

std::size_t DynamicBitset::size() const
{
    return m_size;
}

std::size_t DynamicBitset::numBlocks() const
{
    return m_buffer.size();
}

bool DynamicBitset::empty() const
{
    return m_buffer.empty();
}

bool DynamicBitset::test(const std::size_t bitIndex) const
{
    if(bitIndex >= m_size)
        throw std::out_of_range("Bit index is out of range");

    const auto [quot, rem] = div(static_cast<int>(bitIndex), BITS_PER_BLOCK);
    return (m_buffer[quot] & 1 << rem) != 0;
}

DynamicBitset::block_t DynamicBitset::getBlockValue(const std::size_t blockIndex) const
{
    return m_buffer.at(blockIndex);
}

std::size_t DynamicBitset::getNumBlocksRequired(const std::size_t bitsCount)
{
    return (bitsCount + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK;
}

const DynamicBitset::block_t * DynamicBitset::data() const
{
    return m_buffer.data();
}

DynamicBitset::block_t * DynamicBitset::data()
{
    return m_buffer.data();
}

}