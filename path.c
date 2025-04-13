#include "minishell.h"

void write_split(char **split)
{
	int i = 0;
	while (split[i])
	{
		printf("%s\n", split[i]);
		i++;
	}
}

int calculate_cmd_nbr(t_token *token)
{
    int count = 1; 
    
    while (token)
    {
        while(token->value)
        {
            if(token->type == 1)
                count++;
        }
    }
    return (count);
}


void execute_one(char *input, t_data *data)
{
	char **cmd;
	pid_t pid;
	pid  = fork();
	cmd = ft_split(input, 32);
	if (pid == 0)
	{
		char *full_path = check_path(data->env, cmd[0]);
		if (!full_path)
			exit(1);
		execve(full_path, cmd, data->env);
	}
	wait(NULL);
}

int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    int pipe_fd[MAX_PIPES][2];
    t_data data;
    char *input;
    int status;
    data.env = env;

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, handler);
    rl_catch_signals = 0;
    char *prompt = "minishel> ";

    while (1)
    {
        input = readline(prompt);
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
        data.cmd_nbrs = calculate_cmd_nbr(input);
		// printf("cmd_nbrs = %d\n", data.cmd_nbrs);
		if (data.cmd_nbrs == 1)
			execute_one(input, &data);
        else if (data.cmd_nbrs > 1)
        {
            create_pipe(&data, pipe_fd);
            create_child(&data, pipe_fd, input);
            close_pipes(&data, pipe_fd);
            wait_children(&data, &status);
        }
        free(input);
    }  
    return (WEXITSTATUS(status));
}









// int main(int ac, char **av, char **env)
// {

// 	(void)ac;
// 	(void)av;
// 	pid_t pid;
// 	char **cmd;
// 	char *ret;
	
// 	signal(SIGQUIT, SIG_IGN);
// 	signal(SIGINT, handler);
// 	rl_catch_signals = 0;
// 	char *str = "minishel> ";
// 	while(1)
// 	{
// 		ret = readline(str);
// 		if (!ret)
// 		{
// 			write(1,"exit\n", 5);
// 			exit(0);
// 		}
// 		else if (ret)
// 		{
// 			pid  = fork();
// 			cmd = ft_split(ret, 32);
// 			if (pid == 0)
// 			{
// 				char *full_path = check_path(env, cmd[0]);
// 				if (!full_path)
// 					return(1);
// 				execve(full_path, cmd, env);
// 			}
// 			wait(NULL);
// 		}
// 	}
// 	return (0);
// }