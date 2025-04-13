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
    // int pipe_fd[MAX_PIPES][2];
    // t_data data;
	char	*input;
	t_lexer	*lexer;
	t_token	*token;
    // int status;
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
            while (lexer->position < lexer->lenght)
            {
                token = get_next_token(lexer);
                if (!token->value)
                    return (0);
                token->type = token_type(token);
                execute_builtin(token, lexer, &envlist);
            }
            free(input);
        }
	}
	return (0);
}