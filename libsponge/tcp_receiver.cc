#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();

    if (!head.syn && !_receivedSYN) {
        return;
    }

    bool eof = false;

    string data = seg.payload().copy();

    if (head.syn && !_receivedSYN) {
        _ISN = head.seqno;
        _receivedSYN = true;
        if (head.fin) {
            _receivedFIN = eof = true;
        }
        _reassembler.push_substring(data, 0, eof);
        return;
    }

    if (_receivedSYN && head.fin) {
        _receivedFIN = eof = true;
    }

    uint64_t checkpoint = _reassembler.ack_num();
    uint64_t abs_seqno = unwrap(head.seqno, _ISN, checkpoint);
    uint64_t stream_idx = abs_seqno - _receivedSYN;

    _reassembler.push_substring(data, stream_idx, eof);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_receivedSYN) {
        return nullopt;
    }
    return wrap(_reassembler.ack_num() + 1 + (_receivedFIN && _reassembler.empty()), _ISN);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
