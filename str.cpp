#include "str.h"
#include "error.h"
#include "hashcode.h"
#include <cstring>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iomanip>
using namespace std;

/***** StringView Implementation *****/
StringView::StringView(char ch) {
    /* Make a string buffer of just this character. */
    _valueBuffer[0] = ch;
    _valueBuffer[1] = 0;

    /* Delineate the range. */
    _begin = _valueBuffer;
    _end   = _valueBuffer + 1;
}

StringView::StringView(const char* text) {
    if (text == nullptr) {
        error("Cannot treat a null pointer as a String.");
    }

    /* Assume C-style string. */
    _begin = text;
    _end   = text + strlen(text);
}

StringView::StringView(const std::string& text) {
    /* Grab the whole range, even if it contains null characters. */
    _begin = text.data();
    _end   = text.data() + text.size();
}

StringView::StringView(const String& text) {
    /* Grab the whole range, even if it contains null characters. */
    _begin = text.c_str();
    _end   = text.c_str() + text.length();
}

const char* StringView::begin() const {
    return _begin;
}
const char* StringView::end() const {
    return _end;
}

char StringView::operator[] (int index) const {
    if (index < 0 || index > size()) {
        error("String index out of range.");
    }
    return _begin[index];
}

int StringView::size() const {
    return _end - _begin;
}

/***** String implementation *****/

String::String(StringView text) : _data(text.begin(), text.end()) {
    // Handled in initialization list
}

String::String(int numCopies, char ch) {
    if (numCopies < 0) {
        error("String::String: numCopies < 0");
    }
    _data.assign(numCopies, ch);
}

char String::operator[](int index) const {
    checkIndex(index, "operator[]", 0, length() - 1);
    return _data[index];
}

char& String::operator[](int index) {
    checkIndex(index, "operator[]", 0, length() - 1);
    return _data[index];
}

bool String::isEmpty() const {
    return _data.empty();
}

int String::length() const {
    return int(_data.length());
}

void String::checkIndex(int index, const char* why, int low, int high) const {
    if (index < low || index > high) {
        ostringstream builder;
        builder << "String::" << why << ": Index " << index << " is out of range [" << low << " .. " << high << "]";
        error(builder.str());
    }
}

bool String::contains(StringView text) const {
    return _data.find(text.begin(), 0, text.size()) != string::npos;
}

int String::find(StringView text, int startIndex) const {
    /* Bounds-checking. */
    if (startIndex < 0) {
        error("String::find: Start index must be greater than or equal to zero.");
    }

    /* Cast to/from unsigned. */
    auto result = _data.find(text.begin(), string::size_type(startIndex), text.size());
    return result == string::npos? -1 : int(result);
}

int String::findFromBack(StringView text) const {
    return findFromBack(text, length());
}

int String::findFromBack(StringView text, int lastIndex) const {
    /* Bounds-checking. */
    if (lastIndex < 0) {
        error("String::findFromBack: Start index must be greater than or equal to zero.");
    }

    /* Cast to/from unsigned. */
    auto result = _data.rfind(text.begin(), string::size_type(lastIndex), text.size());
    return result == string::npos? -1 : int(result);
}

String String::substr(int start) const {
    return substr(start, length());
}

String String::substr(int start, int length) const {
    checkIndex(start, "substr", 0, this->length());
    if (length < 0) error("String::substr: Negative length.");

    return String(_data.substr(size_t(start), size_t(length)));
}

void String::remove(int index) {
    remove(index, 1);
}

void String::remove(int index, int length) {
    checkIndex(index, "remove", 0, this->length());
    if (length < 0) error("String::remove: Negative length.");

    _data.erase(index, length);
    updateVersion();
}

void String::insert(int index, StringView text) {
    checkIndex(index, "insert", 0, length());

    _data.insert(size_t(index), text.begin(), text.size());
    updateVersion();
}

String& String::operator+= (StringView text) {
    _data.append(text.begin(), text.size());
    updateVersion();
    return *this;
}

String String::operator+ (StringView text) const {
    String result = *this;
    result += text;
    return result;
}

