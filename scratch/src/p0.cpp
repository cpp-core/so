// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

using std::cout, std::endl;

template<class Iterator>
void codepoint_to_utf8(uint32_t code, Iterator iter) {
    if (code <= 0x007F) {
	*iter++ = static_cast<uint8_t>(code bitand 0x7F);
    }
    else if (code <= 0x07FF) {
	*iter++ = 0xC0 bitor static_cast<uint8_t>((code >> 6) bitand 0x1F);
	*iter++ = 0x80 bitor static_cast<uint8_t>(code bitand 0x3F);
    }
    else if (code <= 0xFFFF) {
	*iter++ = 0xE0 bitor static_cast<uint8_t>((code >> 12) bitand 0x0F);
        *iter++ = 0x80 bitor static_cast<uint8_t>((code >> 6) bitand 0x3F);
        *iter++ = 0x80 bitor static_cast<uint8_t>(code bitand 0x3F);
    }
    else if (code <= 0x10'FFFF) {
        *iter++ = 0xF0 bitor static_cast<uint8_t>((code >> 18) bitand 0x07);
        *iter++ = 0x80 bitor static_cast<uint8_t>((code >> 12) bitand 0x3F);
        *iter++ = 0x80 bitor static_cast<uint8_t>((code >> 6) bitand 0x3F);
        *iter++ = 0x80 bitor static_cast<uint8_t>(code bitand 0x3F);
    }
}

template<class Iterator>
uint32_t utf8_sequence_to_codepoint(Iterator iter) {
    auto byte = static_cast<uint32_t>(*iter++);
    if (byte <= 0x7F) {
        return byte;
    }
    else if ((byte bitand 0xE0) == 0xC0) {
        uint32_t r = (byte bitand 0x1F) << 6;
        r |= static_cast<uint32_t>(*iter++) bitand 0x3F;
        return r;
    }
    else if ((byte bitand 0xF0) == 0xE0) {
        uint32_t r = (byte bitand 0x0F) << 12;
        r |= (static_cast<uint32_t>(*iter++) bitand 0x3F) << 6;
        r |= static_cast<uint32_t>(*iter++) bitand 0x3F;
        return r;
    }
    else if ((byte bitand 0xF8) == 0xF0) {
        uint32_t r = (byte bitand 0x07) << 18;
        r |= (static_cast<uint32_t>(*iter++) bitand 0x3F) << 12;
        r |= (static_cast<uint32_t>(*iter++) bitand 0x3F) << 6;
        r |= static_cast<uint32_t>(*iter++) bitand 0x3F;
        return r;
    }
    else
        return 0xFFFF'FFFF;
}

void examine(uint32_t code) {
    std::string str;
    codepoint_to_utf8(code, std::back_inserter(str));
    cout << std::hex << code << " = " << str << " = ";
    cout << std::hex << utf8_sequence_to_codepoint(str.begin()) << endl;
}

int main(int argc, const char *argv[]) {
    examine(0x0024);
    examine(0x00A3);
    examine(0x0418);
    examine(0x0939);
    examine(0x20AC);
    examine(0xD55C);
    examine(0x1'0348);
    return 0;
}
