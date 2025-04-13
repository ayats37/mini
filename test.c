/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 12:07:43 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/04/13 20:01:57 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// void	print_linked_list(t_token *token_list)
// {
// 	t_token *current = token_list;
// 	while (current)
// 	{
// 		printf("token->value = %s		token->type = %d			token->has_space = %d\n", current->value, current->type, current->has_space);
// 		current = current->next;
// 	}
// }

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

t_token	*create_token(char *value, char quote, int has_space)
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
	token->has_space = has_space;
	// printf("has_space = %d\n", token->has_space);
	return (token);
}

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
	if (lexer->input[lexer->position] == 32)
		return (create_token(value, quote, 1));
	return (create_token(value, quote, 0));
}

t_token	*handle_operations(t_lexer *lexer, char *oper, int i)
{
	char	*str;

	str = ft_substr(oper, 0, i);
	if (!str)
		return (NULL);
	str[i] = '\0';
	lexer->position += i;
	if (lexer->input[lexer->position] == 32)
		return (create_token(str, str[0], 1));
	return (create_token(str, str[0], 0));
}

t_token	*handle_word(t_lexer *lexer)
{
	char	*word;
	size_t	start;
	size_t	lenght;

	start = lexer->position;
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
	if (lexer->input[lexer->position] == 32)
		return (create_token(word, word[0], 1));
	return (create_token(word, word[0], 0));
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

void	append_token(t_token **head, t_token *token)
{
	t_token *tmp;

	if (!*head)
	{
		*head = token;
		return ;
	}
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = token;
}

t_tree *create_tree_node(t_token *token)
{
	t_tree	*node;

	node = malloc(sizeof(t_tree));
	if (!node)
		return (NULL);
	node->type = token->type;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

t_token *get_last_pipe(t_token *token)
{
	t_token *pipe = NULL;
	t_token *tmp;

	tmp = token;
	while(tmp->next)
	{
		if (tmp->type == 2)
			pipe = tmp;
		tmp = tmp->next;
	}
	return (pipe);
}

t_token *get_last_opr(t_token *token)
{
	size_t	paren = 0;
	t_token *opr = NULL;
	t_token *tmp;

	tmp = token;
	while(tmp->next)
	{
		if (tmp->type == 9)
			paren++;
		else if (tmp->type == 10)
			paren--;
		else if ((tmp->type == 11 || tmp->type == 12) && paren == 0)
			opr = tmp;
		tmp = tmp->next;
	}
	return (opr);
}

t_token *sub_left(t_token *token, t_token *opr)
{
	t_token *head = NULL;
	t_token *tmp;
	t_token *new_node;
	t_token *current = token;

	while (current)
	{
		if (current == opr)
			break;
		new_node = malloc(sizeof(t_token));
		if (!new_node)
			return (NULL);
		new_node->value = ft_strdup(current->value);
		new_node->type = current->type;
		new_node->next = NULL;
		if(!head)
			head = new_node;
		else
			tmp->next = new_node;
		tmp = new_node;
		current = current->next;
	}
	return (head);
}

// t_tree	*parse_something()
// {
	
// }

t_tree	*parse_paren(t_token *token)
{
	t_token *current;
	t_token *tmp = token;
	size_t	paren = 0;
	if (tmp->type != 9)
		return NULL;
		// return(parse_something());
	while (tmp)
	{
		if (tmp->type == 9)
			paren++;
		else if (tmp->type == 10)
			paren--;
		if (paren == 0)
		{
			current = tmp;
			break ;
		}
		tmp = tmp->next;
	}
	if (paren != 0)
	{
		printf("Syntax error: unmatched parenthesis\n");
		return NULL;
	}
	t_token *sub_token = sub_left(token->next, current);
	// print_linked_list(sub_token);
	return(parse_op(sub_token));
}

t_tree	*parse_pipes(t_token *token)
{
	t_token *pipe = get_last_pipe(token);
	t_token *left_token = NULL;
	t_token *right_token = NULL;
	t_token *tmp = token;

	if (pipe)
	{
		t_tree *node = create_tree_node(token);
		left_token = sub_left(tmp, pipe);
		// print_linked_list(left_token);
		// printf("\n\n\n");
		right_token = pipe->next;
		// print_linked_list(right_token);
		// printf("\n");
		node->left = parse_pipes(left_token);
		node->right = parse_pipes(right_token);
		return (node);
	}
	return (parse_paren(token));
}

t_tree	*parse_op(t_token *token)
{
	t_token *opr = get_last_opr(token);
	t_token *left_token = NULL;
	t_token *right_token = NULL;
	t_token *tmp = token;

	if (opr)
	{
		t_tree *node = create_tree_node(token);
		left_token = sub_left(tmp, opr);
		// print_linked_list(left_token);
		// printf("\n\n\n");
		right_token = opr->next;
		// print_linked_list(right_token);
		// printf("\n");
		node->left = parse_op(left_token);
		node->right = parse_op(right_token);
		return (node);
	}
	return (parse_pipes(token));
}

// int	main(int argc, char **argv)
// {
// 	char	*input;
// 	t_lexer	*lexer;
// 	t_token	*token;
// 	t_token *token_list = NULL;
// 	// t_tree	*node = NULL;

// 	(void)argc;
// 	(void)argv;
// 	while (1)
// 	{
// 		input = readline("minishell> ");
// 		lexer = initialize_lexer(input);
// 		token_list = NULL;
// 		while (lexer->position < lexer->lenght)
// 		{
// 			token = get_next_token(lexer);
// 			if (!token)
// 				continue;
// 			// if (token->type != 3 && token->type != 4)
// 			token->type = token_type(token);
// 			append_token(&token_list, token);
// 			// node = create_tree_node(token_list);
// 			// printf("token->value = %s			token->type = %d\n", token->value, token->type);
// 		}
// 		parse_op(token_list);
// 		// t_token *current = token_list;
// 		// while (current)
// 		// {
// 		// 	printf("token->value = %s		token->type = %d\n", current->value, current->type);
// 		// 	current = current->next;
// 		// }
// 	}
// 	return (0);
// }