#pragma once

#include "collections.h"
#include "vector.h"
#include <iostream>
#include <string>

class String;

/* Thin wrapper type around a unit of text; used to unify the interface
 * and avoid making copies of things. This makes it possible to have the
 * String type accept anything that's "text-y": std::string, char,
 * and const char*, but not "un-text-y," like int, nullptr, etc.
 */
class StringView {
public:
    /* Can convert characters to strings, but not integers. */
    StringView(char ch);
    StringView(int) = delete;
    StringView(double) = delete;
    StringView(short) = delete;
    StringView(float) = delete;
    StringView(long) = delete;
    StringView(long double) = delete;

    /* String types are okay. */
    StringView(const std::string& text);
    StringView(const String& text);

    /* Can convert C strings to Strings, but not null pointers. */
    StringView(const char* text);
    StringView(std::nullptr_t) = delete;

    /* Data range. */
    const char* begin() const;
    const char* end() const;

    /* Accessor. */
    char operator[] (int index) const;

    /* Size. */
    int size() const;

private:
    const char* _begin;
    const char* _end;
    char _valueBuffer[2]; // Storage for single character and null terminator
};

#include <type_traits>

namespace stanfordcpplib {
    namespace string {
        /* Tests to see if something is a text type or a String. */
        template <typename T> using IsText   = std::is_convertible<T, StringView>;
        template <typename T> using IsString = std::is_same<T, String>;
    }
}

class String {
public:
    /**
     * Initializes a new, empty String.
     * @bigoh O(1)
     */
    String() = default;

    /**
     * Initializes a new string from the given text.
     * @bigoh O(n)
     */
    String(StringView text);

    /**
     * Initializes a new string from the given text.
     * @bigoh O(n)
     */
    template <typename Text, typename = typename stanfordcpplib::string::IsText<Text>::type>
    String(Text&& text);

    /**
     * Initializes a new string that's the indicated number of copies
     * of the given character.
     * @bigoh O(n)
     */
    String(int n, char ch);

    /**
     * Returns the character at the given index. If the index is out of bounds,
     * an error is raised.
     * @bigoh O(1)
     */
    char  operator[](int index) const;

    /**
     * Returns the character at the given index. If the index is out of bounds,
     * an error is raised.
     * @bigoh O(1)
     */
    char& operator[](int index);

    /**
     * Returns whether the string is empty.
     * @bigoh O(1)
     */
    bool isEmpty() const;

    /**
     * Returns the length of the string.
     * @bigoh O(1)
     */
    int  length() const;

    /**
     * Returns whether the given text is present within the string.
     * @bigoh O(mn)
     */
    bool contains(StringView text) const;

    /**
     * Returns the index of the first copy of the given piece of text
     * that occurs at or after the given index. If the text does not
     * occur in the string, returns -1.
     * @bigoh O(mn)
     */
    int find(StringView text, int startIndex = 0) const;

    /**
     * Returns the index of the last copy of the given piece of text
     * that occurs at or before the given index. If the text does not
     * occur in the string, returns -1.
     * @bigoh O(mn)
     */
    int findFromBack(StringView text) const;
    int findFromBack(StringView text, int lastIndex) const;

    /**
     * Returns a substring of the input string starting at position
     * start and extending for up to length more characters (or the
     * end of the string, whichever is first.)
     * @bigoh O(n)
     */
    String substr(int start, int length) const;

    /**
     * Returns a substring of the input string starting at position
     * start and extending to the end of the string.
     * @bigoh O(n)
     */
    String substr(int start) const;

    /**
     * Removes the character at the given index from the string.
     * @bigoh O(n)
     */
    void remove(int index);

    /**
     * Removes the specified number of characters starting at the
     * specified index in the string. If that would remove past the
     * end of the string, instead we stop at the end of the string.
     * @bigoh O(n)
     */
    void remove(int start, int length);

    /**
     * Inserts the specified text at the indicated position.
     * @bigoh O(n+m)
     */
    void insert(int index, StringView text);

    /**
     * Appends the given text to the end of the string.
     * @bigoh O(m)
     */
    String& operator+= (StringView text);

    /**
     * Returns a new string formed by appending the given text to the
     * end of the string. The original string is not modified.
     * @bigoh O(n+m)
     */
    String operator+ (StringView text) const;

