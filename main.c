#include "minishell.h"



void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

int	main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    t_token *token_list = NULL;
	char	*input;
	t_lexer	*lexer;
	t_token	*token;
    // t_tree *node;
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, handler);
    t_env *envlist = init_env(env);

    rl_catch_signals = 0;
	while (1)
	{
		input = readline("minishell> ");
         if (!input)
        {
            write(1, "exit\n", 5);
            exit(0);
        }
        if (input[0] == '\0')
        {
            free(input);
            continue;
        }
        if (input)
        {
            add_history(input); 
            lexer = initialize_lexer(input);
            token_list = NULL;
            while (lexer->position < lexer->lenght)
            {
                token = get_next_token(lexer);
                if (!token->value)
                    continue ;
                token->type = token_type(token);
                append_token(&token_list, token);
            }
            parse_op(token_list);
            execute_builtin(token_list, &envlist);
            free(input);
        }
	}
	return (0);
}
