#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include "tokenization.hpp"
// Define an enumeration for token options
enum class TokenType {
    exit, 
    int_lit, 
    semi
};

// Define a structure to represent a token
struct Token {
    TokenType type;
    std::optional<std::string> value;
};

// Function to tokenize the input string
std::vector<Token> tokenize(const std::string&  str){
//vector to store tokens
std::vector<Token> tokens;
    //buffer to store current tokem
std::string buf;
for(int i = 0; i < str.length(); i++){
    char c = str.at(i);
    if(std::isalpha(c)){
        buf.push_back(c);
        i++;
        while(std::isalnum(str.at(i))){
            buf.push_back(str.at(i));
            i++;
        }
        i--;
        if(buf == "exit"){
            tokens.push_back(Token{TokenType::exit, std::nullopt});
            buf.clear();
            continue;
        } else {
            std::cerr << "Error: Unknown identifier '" << buf << "' at position " << i - buf.length() << std::endl;
            exit(EXIT_FAILURE);
        }
} else if(std::isspace(c)){
        continue;
} else if(std::isdigit(c)){
    buf.push_back(c);
    i++;
    while(std::isdigit(str.at(i))){
        buf.push_back(str.at(i));
        i++;
    }
    i--;
    tokens.push_back(Token{TokenType::int_lit, buf});
    buf.clear();
} else if(c==';'){
    tokens.push_back(Token{TokenType::semi, std::nullopt});
} else {
    std::cerr << "Error: Unknown character '" << c << "' at position " << i << std::endl;
    exit(EXIT_FAILURE);
}
}
return tokens;
}

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
    // Tokenize the contents of the file
    std::vector<Token> tokens = tokenize(contents);
    {
        std::fstream file("../out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }
    return EXIT_SUCCESS;
}