    /**
     * Returns whether the given string starts with the specified text.
     * @bigoh O(m)
     */
    bool startsWith(StringView text) const;

    /**
     * Returns whether the given string ends with the specified text.
     * @bigoh O(m)
     */
    bool endsWith(StringView text) const;

    /**
     * Converts all characters in the string to upper case. Due to how C++
     * handles the char type, this only works with characters in the English
     * alphabet. Any other characters are ignored.
     * @bigoh O(n)
     */
    void toLowerCase();

    /**
     * Converts all characters in the string to lower case. Due to how C++
     * handles the char type, this only works with characters in the English
     * alphabet. Any other characters are ignored.
     * @bigoh O(n)
     */
    void toUpperCase();

    /**
     * Returns the string produced by converting all English letters in the string
     * to lower case. The original string is not modified.
     * @bigoh O(n)
     */
    String asLowerCase() const;

    /**
     * Returns the string produced by converting all English letters in the string
     * to upper case. The original string is not modified.
     * @bigoh O(n)
     */
    String asUpperCase() const;

    /**
     * Removes all leading and/or trailing whitespace from the string.
     * @bigoh O(n)
     */
    void trim();
    void trimFront();
    void trimBack();

    /**
     * Returns the string formed by removing all leading and trailing whitespace
     * from the string. The original string is not modified.
     * @bigoh O(n)
     */
    String trimmed() const;
    String frontTrimmed() const;
    String backTrimmed() const;

    /**
     * Replaces all copies of the indicated text string with the new text string.
     * New copies of the replacement string that appear as a result of the
     * replacement are not themselves replaced.
     * @bigoh O(mn^2)
     */
    void replaceAll(StringView text, StringView with);

    /**
     * Splits the string into a sequence of strings by chopping at each point
     * where the given delimiter is provided.
     * @bigoh O(n)
     */
    Vector<String> split(StringView delimiter) const;

    /**
     * Joins the given Vector<String> together into a single String,
     * interpersing the given delimiter string between them. By default,
     * the delimiter string is a newline.
     * @bigoh O(n + k * m), where n is the total length of all the strings,
     *        k is the number of strings, and m is the length of the delimiter.
     *        (For any reasonable delimiter, this is O(n).)
     */
    static String join(const Vector<String>& v,
                       StringView delimiter = '\n');

    /**
     * Converts the indicated value to a String. For example:
     *
     *   String text = String::from(137);   // "137"
     *   String text = String::from(2.5);   // "2.5"
     *   String text = String::from(false); // "false"
     */
    template <typename T> static String from(const T& value);

    /**
     * Converts the indicated String to a value of a different type. The syntax
     * requires you to specify which type to convert to in angle braces. For
     * example:
     *
     *   String text = "137";
     *   int value = String::to<int>(text);
     *
     *   String text = "2.5";
     *   double value = String::to<double>(text);
     *
     *   String text = "false";
     *   bool value = String::to<bool>(text);
     *
     *   String text = '!';
     *   char value = String::to<char>(text);
     *
     * If the conversion fails, calls error() to report an error.
     */
    template <typename T> static T to(const String& text);

    /**
     * Converts the indicated String to an integer using the specified
     * base (e.g. base-10, base-8, or base-16). For example:
     *
     *   String text = "0xDEADBEEF";
     *   int value = String::to<int>(text, 16);
     *
     *   String text = "0744";
     *   int value = String::to<int>(text, 8);
     *
     *   String text = "137";
     *   int value = String::to<int>(text, 10);
     */
    template <typename T> static T to(const String& text, int radix);

    /**
     * Returns whether the given string can be converted to the given
     * type without actually returning the result of the conversion.
     */
    template <typename T> static bool is(const String& text);
    template <typename T> static bool is(const String& text, int radix);

    /**
     * Returns a URL-decoded version of the given string, where any %xx
     * character codes are converted back to the equivalent characters.
     * @bigoh O(n)
     */
    String urlDecoded() const;

    /**
     * Returns a URL-encoded version of the given string, where most non-
     * alphabetic characters are replaced by %xx character codes.
     * @bigoh O(n)
     */
    String urlEncoded() const;

private:
    std::string _data;
    stanfordcpplib::collections::VersionTracker _version;

