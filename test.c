

void skip_whitespace(t_lexer *lexer) {
    while (lexer->position < lexer->length && isspace(lexer->input[lexer->position])) {
        lexer->position++;
    }
}
