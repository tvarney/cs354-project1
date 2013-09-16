
#ifndef CS354_GENERIC_PARSER_HPP
#define CS354_GENERIC_PARSER_HPP

#include <cstdio>
#include <string>

namespace cs354 {
    class Token {
    public:
        Token();
        Token(std::string &image, bool good = true);
        
        bool good;
        std::string image;
        
        Token & operator=(const Token &rhs);
        bool operator==(const Token &rhs) const;
        bool operator==(const char *rhs) const;
        bool operator==(const std::string &rhs) const;
    };
    
    class Parser {
    public:
        Parser(const char *fname, size_t buff_size = 1024);
        Parser(FILE *fp, size_t buff_size = 1024, const char *fname = NULL);
        ~Parser();
        
        bool done() const;
        /* Returns the next token.
         * If the parser is finished, the returned token will have a size of
         * zero and (str == NULL).
         */
        const Token & next();
    private:
        const char *fname;
        FILE *fp;
        bool _done;
        size_t pos, nread, buffer_size;
        char *buffer;
        Token token;
    };
}

#endif
