#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include "tokenization.hpp"
#include "token.hpp"

std::string tokens_to_asm(const std::vector<Token>& tokens){
    std::stringstream output;
    output << "global _start\n _start:\n";
    for(int i=0 ; i <tokens.size(); i++){
        const Token& token = tokens[i];
        if(token.type == TokenType::exit){
           if(i+1 < tokens.size() && tokens[i+1].type == TokenType::int_lit){
               if(i+2 < tokens.size() && tokens[i+2].type == TokenType::semi){
                output << "    mov rax, 60\n";
                output    << "    mov rdi, " << tokens[i+1].value.value() << "\n";
                output   << "    syscall\n";
               }
            }
    }
}
return output.str();
}
 
// Function to tokenize the input string
int main( int argc, char* argv[] ) {
    //file will be argv[1], if we dont have two args we will return an error
    if ( argc != 2 ) {
        std::cerr << "Incorrect usage. Correct usage is emerald <input.em>" << std::endl;
        return EXIT_FAILURE;
    }
    // Read the file contents into a string
    std::string contents;
    {
    std::string inputFile = argv[1];
    if(inputFile.substr(inputFile.find_last_of(".") + 1) != "em") {
        std::cerr << "Error: Input file must have .em extension." << std::endl;
        return EXIT_FAILURE;
    }
    std::fstream file(inputFile, std::ios::in);
    std::stringstream cont;
    cont << file.rdbuf();
    contents = cont.str();
    }
    Tokenizer tokenizer(std::move(contents)); 
    // Tokenize the contents of the file
    std::vector<Token> tokens = tokenizer.tokenize();
    {
        std::fstream file("../out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }
    return EXIT_SUCCESS;
}