    /* Updates the internal version count. Only our libraries need this, and they only
     * need it in rare cases where an operation that's semantically mutating but bitwise
     * non-mutating occurs.
     */
    void updateVersion();

    /* Index check. */
    void checkIndex(int index, const char* why, int low, int high) const;

public:
    /* Iterator support. */
    using iterator = stanfordcpplib::collections::CheckedIterator<std::string::iterator>;
    using const_iterator = stanfordcpplib::collections::CheckedIterator<std::string::const_iterator>;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    /* Streams support. */
    friend std::ostream& operator<< (std::ostream& out, const String& toWrite);
    friend std::istream& operator>> (std::istream& in,  String& toRead);
    friend std::istream& getline(std::istream& in, String& toRead);

    /* Hashing. */
    friend int hashCode(const String& str);

    /* Data export. */
    std::string asStdString() const;
    const char* c_str() const;

    /* Interoperability with std::string. */
    operator const std::string&() const;
    operator std::string&();

    /* Relational operators */
    bool operator== (StringView text) const;
    bool operator!= (StringView text) const;
    bool operator<  (StringView text) const;
    bool operator>  (StringView text) const;
    bool operator<= (StringView text) const;
    bool operator>= (StringView text) const;
};

/* Utilities. We need these replacements for <cctype> because they take in and return
 * integers, which do not play well with the String type.
 */
char toUpper(char ch);
char toLower(char ch);
bool isAlpha(char ch);
bool isDigit(char ch);
bool isSpace(char ch);
bool isAlnum(char ch);
bool isPrint(char ch);
bool isPunct(char ch);

/* * * * * Implementation Below This Point * * * * */
namespace stanfordcpplib {
    namespace string {
        /* We have different code paths for conversion operators that are used when working with
         * types that are natively text versus types that are not natively text. These utility
         * types selectively enable or disable them.
         */
        template <typename T, typename U> using NonTextConvertResult = std::enable_if<!IsText<T>::value, U>;
        template <typename T, typename U> using TextConvertResult    = std::enable_if<IsText<T>::value, U>;

        /* Checks if something is String-y but not a string. */
        template <typename T, typename U> using StringConvertibleResult = std::enable_if<IsText<T>::value && !IsString<T>::value, U>;

        /* Global relational operators (ones with an RHS of String) can only be used if th LHS is text-y,
         * but specifically not a String. If it's a String, we use the member function versions of the
         * relational overloads.
         */
        template <typename T> using ComparisonResult = StringConvertibleResult<T, bool>;

        /* Global + operator can only be used if the LHS is text-y but not a String. If it's a String,
         * we can use the member function version.
         */
        template <typename T> using ConcatResult = StringConvertibleResult<T, String>;

        /* Detect whether something can be read to or written from a stream. */
        template <typename T> struct StreamTraits {
        private:
            /* Two types of guaranteed different sizes, used to detect the result. */
            typedef char Yes;
            struct No { char buffer[137]; };

            /* We can write something if the expression
             *
             *    cout << some-object-of-this-type
             *
             * produces an ostream&. We detect this by stripping off the & to get ostream,
             * then declaring an ostream*, but only in the case where the stream insertion
             * expression is valid.
             */
            template <typename U> static Yes checkWrite(typename std::remove_reference<decltype(std::cout << std::declval<U>())>::type* = 0);
            template <typename U> static No checkWrite(...);

            /* Same trick, but for istream. Note that we use declval<U&> to force the RHS to be a reference type. */
            template <typename U> static Yes checkRead(typename std::remove_reference<decltype(std::cin >> std::declval<U&>())>::type* = 0);
            template <typename U> static No checkRead(...);

        public:
            static const bool isWritable = (sizeof(checkWrite<T>(nullptr)) == sizeof(Yes));
            static const bool isReadable = (sizeof(checkRead<T>(nullptr)) == sizeof(Yes));
        };

        /* Checks to see if something is a signed / unsigned integral type. */
        template <typename T> using SignedIntegralResult    = std::enable_if<std::is_integral<T>::value && !std::is_unsigned<T>::value, T>;
        template <typename T> using UnsignedIntegralResult  = std::enable_if<std::is_integral<T>::value &&  std::is_unsigned<T>::value, T>;
    }
}

