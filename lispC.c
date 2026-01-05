#include <stdlib.h>
#include <stdio.h>

typedef enum {
    tok_none,
    tok_plus,
    tok_minus,
    tok_mul,
    tok_div,
    tok_num,
    tok_bool,
    tok_var,
}token_type;

typedef enum{
    keyword_none,
    keyword_true,
    keyword_false,
}
keyword_type;

typedef enum{
    expr_none,
    expr_unary,
    expr_binary,
    expr_value,
}
expr_type;

typedef struct token{
    token_type type;
    int offset;
    union {
        int integer;
        char character;
        int varStorageOffset;
    }data;
}token;

typedef struct expr expr;

typedef struct binary_expr{
    token t;
    expr* a;
    expr* b;
}binary_expr;

typedef struct value_expr{
    token t;
}value_expr;


typedef struct expr{
    expr_type type;
    union {
        binary_expr binary;
        value_expr value;
    }data;
}expr;

static char input[2048];
expr exprs[2048];
token tokens[2048];
char varStorage[2048];
int varCount = 0;
int exprCount = 0;
int tokenCount = 0;
int parseError = 0;
int processingError = 0;

void printTokens(){
    for(int i = 0; i < tokenCount; ++i){
        token t = tokens[i];
        switch(t.type){
            case tok_none:{ printf("token: %3d | tok_none   \n", i);}break;
            case tok_plus:{ printf("token: %3d | tok_plus   \n", i);}break;
            case tok_minus:{printf("token: %3d | tok_minus  \n", i);}break;
            case tok_mul:{  printf("token: %3d | tok_mul    \n", i);}break;
            case tok_div:{  printf("token: %3d | tok_div    \n", i);}break;
            case tok_num:{  printf("token: %3d | tok_num    | %d\n", i, t.data.integer);}break;
            case tok_bool:{ printf("token: %3d | tok_bool   \n", i);}break;
            case tok_var:{  printf("token: %3d | tok_var    | %s\n", i, varStorage + t.data.varStorageOffset);}break;
            default:{}break;
        }
    }
}

void runExprs(){
    for(int i = 0; i < exprCount; ++i){
        if(exprs[i].type == expr_binary){
            printf("BINARY EXPRESSION\n");

        }
    }

}

expr* processExpr(token t, int* i){
    switch(t.type){
        case tok_num:{
            (*i)++;
            expr newe = {};
            newe.type = expr_value;
            newe.data.value.t = t;
            exprs[exprCount++] = newe;
            printf("VALUE EXPRESSION!\n");
            return exprs + (exprCount-1);
        }break;
        case tok_plus:{
            (*i)++;
            expr newe = {};
            newe.type = expr_binary;
            newe.data.binary.t = t;
            if(exprCount-1 >= 0)newe.data.binary.a = exprs + (exprCount-1);
            else processingError++;
            token nextT = tokens[(*i)++];
            newe.data.binary.b = processExpr(nextT, i);
            exprs[exprCount++] = newe;
            printf("BINARY EXPRESSION!\n");
            return exprs + (exprCount-1);


        };
        default:{}break;
    }
    return NULL;
}

void executeExprs(){
    int i = 0;
    while(i < exprCount){
        expr e = exprs[i++];
        switch(e.type){
            case expr_binary:{
                int a = e.data.binary.a->data.value.t.data.integer;
                int b = e.data.binary.b->data.value.t.data.integer;
                printf("%d + %d = %d\n", a, b, a+b);
            }break;
            default:{}break;
        }
    }
}

void processTokens(){
    int i = 0;
    token t = tokens[i];
    while(i < tokenCount){
        processExpr(t, &i);
        t = tokens[i];
    }
}

int isNum(char c){
    if(c >= '0' && c <= '9')return 1;
    return 0;
}

int isAlpha(char c){
    if(c >= 'A' && c <= 'Z')return 1;
    else if(c >= 'a' && c <= 'z')return 1;
    return 0;
}

int isWhitespace(char c){
    switch(c){
        case ' ' :{return 1;}break;
        case '\n':{return 1;}break;
        case '\t':{return 1;}break;
        case '\r':{return 1;}break;
        default:{}break;
    }
    return 0;
}

void skipWhitespace(char* str){
    while(isWhitespace(*str)){
        str++;
    }
}

int isUpper(char c){
    if(c >= 'A' && c <= 'Z')return 1;
    return 0;
}

