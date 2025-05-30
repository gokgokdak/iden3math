#include <iden3math/bitvec.h>
#include <gtest/gtest.h>

namespace iden3math {

TEST(bitvec, constructor) {
    BitVec1D bv;
    EXPECT_EQ(bv.size(), 0);
}

TEST(bitvec, access_out_of_index) {
    BitVec1D bv;
    // Test out of index access
    EXPECT_THROW(bv[1], std::out_of_range);
    for (size_t i = 0; i < 8; ++i) {
        bv.push(false);
    }
    EXPECT_THROW(bv[8], std::out_of_range);
    bv.push(false);
    EXPECT_THROW(bv[9], std::out_of_range);
}

TEST(bitvec, push_and_access) {
    BitVec1D bv;
    
    // Test pushing bits
    bv.push(true);
    EXPECT_EQ(bv[0], true);
    
    bv.push(false);
    EXPECT_EQ(bv[1], false);

    bv.push(true);
    EXPECT_EQ(bv[2], true);

    // Test bits across byte boundaries
    for(int i = 0; i < 10; i++) {
        bv.push(i % 2 == 0);
    }

    EXPECT_EQ(bv[3], true);
    EXPECT_EQ(bv[4], false);
    EXPECT_EQ(bv[5], true);
    EXPECT_EQ(bv[6], false);
    EXPECT_EQ(bv[7], true);
    EXPECT_EQ(bv[8], false);
    EXPECT_EQ(bv[9], true);
    EXPECT_EQ(bv[10], false);
    EXPECT_EQ(bv[11], true);
    EXPECT_EQ(bv[12], false);
}

TEST(bitvec, byte_boundary) {
    BitVec1D bv;
    
    // Push 8 bits to fill first byte
    for(int i = 0; i < 8; i++) {
        bv.push(true);
    }
    
    // Verify all bits in first byte
    for(int i = 0; i < 8; i++) {
        EXPECT_EQ(bv[i], true);
    }
    
    // Push more bits to test byte boundary crossing
    bv.push(false);
    bv.push(true);
    
    EXPECT_EQ(bv[8], false);
    EXPECT_EQ(bv[9], true);
}

TEST(bitvec, alternating_pattern) {
    BitVec1D bv;
    
    // Push alternating pattern
    for(int i = 0; i < 16; i++) {
        bv.push(i % 2 == 0);
    }
    
    // Verify pattern
    for(int i = 0; i < 16; i++) {
        EXPECT_EQ(bv[i], i % 2 == 0);
    }
}

} // namespace iden3math
