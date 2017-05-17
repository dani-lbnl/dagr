#include <iostream>
#include <sstream>
#include <cstring>

#include "dagr_common.h"
#include "dagr_type_select.h"

const char *expected_output = {
    "double"
    ",double"
    ",char"
    ",char"
    ",double"
    ",double"
    ",unsigned long long"
    ",unsigned long long"
    ",double"
    ",double"
    ",float"
    ",float"
    ",long"
    ",long"
    ",unsigned int"
    ",unsigned int"
    };

int main()
{
    std::ostringstream oss;
    oss << dagr_type_select::elevate<double,char>::type_name() << ","
        << dagr_type_select::elevate<char,double>::type_name() << ","
        << dagr_type_select::decay<double,char>::type_name() << ","
        << dagr_type_select::decay<char,double>::type_name() << ","
        << dagr_type_select::elevate<double,unsigned long long>::type_name() << ","
        << dagr_type_select::elevate<unsigned long long,double>::type_name() << ","
        << dagr_type_select::decay<double,unsigned long long>::type_name() << ","
        << dagr_type_select::decay<unsigned long long,double>::type_name() << ","
        << dagr_type_select::elevate<double,float>::type_name() << ","
        << dagr_type_select::elevate<float,double>::type_name() << ","
        << dagr_type_select::decay<double,float>::type_name() << ","
        << dagr_type_select::decay<float,double>::type_name() << ","
        << dagr_type_select::elevate<unsigned long,int>::type_name() << ","
        << dagr_type_select::elevate<int,unsigned long>::type_name() << ","
        << dagr_type_select::decay<unsigned long,int>::type_name() << ","
        << dagr_type_select::decay<int,unsigned long>::type_name();

    if (strcmp(expected_output, oss.str().c_str()))
    {
        DAGR_ERROR("test failed." << std::endl
            << "expected: " << expected_output << std::endl
            << "got     : " << oss.str())
        return -1;
    }

    return 0;
}
