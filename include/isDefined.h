#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <memory>

void isTrue(bool condition, const std::string& message)
{
    if (!condition)
    {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::terminate();
    }
}

template<typename T>
void isDefined(const std::unique_ptr<T>& condition, const std::string& message)
{
    isTrue(condition != nullptr, message);
}
