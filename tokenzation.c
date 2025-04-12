/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenzation.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 15:24:04 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/04/12 12:33:53 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_lexer	*initialize_lexer(char *input)
{
	t_lexer	*lexer;

	lexer = malloc((sizeof(t_lexer)));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->lenght = ft_strlen(input);
	lexer->position = 0;
	return (lexer);
}

int	is_space(t_lexer *lexer)
{
	if ((lexer->input[lexer->position] >= 9 
			&& lexer->input[lexer->position] <= 13)
		|| lexer->input[lexer->position] == 32)
		return (1);
	return (0);
}

void	skip_whitespace(t_lexer *lexer)
{
	while (lexer->position < lexer->lenght && is_space(lexer))
		lexer->position++;
}

t_token	*create_token(char *value, char quote)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	else
	{
		token->value = ft_strdup(value);
		token->next = NULL;
	}
	if (quote == '\'')
		token->type = 3;
	else if (quote == '"')
		token->type = 4;
	return (token);
}

// t_token	*quote_token(t_lexer *lexer, char quote)
// {
// 	int i;

// 	i = lexer->position + 1;
// 	while (i < lexer->lenght)
// 	{
// 		if (lexer->input[i] == '\'' || lexer->input[i] == '"')
// 		{
// 			lexer->quote += 1;
// 			break ;
// 		}
// 		else
// 			i++;
// 	}
// 	lexer->position += 1;
// 	return(create_token(ft_substr(&quote, 0 , 1)));
// }

t_token	*handle_quote(t_lexer *lexer, char quote)
{
	size_t	lenght;
	size_t	start;
	char	*value;

	lexer->position += 1;
	start = lexer->position;
	while (lexer->position < lexer->lenght 
		&& lexer->input[lexer->position] != quote)
		lexer->position++;
	if (lexer->position >= lexer->lenght)
	{
		perror("unclosed quote\n");
		return (NULL);
	}
	lenght = lexer->position - start;
	value = ft_substr(lexer->input, start, lenght);
	lexer->position += 1;
	return (create_token(value, quote));
}

t_token	*handle_operations(t_lexer *lexer, char *oper, int i)
{
	char	*str;

	str = ft_substr(oper, 0, i);
	if (!str)
		return (NULL);
	str[i] = '\0';
	lexer->position += i;
	return (create_token(str, str[0]));
}

t_token	*handle_word(t_lexer *lexer)
{
	char	*word;
	size_t	start;
	size_t	lenght;

	start = lexer->position;
	// printf("lexer->position = %d			lexer->lenght = %d\n", lexer->position, lexer->lenght);
	while (lexer->position < lexer->lenght && (!is_space(lexer)) 
		&& lexer->input[lexer->position] != '\'' 
		&& lexer->input[lexer->position] != '"' 
		&& lexer->input[lexer->position] != '|' 
		&& lexer->input[lexer->position] != '<' 
		&& lexer->input[lexer->position] != '>' 
		&& lexer->input[lexer->position] != '(' 
		&& lexer->input[lexer->position] != ')'
		&& lexer->input[lexer->position] != '&')
		lexer->position++;
	lenght = lexer->position - start;
	word = ft_substr(lexer->input, start, lenght);
	return (create_token(word, word[0]));
}

t_token	*get_next_token(t_lexer *lexer)
{
	char	*current;

	skip_whitespace(lexer);
	if (lexer->position >= lexer->lenght)
		return (0);
	current = lexer->input + lexer->position;
	if (current[0] == '\'' || current[0] == '"')
		return (handle_quote(lexer, *current));
	if ((lexer->input[lexer->position] == '|' 
			&& lexer->input[lexer->position + 1] == '|') 
		|| (lexer->input[lexer->position] == '&' 
			&& lexer->input[lexer->position + 1] == '&'))
		return (handle_operations(lexer, current, 2));
	if ((lexer->input[lexer->position] == '>' 
			&& lexer->input[lexer->position + 1] == '>') 
		|| (lexer->input[lexer->position] == '<' 
			&& lexer->input[lexer->position + 1] == '<'))
		return (handle_operations(lexer, current, 2));
	if (current[0] == '|' || current[0] == '<' || current[0] == '>' 
		|| current[0] == '(' || current[0] == ')' || current[0] == '&')
		return (handle_operations(lexer, current, 1));
	return (handle_word(lexer));
}

int	cmd_type(t_token *token, int first_cmd)
{
	if (first_cmd == 0)
	{
		first_cmd++;
		return (1);
	}
	else if (token->type == PIPE || token->type == OPEN_PAREN 
		|| token->type == DOUBLE_QUOTE || token->type == OR 
		|| token->type == AND)
		return (1);
	else
		return (0);
}

t_type	token_type(t_token *token)
{
	// printf("token->type = %u\n", token->type);
	if (token->type == 3)
		return (SINGLE_QUOTE);
	else if (token->type == 4)
		return (DOUBLE_QUOTE);
	else if (strcmp(token->value, "||") == 0)
		return (OR);
	else if (strcmp(token->value, "&&") == 0)
		return (AND);
	else if (strcmp(token->value, "|") == 0)
		return (PIPE);
	else if (strcmp(token->value, ">>") == 0)
		return (APPEND);
	else if (strcmp(token->value, "<<") == 0)
		return (HEREDOC);
	else if (strcmp(token->value, "<") == 0)
		return (REDIR_IN);
	else if (strcmp(token->value, ">") == 0)
		return (REDIR_OUT);
	else if (strcmp(token->value, "(") == 0)
		return (OPEN_PAREN);
	else if (strcmp(token->value, ")") == 0)
		return (CLOSE_PAREN);
	else
		return (WORD);
}

t_precedence precedence_type(t_token *token)
{
	if (token->type == 3 || token->type == 4)
		return (QUOTES);
	else if (token->type == 9 || token->type == 10)
		return (PAREN);
	else if (token->type == 5 || token->type == 6 || token->type == 7 || token->type == 8)
		return (REDIR);
	else if (token->type == 2)
		return (PIPES);
	else if (token->type == 11 || token->type == 12)
		return (OPER);
	return (WORDS);
}

void enqueue(t_queue *queue, t_token *token)
{
    token->next = NULL;
    if (queue->front == NULL)
    {
        queue->front = token;
        queue->back = token;
    }
    else
    {
        queue->back->next = token;
        queue->back = token;
    }
}

// int	main(int argc, char **argv)
// {
// 	char	*input;
// 	t_lexer	*lexer;
// 	t_token	*token;

// 	(void)argc;
// 	(void)argv;
// 	while (1)
// 	{
// 		input = readline("minishell> ");
// 		lexer = initialize_lexer(input);
// 		while (lexer->position < lexer->lenght)
// 		{
// 			token = get_next_token(lexer);
// 			if (!token)
// 				continue;
// 			token->type = token_type(token);
// 			printf("token->value = %s			token->type = %d\n", token->value, token->type);
// 			// token->prece = precedence_type(token);
// 		}
// 	}
// 	return (0);
// }