#include <utility>
template <typename Text, typename> String::String(Text&& text) : String(StringView(std::forward<Text>(text))) {
    // Handled in initialization list
}

/* Relational operators where the String is on the right. These forward to the operators
 * where the String is on the left.
 */
template <typename Text>
typename stanfordcpplib::string::ComparisonResult<Text>::type operator== (const Text& lhs, const String& rhs) {
    return rhs == lhs;
}

template <typename Text>
typename stanfordcpplib::string::ComparisonResult<Text>::type operator!= (const Text& lhs, const String& rhs) {
    return rhs != lhs;
}

template <typename Text>
typename stanfordcpplib::string::ComparisonResult<Text>::type operator<  (const Text& lhs, const String& rhs) {
    return rhs > lhs;
}

template <typename Text>
typename stanfordcpplib::string::ComparisonResult<Text>::type operator<= (const Text& lhs, const String& rhs) {
    return rhs >= lhs;
}

template <typename Text>
typename stanfordcpplib::string::ComparisonResult<Text>::type operator>= (const Text& lhs, const String& rhs) {
    return rhs <= lhs;
}

template <typename Text>
typename stanfordcpplib::string::ComparisonResult<Text>::type operator> (const Text& lhs, const String& rhs) {
    return rhs > lhs;
}

/* Concatenation when the string is on the right. */
template <typename Text>
typename stanfordcpplib::string::ConcatResult<Text>::type operator+ (const Text& lhs, const String& rhs) {
    return String(lhs) + rhs;
}


#include <sstream>

namespace stanfordcpplib {
    namespace string {
        /* Specific implementations of String::from for types where routing through a stream
         * doesn't give the desired behavior.
         */

        /* Booleans map to "true" and "false." */
        inline String fromImpl(bool value) {
            return value? "true" : "false";
        }

        /* Any text type is directly converted to a String through the constructor. */
        template <typename T>
        inline typename TextConvertResult<T, String>::type fromImpl(const T& data) {
            return String(data);
        }

        /* All non-text types are routed through a stringstream. */
        template <typename T>
        inline typename NonTextConvertResult<T, String>::type fromImpl(const T& data) {
            /* Hello CS106er! If you're reading this message, it means you tried to call
             * String::from() on something that we don't know how to convert to a string.
             *
             * We use the same << operator that you're familiar with from cout to convert
             * values to strings. If you try converting a type that doesn't have a <<
             * operator defined, we can't convert your type to a string.
             *
             * To fix this, define your own << operator. Check the textbook section on
             * overloading operator<< for information about how to do this.
             */
            static_assert(StreamTraits<T>::isWritable, "Oops! You have tried converting something to a string that we don't know how to convert. Click this error message for details.");

            std::ostringstream builder;
            builder << data;
            return String(builder.str());
        }

        /* Implementation of String::to. We use tag dispatching to identify specific
         * functions for specific types that we care about and use SFINAE to broadly
         * classify things as either "route through a stream" or "don't."
         */

        /* For tag dispatching. */
        template <typename T> struct Box {};

        /* Booleans must be "true" or "false". */
        inline bool toImpl(const std::string& data, Box<bool>) {
            if (data == "true") return true;
            if (data == "false") return false;
            error("String::to: Boolean values must be either 'true' or 'false'");
        }

        /* Characters are extracted after confirming the length is right. */
        inline char toImpl(const std::string& data, Box<char>) {
            if (data.size() != 1) {
                error("String::to: String must have length one to be converted to a char.");
            }
            return data[0];
        }

        /* Overloads for signed and unsigned characters. */
        inline signed char toImpl(const std::string& data, Box<signed char>) {
            return static_cast<signed char>(toImpl(data, Box<char>()));
        }

        inline unsigned char toImpl(const std::string& data, Box<unsigned char>) {
            return static_cast<unsigned char>(toImpl(data, Box<char>()));
        }

