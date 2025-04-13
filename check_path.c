#include"minishell.h"

char	*get_path(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);
		i++;
	}
	return (NULL);
}
char	**get_paths(char **env)
{
	char	*path_string;
	char	**paths;

	path_string = get_path(env);
	if (!path_string)
	{
		perror("path not found");
		return (NULL);
	}
	paths = ft_split(path_string, ':');
	if (!paths)
		perror("path split failed");
	return (paths);
}

char	*build_path(char *path, char *cmd)
{
	char	*full_path;

	full_path = malloc(ft_strlen(path) + ft_strlen(cmd) + 2);
	if (!full_path)
		return (NULL);
	ft_strjoin(full_path, path);
	ft_strjoin(full_path, "/");
	ft_strjoin(full_path, cmd);
	return (full_path);
}

char	*check_paths(char **paths, char *cmd)
{
	char	*full_path;
	int		i;

	if ((cmd[0] == '.' && cmd[1] == '/') || cmd[0] == '/')
	{
		if (access(cmd, F_OK | X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	i = 0;
	while (paths[i])
	{
		full_path = build_path(paths[i], cmd);
		if (!full_path)
			return (NULL);
		if (access(full_path, F_OK | X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}