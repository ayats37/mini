/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ouel-afi <ouel-afi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 12:58:09 by ouel-afi          #+#    #+#             */
/*   Updated: 2025/04/12 11:40:54 by ouel-afi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include "./libft/libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#define MAX_PIPES 1024
typedef struct s_data
{
	char **env;
	int cmd_nbrs;	
	int i;
}	t_data;

typedef struct s_lexer {
	int position;
	int lenght;
	char *input;
}	t_lexer;

typedef struct s_env
{
	char *name;
	char *value;
	struct s_env *next;
	char **env;
}	t_env;

typedef enum s_type {
    CMD,
	WORD,
	PIPE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE,
    REDIR_IN,
    REDIR_OUT,
    APPEND,
    HEREDOC,
    OPEN_PAREN,
    CLOSE_PAREN,
	OR,
	AND
} t_type;

typedef enum s_precedence {
	WORDS = 2,
	QUOTES,
	PAREN,
	REDIR,
	PIPES,
	OPER
} t_precedence;

typedef struct s_token {
	char	*value;
	t_type		type;
	t_precedence prece;
	struct s_token *next;
}	t_token;

typedef struct s_queue {
	t_token *front;
	t_token *back;
} t_queue;

char	**ft_split(char const *s, char c);
char *check_path(char **env, char *cmd);
void	create_child(t_data *data, int pipe_fd[][2], char *input);
void create_pipe(t_data *data, int pipe_fd[][2]);
void	close_pipes(t_data *data, int pipe_fd[][2]);
void child(t_data *data, int pipe_fd[][2], char *input);
void	execute_cmd(t_data *data, char *input);
void first_cmd(t_data *data, int pipe_fd[][2]);
void middle_cmd(t_data *data, int pipe_fd[][2]);
void last_cmd(t_data *data, int pipe_fd[][2]);
void	wait_children(t_data *data, int *status);
void write_split(char **split);

//************************************************************tokenzation********************************************************************************
t_lexer	*initialize_lexer(char *input);
int	is_space(t_lexer *lexer);
void	skip_whitespace(t_lexer *lexer);
t_token	*create_token(char *value, char quote);
t_token	*quote_token(t_lexer *lexer, char quote);
t_token	*handle_quote(t_lexer *lexer, char quote);
t_token	*handle_operations(t_lexer *lexer, char *oper, int i);
t_token	*handle_word(t_lexer *lexer);
t_token	*get_next_token(t_lexer *lexer);
int	cmd_type(t_token *token, int first_cmd);
t_type	token_type(t_token *token);
t_precedence precedence_type(t_token *token);
//***************************************exec**********************************************
void update_env(char *name, char *value, t_env **env_list);
char *get_env_value(char *name, t_env *env_list);
t_env *init_env(char **env);
t_env *create_env_node(char *env_var);
void add_to_env_list(t_env **head, t_env *new_node);
t_env *init_env(char **envp);

#endif