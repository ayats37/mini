#include "minishell.h"

int is_builtin(char *cmd)
{
    if (!cmd)
        return (0);
    return (!strcmp(cmd, "cd")
        || !strcmp(cmd, "echo")
        || !strcmp(cmd, "exit")
        || !strcmp(cmd, "env")
        || !strcmp(cmd, "export")
        || !strcmp(cmd, "unset")
        || !strcmp(cmd, "pwd"));
}

void write_error(char *message)
{
    perror(message);
    exit(1);
}
int execute_cmds(char **cmds, char **env)
{
    pid_t pid;
    int status;
    char *full_path;

    pid = fork();
    if (pid == -1)
        write_error("fork failed");
    if (pid == 0)
    {
        full_path = find_cmd_path(cmds[0], env);
        if (!full_path)
            write_error("command not found");
        execve(full_path, cmds, env);
        free(full_path);
        write_error("execve failed");
    }
    else
    {
        waitpid(pid, &status, 0);
        return (WEXITSTATUS(status));
    }
    return (0);
}
// int execute_pipe(t_tree *node, char **env)
// {
//     pid_t pid1;
//     pid_t pid2;
//     int status1;
//     int status2;
//     int pipe_fd[2];

//     if (pipe(pipe_fd) == -1)
//         write_error("pipe failed");
//     pid1 = fork();
//     if (pid1 == -1)
//         write_error("fork failed");
//     if (pid1 == 0)
//     {
//         close(pipe_fd[0]);
//         dup2(pipe_fd[1], STDOUT_FILENO);
//         close(pipe_fd[1]);
//         execute_tree(node->left, env);
//         exit(0);
//     }
//     pid2 = fork();
//     if (pid2 == -1)
//         write_error("fork failed");
//     if (pid2 == 0)
//     {
//         close(pipe_fd[1]);
//         dup2(pipe_fd[0], STDIN_FILENO);
//         close(pipe_fd[0]);
//         execute_tree(node->right, env);
//         exit(0);
//     }
//     close(pipe_fd[0]);
//     close(pipe_fd[1]);
//     waitpid(pid1, &status1, 0);
//     waitpid(pid2, &status2, 0);
    
//     return (WEXITSTATUS(status2));
// }

// int handle_redirection(t_tree *node)
// {
//     int fd;
//     if (node->type == REDIR_IN)
//     {
//         if ((fd = open(node->file, O_RDONLY)) == -1)
//             write_error("open fd failed");
//         dup2(fd, STDIN_FILENO);
//         close(fd);
//     }
//     if (node->type == REDIR_OUT)
//     {
//         if ((fd = open(node->file,  O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
//             write_error("open fd failed");
//         dup2(fd, STDOUT_FILENO);
//         close(fd);
//     }
//     if (node->type == APPEND)
//     {
//         if ((fd = open(node->file, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1)
//             write_error("open fd failed");
//         dup2(fd, STDOUT_FILENO);
//         close(fd);
//     }
//     return (0);
// }

int execute_tree(t_tree *node, char **env)
{
    int status;
    t_env *envlist = NULL;
    char *cmd[] = {"ls", NULL};

    status = 0;
    if (!node)
    {
        write(1, "eegrg\n", 7);
        return (1);
    }
    if (node->type == CMD)
    {
        if (is_builtin(cmd[0]))
            return (execute_builtin(node->token, &envlist));
        else
            return (execute_cmds(cmd, env));
    }
    // else if (node->type == PIPE)
    //     return (execute_pipe(node, env));
    // else if (node->type == REDIR_IN || node->type == REDIR_OUT || node->type == APPEND || node->type == HEREDOC)
    //  {
    //     status = handle_redirection(node);
    //     if (status != 0)
    //         return (status);
    //     return (execute_tree(node->left, env));
    //  } 
    //  else if (node->type == AND || node->type == OR)
    //  {
    //     status = execute_tree(node->left, env);
    //     if (node->type == AND && status == 0)
    //         return (execute_tree(node->right, env));
    //     if (node->type == OR && status != 0)
    //         return (execute_tree(node->right, env));
    //     return (status);
    //  }
     return (1);
}
