
#include "generic/Parser.hpp"

#include "generic/String.hpp"
#include <cstring>
#include <stdexcept>

using namespace cs354;

Token::Token() :
    good(false), image("")
{ }
Token::Token(std::string &image, bool good) :
    good(good), image(image)
{ }

Token & Token::operator=(const Token &rhs) {
    good = rhs.good;
    image = rhs.image;
    return (*this);
}

bool Token::operator==(const Token &rhs) const {
    if(!good && !(rhs.good)) {
        return true;
    }
    
    return rhs.image == image;
}
bool Token::operator==(const char *rhs) const {
    return good && (std::strcmp(rhs, image.data()) == 0);
}
bool Token::operator==(const std::string &rhs) const {
    return good && (image == rhs);
}

Parser::Parser(const char *fname, size_t buff_size) :
    fname(fname), _done(false), pos(0), nread(0),
    buffer_size(buff_size > 0 ? buff_size : 1024)
{
    fp = std::fopen(fname, "r");
    if(!fp) {
        throw std::runtime_error(std::string("Could not open file"));
    }
    
    buffer = new char[buffer_size];
    
    nread = std::fread(buffer, 1, buffer_size, fp);
    _done = (nread == 0);
}
Parser::Parser(FILE *fp, size_t buff_size, const char *fname) :
    fname(fname), fp(fp), pos(0), nread(buff_size > 0 ? buff_size : 1024)
{
    if(!fp) {
        throw std::runtime_error(std::string("Invalid FILE handle"));
    }
    buffer = new char[buffer_size];
    nread = std::fread(buffer, 1, buffer_size, fp);
    _done = (nread == 0);
}
Parser::~Parser() {
    if(fp) {
        fclose(fp);
        fp = NULL;
    }
    if(buffer) {
        delete buffer;
    }
}

bool Parser::done() const {
    return _done;
}

enum ParserState {
    PARSER_READ,
    PARSER_SKIP_SPACE,
    PARSER_SKIP_COMMENT,
    PARSER_DECIDE
};
static Token EOF_Token;
const Token & Parser::next() {
    ParserState state;
    if(_done) {
        return EOF_Token;
    }
    
    if(std::isspace(buffer[pos])) {
        state = PARSER_SKIP_SPACE;
    }else if(buffer[pos] == '#') {
        state = PARSER_SKIP_COMMENT;
    }else {
        state = PARSER_READ;
    }
    size_t start;
    token.good = false;
    token.image = "";
    
    do {
        while(pos < nread) {
            switch(state) {
            case PARSER_READ:
                start = pos;
                while(pos < nread) {
                    if(buffer[pos] == '#' || std::isspace(buffer[pos])) {
                        token.image.append(buffer + start, pos - start);
                        token.good = true;
                        return token;
                    }
                }
                /* If we got here, the token reading terminated the loop, so
                 * we need to save the current data and read more.
                 */
                token.image.append(buffer + start, pos - start);
                break;
            case PARSER_SKIP_SPACE:
                while(pos < nread) {
                    if(!(std::isspace(buffer[pos]))) {
                        if(buffer[pos] == '#') {
                            state = PARSER_SKIP_COMMENT;
                            break;
                        }
                        state = PARSER_READ;
                        break;
                    }
                    pos += 1;
                }
                break;
            case PARSER_SKIP_COMMENT:
                while(pos < nread) {
                    if(buffer[pos] == '\n') {
                        pos += 1;
                        state = PARSER_DECIDE;
                        break;
                    }
                    pos += 1;
                }
                break;
            case PARSER_DECIDE:
                if(std::isspace(buffer[pos])) {
                    state = PARSER_SKIP_SPACE;
                }else if(buffer[pos] == '#') {
                    state = PARSER_SKIP_COMMENT;
                }else {
                    state = PARSER_READ;
                }
                break;
            }
        }
        
        // Update buffer
        nread = fread(buffer, 1, buffer_size, fp);
        pos = 0;
    }while(nread == buffer_size);
    _done = true;
    if(token.image.size() > 0) {
        /* File ends with a token, return the token */
        return token;
    }
    
    return EOF_Token;
}
