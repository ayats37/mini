#include "minishell.h"

void first_cmd(t_data *data, int pipe_fd[][2])
{
	if (dup2(pipe_fd[data->i][1], STDOUT_FILENO) == -1)
	{
		perror("co");
		exit(1);
	}
}
void middle_cmd(t_data *data, int pipe_fd[][2])
{
	dup2(pipe_fd[data->i - 1][0], STDIN_FILENO);
	dup2(pipe_fd[data->i][1], STDOUT_FILENO);
}
void last_cmd(t_data *data, int pipe_fd[][2])
{
	dup2(pipe_fd[data->i - 1][0], STDIN_FILENO);
}

void	execute_cmd(t_data *data, char *input)
{
	char *cmd_path;
	char **cmd_args;

	cmd_args = ft_split(input, ' ');
	if (!cmd_args)
	{
		perror("ft_split failed");
		exit(1);
	}
	cmd_path  = check_path(data->env, cmd_args[0]);
	if (!cmd_path)
	{
		printf("%s: command not found\n", cmd_args[0]);
		exit(1);
	}
	execve(cmd_path, cmd_args, data->env);
	perror("execve failed");
	exit(1);
}

void child(t_data *data, int pipe_fd[][2], char *input)
{
    int j = 0;

    if (data->i == 0)
        first_cmd(data, pipe_fd);
	// printf("input = %s\n", input);
    else if (data->i < data->cmd_nbrs - 1)
			last_cmd(data, pipe_fd);
    else
			middle_cmd(data, pipe_fd); 
    
    while (j < data->cmd_nbrs - 1)
    {
        close(pipe_fd[j][0]);
        close(pipe_fd[j][1]);
        j++;
    }
    execute_cmd(data, input);
}