void String::toLowerCase() {
    transform(_data.begin(), _data.end(), _data.begin(), ::tolower);
}

void String::toUpperCase() {
    transform(_data.begin(), _data.end(), _data.begin(), ::toupper);
}

String String::asLowerCase() const {
    String result = *this;
    result.toLowerCase();
    return result;
}

String String::asUpperCase() const {
    String result = *this;
    result.toUpperCase();
    return result;
}

namespace {
    /* Source: StanfordCPPLib */
    void trimStartInPlace(std::string& str) {
        int start = 0;
        int finish = (int) str.length() - 1;
        while (start <= finish && isspace(str[start])) {
            start++;
        }
        if (start > 0) {
            str.erase(0, start);
        }
    }

    /* Source: StanfordCPPLib */
    void trimEndInPlace(std::string& str) {
        int end = (int) str.length();
        int finish = end;
        while (finish > 0 && isspace(str[finish - 1])) {
            finish--;
        }
        if (finish < end) {
            str.erase(finish, end - finish);
        }
    }

    /* Source: StanfordCPPLib */
    void trimInPlace(std::string& str) {
        trimEndInPlace(str);
        trimStartInPlace(str);
    }
}

void String::trimFront() {
    trimStartInPlace(_data);
    updateVersion();
}

void String::trimBack() {
    trimEndInPlace(_data);
    updateVersion();
}

void String::trim() {
    trimInPlace(_data);
    updateVersion();
}

String String::trimmed() const {
    String result = *this;
    result.trim();
    return result;
}

String String::frontTrimmed() const {
    String result = *this;
    result.trimFront();
    return result;
}

String String::backTrimmed() const {
    String result = *this;
    result.trimBack();
    return result;
}

namespace {
    /* Slightly modified from StanfordCPPLib */
    Vector<String> stringSplit(const std::string& str, StringView delimiter) {
        if (delimiter.size() == 0) {
            error("stringSplit: Delimiter cannot be the empty string.");
        }

        std::string str2 = str;
        Vector<String> result;
        int count = 0;
        size_t index = 0;

        /* TODO: Quadratic? Fix this. */
        while (true) {
            index = str2.find(delimiter.begin(), 0, delimiter.size());
            if (index == std::string::npos) {
                break;
            }
            // don't add empty token, coalesce adjacent/leading delimiters
            if (index != 0) result.add(str2.substr(0, index));
            str2.erase(str2.begin(), str2.begin() + index + delimiter.size());
            count++;
        }
        if ((int) str2.length() > 0) {
            result.add(str2);
        }

        return result;
    }
}

Vector<String> String::split(StringView delimiters) const {
    return stringSplit(_data, delimiters);
}

void String::replaceAll(StringView text, StringView with) {
    /* How long is the string to replace with? */
    size_t textLength = text.size();
    if (textLength == 0) {
        error("String::replaceAll: Cannot replace the empty string.");
    }

    size_t replaceLength = with.size();
    for (size_t i = _data.find(text.begin(), 0, text.size()); i != string::npos; i = _data.find(text.begin(), i + replaceLength, text.size())) {
        _data.replace(i, textLength, with.begin(), with.size());
        updateVersion();
    }
}

void String::updateVersion() {
    _version.update();
}

String::iterator String::begin() {
    return { &_version, _data.begin(), _data };
}

String::iterator String::end() {
    return { &_version, _data.end(), _data };
}

String::const_iterator String::begin() const {
    return { &_version, _data.begin(), _data };
}

String::const_iterator String::end() const {
    return { &_version, _data.end(), _data };
}

ostream& operator<< (ostream& out, const String& toWrite) {
    return out << toWrite._data;
}

istream& operator>> (istream& in,  String& toRead) {
    if (in >> toRead._data) toRead.updateVersion();
    return in;
}

istream& getline(std::istream& in, String& toRead) {
    if (getline(in, toRead._data)) toRead.updateVersion();
    return in;
}

string String::asStdString() const {
    return _data;
}

const char* String::c_str() const {
    return _data.c_str();
}

bool String::operator< (StringView lhs) const {
    return std::lexicographical_compare(_data.begin(), _data.end(), lhs.begin(), lhs.end());
}

