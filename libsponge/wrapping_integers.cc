#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return WrappingInt32(isn + uint32_t(n)); }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t tmp = 0;
    uint64_t tmp_pre = 0;
    if (n - isn < 0) {
        tmp = uint64_t(n - isn + (1l << 32));
    } else {
        tmp = uint64_t(n - isn);
    }
    if (tmp >= checkpoint) {
        return tmp;
    }
    //! \note An important step.
    // This would help to avoid long time of while loops.
    // It turns tmp to be some int just slightly smaller than checkpoint.
    // Otherwise there tmp needs to add 2^32 many times to get near to checkpoint.
    tmp |= ((checkpoint >> 32) << 32);

    while (tmp <= checkpoint)
        tmp += (1ll << 32);
    tmp_pre = tmp - (1ll << 32);
    return (checkpoint - tmp_pre < tmp - checkpoint) ? tmp_pre : tmp;
}
