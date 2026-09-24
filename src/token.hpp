#pragma once
#include <string>
#include <cmath>

enum class TokenType
{
    IDENT,
    INT_KW,
    FLOAT_KW,
    STRING_KW,
    BOOL_KW,
    make_KW,
    fn_KW,
    RET_KW,
    if_KW,
    else_KW,
    while_KW,
    main_KW,
    and_KW,
    or_KW,
    not_KW,
    true_KW,
    false_KW,
    int_LIT,
    float_LIT,
    string_LIT,
    PLUS_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,
    EQ_OP,
    EQ_COMP,
    NOT_COMP,
    GT_COMP,
    LT_COMP,
    GTEQ_COMP,
    LTEQ_COMP,
    PARAN_OPEN,
    PARAN_CLOSE,
    BRACE_OPEN,
    BRACE_CLOSE,
    SEMI,
    COLUMN,
    COMMA,
    END
};

struct Token
{
    TokenType type = TokenType::END;
    std::string text = "";
    long long int_val = 0;
    double float_val = NAN;
};