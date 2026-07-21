
# Simple-Lexer (slexer)
Simple-Lexer `slexer` is a small repository that contains the basic and necessary resources for handling files and strings using regular expressions and basic user-defined functions. The idea was to do something similar to flex but mostly with C++.
It should be noted that there are still things to improve, mainly the construction of rules from regular expressions. In certain circumstances, the construction time increases considerably when using the wchar_t character type, although it mainly depends on the regex engine `myregex`, also created by me as part of another repository, and some unexpected errors, although I've already considered a compiled alternative that will be uploaded soon to simplify things and avoid creating regex structures at runtime. It doesn't require installation, just using `CMake` and adding the files to the `include` directory of your project (at least until I improve using `CMake`).


## Usage/Examples

```c++
// C++ code
#include <iostream>
#include <slexer/slexer.hpp>

// Here the character type to handle and the identifier for each rule are specified; it can be an unsigned integer or an enum class
slexer::basic_lexer<char, size_t> _lexer = slexer::basic_builder<char, size_t>({
{1, "[a-z]+", slexer::basic_lexer<char, size_t>::defaultf},
/* The defaultf function is the basic way to handle the rules,
here the user can write their own function represented by:
void function(basic_lexer<charT, idT>::master &);*/
{-1ULL, "[ \t\n\v\r\f]+", nullptr} // When passing null through the function This causes the capture to be ignored, useful for whitespace.
})
.build(); // and use .build() to return the already built lexer
// Other functions will be explained later; for now, this is the basics.
int main()
{

    for (auto &&i : _lexer.tokenize("Slexer tokenizing a string"))
        std::cout << i.text() << std::endl;

    std::ifstream in{"file path"};

    for (auto &&i : _lexer.tokenize(in)) // Slexer tokenizing a file directly
        std::cout << i.text() << std::endl;
    return 0;
}
```

