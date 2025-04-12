#include "minishell.h"

void create_pipe(t_data *data, int pipe_fd[][2])
{
	int i;

	i = 0;
	while (i < data->cmd_nbrs - 1)
	{
		if (pipe(pipe_fd[i]) == -1)
		{
			perror("pipe failed");
			exit(1);
		}
		i++;
	}
}
char **split_commands(char *input)
{
    return ft_split(input, '|');
}

void create_child(t_data *data, int pipe_fd[][2], char *input)
{
    pid_t pid;
    int i;
    char **commands = split_commands(input);

    i = 0;
    while (i < data->cmd_nbrs)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            exit(1);
        }
        if (pid == 0)
        {
            data->i = i;
            child(data, pipe_fd, commands[i]);
        }
        i++;
    }
}

void	close_pipes(t_data *data, int pipe_fd[][2])
{
	int	i;

	i = 0;
	while (i < data->cmd_nbrs - 1)
	{
		close(pipe_fd[i][0]);
		close(pipe_fd[i][1]);
		i++;
	}
}

void	wait_children(t_data *data, int *status)
{
	int	i;

	i = 0;
	while (i < data->cmd_nbrs)
	{
		wait(status);
		i++;
	}
}
