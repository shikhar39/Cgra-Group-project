#include "ComputeShader.hpp"
#include <opengl.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

ComputeShader::ComputeShader(const std::string &computePath)
{

    std::string computeCode;
    std::ifstream cShaderFile;

    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        cShaderFile.open(computePath);
        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();
        cShaderFile.close();
        computeCode = cShaderStream.str();

    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();
    unsigned int compute;
    // compute shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    // checkCompileErrors(compute, "COMPUTE");
    int success;
    char infoLog[512];
    // print compile errors if any
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::COMPUTE_SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(compute);
}

void ComputeShader::use() {
    glUseProgram(ID);
}
