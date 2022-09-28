#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) :
       	_output(capacity), 
	_capacity(capacity), 
	unassemble_base(0), 
	unassemble_size(0), 
	_eof(0),
	buffer(capacity,'\0'),
	bitmapping(capacity, false) 
	{}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::check_contiguous() {
    string temp = "";
    while (bitmapping.front()) {
	temp += buffer.front();
	buffer.pop_front();
	bitmapping.pop_front();
	buffer.push_back('\0');
	bitmapping.push_back(false);
    }
    size_t len = temp.length();
    if (len > 0) {
	_output.write(temp);
	unassemble_base += len;
	unassemble_size -= len;
    }
}

void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof) {
    	_eof = true;
    }
    size_t len = data.length();
    if (len == 0 && _eof && unassemble_size == 0){
	_output.end_input();
	return;
    }
    if (index >= unassemble_base + _capacity) { return; }
    if (index >= unassemble_base) {
    	int offset = index - unassemble_base;
	size_t real_len = min(len, _capacity - _output.buffer_size() - offset);
	if (real_len < len) {
		_eof = false;
	}
	for (size_t i = 0; i < real_len; i++){
		if (bitmapping[i + offset])
			continue;
		buffer[i + offset] = data[i];
		bitmapping[i + offset] = true;
		unassemble_size++;
	}
    }else if (index + len > unassemble_base){
    	int offset = unassemble_base - index;
	size_t real_len = min(len - offset, _capacity - _output.buffer_size());
	if (real_len < len - offset) { _eof = false; } 
	for (size_t i = 0; i < real_len; i++) {
		if (bitmapping[i])
			continue;
		buffer[i] = data[i + offset];
		bitmapping[i] = true;
		unassemble_size++;
	}
    }
    check_contiguous();
    if (_eof && unassemble_size == 0) {
    	_output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassemble_size; }

bool StreamReassembler::empty() const { return unassemble_size == 0; }