bool String::operator> (StringView lhs) const {
    return lhs < StringView(*this);
}

bool String::operator==(StringView lhs) const {
    /* Same size and contents. */
    return length() == lhs.size() && equal(begin(), end(), lhs.begin());

}

bool String::operator!=(StringView lhs) const {
    return !(*this == lhs);
}

bool String::operator<=(StringView lhs) const {
    return !(lhs < StringView(*this));
}

bool String::operator>=(StringView lhs) const {
    return !(*this < lhs);
}

int hashCode(const String& str) {
    return hashCode(str._data);
}

String::operator const string &() const {
    return _data;
}

String::operator string &() {
    /* The conservative option is to not do version updating here,
     * since if someone uses the non-const conversion to string
     * but doesn't actually update the string we don't want to
     * invalidate any iterators. But we may want to revisit this.
     */
    return _data;
}

char toUpper(char ch) {
    return toupper(ch);
}

char toLower(char ch) {
    return tolower(ch);
}

bool isAlpha(char ch) {
    return !!isalpha(ch);
}

bool isDigit(char ch) {
    return !!isdigit(ch);
}

bool isSpace(char ch) {
    return !!isspace(ch);
}

bool isAlnum(char ch) {
    return !!isalnum(ch);
}

bool isPrint(char ch) {
    return !!isprint(ch);
}

bool isPunct(char ch) {
    return !!ispunct(ch);
}

bool String::startsWith(StringView prefix) const {
    /* Modified from StanfordCPPLib. */
    if (_data.length() < prefix.size()) return false;

    for (int i = 0; i < prefix.size(); i++) {
        if (_data[i] != prefix[i]) return false;
    }
    return true;
}

bool String::endsWith(StringView suffix) const {
    /* Modified from StanfordCPPLib. */
    int start = length() - suffix.size();
    if (start < 0) return false;

    for (int i = 0; i < suffix.size(); i++) {
        if (_data[start + i] != suffix[i]) return false;
    }
    return true;
}

String String::join(const Vector<String>& v, const StringView delimiter) {
    String result;
    for (int i = 0; i < v.size(); i++) {
        result += v[i];
        if (i + 1 != v.size()) {
            result += delimiter;
        }
    }
    return result;
}

String String::urlDecoded() const {
    /* Taken from StanfordCPPLib. */
    std::ostringstream unescaped;
    for (auto i = begin(), n = end(); i != n; ++i) {
        char c = *i;
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '*') {
            unescaped << c;
        } else if (c == '+')  {
            unescaped << ' ';
        } else if (c == '%') {
            // throw error if string is invalid and doesn't have 2 char after,
            // or if it has non-hex chars here (courtesy GitHub @scinart)
            if (i + 2 >= n || !isxdigit(*(i + 1)) || !isxdigit(*(i + 2))) {
                error("urlDecode: Invalid percent-encoding");
            }

            // decode a URL-encoded ASCII character, e.g. %40 => &
            char ch1 = *(i + 1);
            char ch2 = *(i + 2);
            int hex1 = (isdigit(ch1) ? (ch1 - '0') : (toupper(ch1) - 'A' + 10));
            int hex2 = (isdigit(ch2) ? (ch2 - '0') : (toupper(ch2) - 'A' + 10));
            int decodedChar = (hex1 << 4) + hex2;
            unescaped << (char) decodedChar;
            i += 2;
        } else {
            std::ostringstream msg;
            msg << "urlDecode: Unexpected character in string: "
                << (int) c << " (" << c << ")";
            error(msg.str());
        }
    }

    return unescaped.str();
}

String String::urlEncoded() const {
    /* Taken from StanfordCPPLib */
    std::ostringstream escaped;
    escaped << hex << setfill('0') << uppercase;

    for (auto i = begin(), n = end(); i != n; ++i) {
        char c = *i;
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '*') {
            escaped << c;
        } else if (c == ' ')  {
            escaped << '+';
        } else {
            escaped << '%' << setw(2) << int(c) << setw(0);
        }
    }

    return escaped.str();
}
