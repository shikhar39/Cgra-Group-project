#pragma once
#include <string>
class ComputeShader
{
    public:
        unsigned int ID;

        ComputeShader() {};
        ComputeShader(const std::string &computePath);
        void use();

};