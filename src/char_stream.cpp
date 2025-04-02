#include "char_stream.hpp"

char CharStream::get() {
    char ch;
    if (!input.get(ch)) return EOF;

    if (ch == '\r') {
        if (input.peek() == '\n') {
            input.get();
        }
        ch = '\n';
        return ch;
    }
    return ch;
}

char CharStream::peek() {
    char ch = input.peek();
    if (ch == '\r') return '\n';
    return ch;
}

bool CharStream::eof() const {
    return input.eof();
}