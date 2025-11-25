#include "Structs.hpp"

#include <Windows.h>

std::string Il2CppString::toString() const {
    if (!this || this->length == 0) {
        return "";
    }

    const wchar_t* utf16Chars = this->chars;
    const int utf16Len        = this->length;

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, utf16Chars, utf16Len, nullptr, 0, nullptr, nullptr);

    if (utf8Len == 0) {
        return "";
    }

    std::string utf8Str(utf8Len, 0);
    WideCharToMultiByte(CP_UTF8, 0, utf16Chars, utf16Len, &utf8Str[0], utf8Len, nullptr, nullptr);

    return utf8Str;
}
