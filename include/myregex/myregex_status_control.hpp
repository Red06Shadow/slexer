#ifndef MYREGEXCONTROL
#define MYREGEXCONTROL

#include <iostream>

namespace myregex
{
    struct basic_control
    {
        bool questbeginline : 1;
        bool questendline : 1;
        bool isoptionallookahead : 1;
        unsigned char lookahead : 3;
        basic_control() { *(unsigned char*)(this) = 0;}
        friend std::ostream& operator<<(std::ostream& out, const basic_control value) {
            out << "^: " << std::boolalpha << value.questbeginline << std::endl
                << "$: " << std::boolalpha << value.questendline << std::endl
                << "lookahead optional: " << std::boolalpha << value.isoptionallookahead << std::endl
                << "lookahead type: " << value.lookahead;
            return out;
        }
        friend std::wostream& operator<<(std::wostream& out, const basic_control value) {
            out << L"^: " << std::boolalpha << value.questbeginline << std::endl
                << L"$: " << std::boolalpha << value.questendline << std::endl
                << L"lookahead optional: " << std::boolalpha << value.isoptionallookahead << std::endl
                << L"lookahead type: " << value.lookahead;
            return out;
        }
    };
    
} // namespace myregex


#endif