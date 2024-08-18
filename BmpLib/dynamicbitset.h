#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

namespace BmpZipper {

class DynamicBitset
{
public:
    static constexpr std::size_t BITS_PER_BLOCK = 8;
    using block_t = std::uint8_t;

    DynamicBitset();
    DynamicBitset(std::size_t blocksCount, block_t blockValue);
    explicit DynamicBitset(std::vector<block_t>&& source) noexcept;
    DynamicBitset(const block_t* blockPtr, std::size_t numBlocks);

    void set(std::size_t bitIndex, bool value = true);

    void clear();
    void shrinkToFit();

    [[nodiscard]]
    std::size_t size() const;

    [[nodiscard]]
    std::size_t numBlocks() const;

    [[nodiscard]]
    bool empty() const;

    [[nodiscard]]
    bool test(std::size_t bitIndex) const;

    [[nodiscard]]
    block_t getBlockValue(std::size_t blockIndex) const;

    [[nodiscard]]
    const block_t* data() const;

    [[nodiscard]]
    block_t* data();

    static std::size_t getNumBlocksRequired(std::size_t bitsCount);

private:
    std::vector<block_t> m_buffer;
    std::size_t m_size;
};

}