#include "minishell.h"

int calculate_cmd_nbr(t_token *token)
{
    int count = 1; 
    
    while (token)
    {
        if(token->type == 1)
            count++;
        token = token->next;
        // printf("next token = %s", token->next->value);
    }
    return (count);
}

void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

// int	main(int argc, char **argv, char **env)
// {
//     (void)argc;
//     (void)argv;
//     // int pipe_fd[MAX_PIPES][2];
//     // t_data data;
// 	char	*input;
// 	t_lexer	*lexer;
// 	t_token	*token;
//     // int status;
//     signal(SIGQUIT, SIG_IGN);
//     signal(SIGINT, handler);
//     t_env *envlist = init_env(env);

//     rl_catch_signals = 0;
// 	while (1)
// 	{
// 		input = readline("minishell> ");
//          if (!input)
//         {
//             write(1, "exit\n", 5);
//             exit(0);
//         }
//         if (input[0] == '\0')
//         {
//             free(input);
//             continue;
//         }
//         if (input)
//         {
//             add_history(input); 
//             lexer = initialize_lexer(input);
//             while (lexer->position < lexer->lenght)
//             {
//                 token = get_next_token(lexer);
//                 if (!token->value)
//                     return (0);
//                 token->type = token_type(token);
//                 execute_builtin(token, lexer, &envlist);
//             }
//             free(input);
//         }
// 	}
// 	return (0);
// }


int	main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // int pipe_fd[MAX_PIPES][2];
    // t_data data;
	char	*input;
	t_lexer	*lexer;
	t_token	*token;
    t_token *head = NULL;
    t_token *current = NULL;
    int count = 0;
    // int status;
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, handler);
    // t_env *envlist = init_env(env);

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
                if (head == NULL) {
                    head = token;
                    current = head;
                } else {
                    current->next = token; 
                    current = current->next;
                }
                // execute_builtin(token, lexer, &envlist);
            }
            count = calculate_cmd_nbr(token);
            free(input);
        }
        printf("count = %d\n",count);
	}
	return (0);
}