        /* Generic routing function for non-text versions of String::to. We route through
         * a stringstream.
         *
         * TODO: This allows for leading and trailing whitespace - do we want that? When I
         * tried using std::noskipws here, that caused issues with our container types.
         * (I suppose we should fix that?)
         */
        template <typename T>
        inline typename NonTextConvertResult<T, T>::type toImpl(const std::string& data, Box<T>) {
            /* Hello CS106er! If you're reading this message, it means you tried to call
             * String::to<T>() on a type T that we don't know how to convert to.
             *
             * We use the >> operator to perform this conversion. To convert to your type
             * T, you'll need to implement operator >> for your custom type. Check the
             * textbook for information about how to do this.
             */
            static_assert(stanfordcpplib::string::StreamTraits<T>::isReadable, "Oops! You tried converting a string to a type we don't know how to convert to. Click this error message for details.");

            std::istringstream extractor(data);

            T result;
            extractor >> result;

            if (!extractor) {
                error("String::to: Could not convert string to that type.");
            }

            char leftover;
            if (extractor >> leftover) {
                error("String::to: Could not convert string to that type.");
            }

            return result;
        }

        /* const char* just gets the underlying C string. */
        inline const char* toImpl(const std::string& data, Box<const char *>) {
            return data.c_str();
        }

        /* Conversion to a std::string is straightforward. */
        inline std::string toImpl(const std::string& data, Box<std::string>) {
            return data;
        }

        /* Not sure why you'd ever do this, but I guess someone might! */
        inline String toImpl(const std::string& data, Box<String>) {
            return data;
        }

        /* Conversion to signed/unsigned integer. We factor out the logic to catch
         * the result and bounds-check everything.
         */
        template <typename T, typename Intermediate> T radixConvert(std::function<Intermediate()> converter) {
            try {
                /* Run the conversion, which may throw if the value is invalid. */
                Intermediate result = converter();

                /* Confirm that it's in range. */
                if (result >= static_cast<Intermediate>(std::numeric_limits<T>::min()) &&
                    result <= static_cast<Intermediate>(std::numeric_limits<T>::max())) {
                    return static_cast<T>(result);
                } else {
                    error("String::to: Could not convert string to that type.");
                }
            } catch (const std::exception& e) {
                error("String::to: Could not convert string to that type.");
            }
        }


        template <typename T>
        typename SignedIntegralResult<T>::type radixTo(const std::string& str, int radix) {
            return radixConvert<T, long long>([&] {
                return std::stoll(str, nullptr, radix);
            });
        }

        template <typename T>
        typename UnsignedIntegralResult<T>::type radixTo(const std::string& str, int radix) {
            return radixConvert<T, unsigned long long>([&] {
                /* Confirm that we don't begin with a minus sign. */
                if (str.empty() || str[0] == '-') {
                    error("Unsigned values can't be negative.");
                }

                return std::stoull(str, nullptr, radix);
            });
        }
    }
}

/* Forward to the appropriate helper. */
template <typename T> String String::from(const T& value) {
    return stanfordcpplib::string::fromImpl(value);
}

/* Text types are converted based on their type. */
template <typename T> T String::to(const String& str) {
    /* Use tag dispatching to send it to the right place. */
    return stanfordcpplib::string::toImpl(str._data, stanfordcpplib::string::Box<T>());
}

/* This overload of to<> only works if the target type is integral. */
template <typename T> T String::to(const String& str, int radix) {
    static_assert(std::is_integral<T>::value, "Oops! You're using the version of String::to that's meant for converting strings to integers, but the type you've provided isn't an integer type.");

    /* Permitted range of radix values. */
    if (radix < 2 || radix > 36) {
        error("String::to: Radix must be between 2 and 36, inclusive.");
    }

    return stanfordcpplib::string::radixTo<T>(str.trimmed()._data, radix);
}

/* 'is' is implemented in terms of 'to.' */
template <typename T> bool String::is(const String& str) try {
    return void(String::to<T>(str)), true;
} catch (const ErrorException &) {
    return false;
}

template <typename T> bool String::is(const String& str, int radix) {
    static_assert(std::is_integral<T>::value, "Oops! You're using the version of String::to that's meant for converting strings to integers, but the type you've provided isn't an integer type.");

    /* Validate the radix. We do this separately from the next step because if
     * the radix is never legal, we don't want to give the impression that the
     * operation failed because the value is out of range.
     */
    if (radix < 2 || radix > 36) {
        error("String::is: Radix must be between 2 and 36, inclusive.");
    }

    try {
        return void(String::to<T>(str, radix)), true;
    } catch (const ErrorException &) {
        return false;
    }
}