char convertToLower(char c){
    int diff = 'a'-'A';
    if(isAlpha(c)){
        if(isUpper(c))return (c+diff);
    }
    return c;
}

int compareKeyword(char* str, const char* compare, int count){
    int match = 1;
    for(int i = 0; i < count; ++i){
        if(convertToLower(str[i]) != compare[i]){match = 0; break;}
    }
    return match;
    
}

void addToken(token t){
    tokens[tokenCount++] = t;
} 

keyword_type isKeyword(char* str){
    char c0 = convertToLower(*str);
    keyword_type type = keyword_none;
    str++;
    int match = 1;
    switch(c0){
        case 't':{
            compareKeyword(str, "rue", 3);
            type = keyword_true;
        }break;
        case 'f':{
            compareKeyword(str, "alse", 4);
            type = keyword_false;
        }break;
        default:{
            match = 0;
        }break;
    }
    return type;

}

void parse(char* input){
    int i = 0;
    char c = input[i];
    while(c != '\0'){
        if(isNum(c)){
            int val = 0;
            while(isNum(input[i])){
                val *= 10;
                val += (input[i] - '0');
                i++;
                    // printf("ALPHA char %d | %c \n",i, convertToLower(c));
                    // c = input[i];
            }
            printf("NUM TOKEN, VALUE: %d\n",val);
            token t = {};
            t.type = tok_num;
            t.data.integer = val;
            addToken(t);
            if(isAlpha(c)){printf("CHARS CANNOT IMMEDIATELY FOLLOW NUMBER! ERROR!\n"); parseError++;}
        }
        else if(isAlpha(c)){
            keyword_type keywordType = isKeyword(input + i);
            if(keywordType){
                token t = {};
                switch(keywordType){
                    case keyword_true:{
                        i+=4;
                        printf("KEYWORD: TRUE\n");
                        t.data.integer = 1;
                        t.type = tok_bool;
                    }break;
                    case keyword_false:{
                        i+=5;
                        printf("KEYWORD: FALSE\n");
                        t.data.integer = 0;
                        t.type = tok_bool;

                    }break;
                    default:{}break;                    
                }
                addToken(t);
            }else{
                //if its not a keyword, lets store it as a token
                token t = {};
                t.offset = i;
                t.type = tok_var;
                t.data.varStorageOffset = varCount;
                addToken(t);
                while(isAlpha(input[i] || isNum(input[i]))){
                    varStorage[varCount++] = input[i];
                    printf("ALPHA char %d | %c \n",i, convertToLower(c));
                    i++;
                    c = input[i];
                }
                varStorage[varCount++] = '\0';

            }
        }else if(!isWhitespace(c)){
            switch(c){
                case '+':{
                    token t = {};
                    t.offset = i;
                    t.type = tok_plus;
                    addToken(t);
                    printf("+ TOKEN AT %d\n", i);
                }break;
                case '-':{
                    token t = {};
                    t.offset = i;
                    t.type = tok_minus;
                    addToken(t);
                    printf("- TOKEN AT %d\n", i);
                }break;
                case '*':{
                    token t = {};
                    t.offset = i;
                    t.type = tok_mul;
                    addToken(t);
                    printf("* TOKEN AT %d\n", i);
                }break;
                case '/':{
                    token t = {};
                    t.offset = i;
                    t.type = tok_div;
                    addToken(t);
                    printf("/ TOKEN AT %d\n", i);
                }break;
                default:{
                    printf("unhandled char: %c\n", c);
                }break;
            }
            i++;
        }else if(isWhitespace(c)){
            if(c == '\n')printf("WHITESPACE char %d | \\n\n",i);
            if(c == '\t')printf("WHITESPACE char %d | \\t\n",i);
            if(c == '\r')printf("WHITESPACE char %d | \\r\n",i);
            if(c == ' ' )printf("WHITESPACE char %d | \' \' |\n",i);
            i++;
        }


        if(parseError)return;
        c = input[i];


    }
}


int main(){
    printf("test\n");

    //print version and exit information
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while(1){
        tokenCount = 0;
        parseError = 0;
        //outpt our prompt
        fputs("lispy> ", stdout);

        //read a line of user inoput of maximum size 2048
        fgets(input, 2048, stdin);

        //echo input back to user
        printf("No youre a %s", input);
        parse(input);
        if(parseError)continue;

        printTokens();
        processTokens();
        if(processingError)continue;
        executeExprs();

    }


    return 0;
}