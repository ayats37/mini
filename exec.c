#include "minishell.h"

t_env *create_env_node(char *env_var)
{
    t_env *new_node;
    char *equal_sign;
    
    new_node = malloc(sizeof(t_env));
    if (!new_node)
        return NULL;

    equal_sign = strchr(env_var, '=');
    if (equal_sign)
    {
        new_node->name = strndup(env_var, equal_sign - env_var);
        new_node->value = strdup(equal_sign + 1);
    }
    else
    {
        new_node->name = strdup(env_var);
        new_node->value = NULL;
    }
    new_node->next = NULL;
    return new_node;
}

void add_to_env_list(t_env **head, t_env *new_node)
{
    t_env *tmp;

    if (*head == NULL) 
        *head = new_node;
    else
    {
        tmp = *head;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_node;
    }
}

int ft_cd(t_token *path, t_lexer *lexer, t_env *env_list)
{
    path = get_next_token(lexer);
    
    if (!path)
    {
        char *home_dir = get_env_value("HOME", env_list);
        if (!home_dir || !*home_dir)
            home_dir = getenv("HOME");
        if (!home_dir || !*home_dir)
        {
            printf("cd: HOME not set\n");
            return (1);
        }
        if (chdir(home_dir) != 0)
        {
            printf("cd: %s: No such file or directory\n", home_dir);
            return (1);
        }
    }
    else
    {
        if (chdir(path->value) != 0)
        {
            printf("cd: %s: No such file or directory\n", path->value);
            return (1);
        }
    }
    
    return (0);
}

int ft_pwd()
{
    char cwd[10240];
    if (!getcwd(cwd, sizeof(cwd)))
    {
        printf("error");
        return(1);
    }
    printf("%s\n", cwd);
    return (0);
}
int ft_echo(t_lexer *lexer, t_env *env_list)
{
    int new_line = 1;
    t_token *token = get_next_token(lexer);

    if (token && strcmp(token->value, "-n") == 0)
    {
        new_line = 0;
        token = get_next_token(lexer);
    }
    while (token)
    {
        char *str = token->value;
        int i = 0;
        while (str[i])
        {
            if (str[i] == '$')
            {
                int start = i + 1;
                int end = start;
                
                while (str[end])
                    end++;
                if (end > start)
                {
                    int var_len = end - start;
                    char *var = malloc(sizeof(char) * (var_len + 1));
                    if (!var)
                        return 1;
                    strncpy(var, &str[start], var_len);
                    var[var_len] = '\0';
                    char *var_value = get_env_value(var, env_list);
                    if (var_value)
                        printf("%s", var_value);
                    free(var);
                    
                    i = end;
                }
                else
                {
                    printf("$");
                    i++;
                }
            }
            else
            {
                printf("%c", str[i]);
                i++;
            }
        }
        token = get_next_token(lexer);
        if (token)
            printf(" ");
    }
    if (new_line)
        printf("\n");   
    return 0;
}

int ft_export(t_token *input, t_lexer *lexer, t_env **env_list)
{
   char *equal_sign;
   char *name;
   char *value;
   input = get_next_token(lexer);

   if (!input || !input->value)
    return(1);
   equal_sign = ft_strchr(input->value, '=');
   if (!equal_sign)
        return(1);
   name = strndup(input->value, equal_sign - input->value);
   value = ft_strdup(equal_sign + 1);
   update_env(name, value, env_list);
   free(name);
   free(value);
   return (0);
}

void update_env(char *name, char *value, t_env **env_list)
{
    t_env *current = *env_list;
    t_env *new_env;

    while (current)
    {
        if ((strcmp(current->name, name) == 0))
        {
            free(current->value);
            current->value = ft_strdup(value);
            return;
        }
        current = current->next;
    }
    new_env = malloc(sizeof(t_env));
    new_env->name = ft_strdup(name);
    new_env->value = ft_strdup(value);
    new_env->next = *env_list;
    *env_list = new_env;
}
char *get_env_value(char *name, t_env *env_list)
{
    t_env *current = env_list;

    while (current)
    {
        if (strcmp(current->name, name) == 0)
            return (current->value);
        current = current->next;
    }
    return(NULL);
}

int ft_env(t_env *env_list)
{
    t_env *current = env_list;
    
    while(current)
    {
        printf("%s=%s\n", current->name, current->value);
        current = current->next;
    }
    return(0);
}

int ft_exit(t_lexer *lexer)
{
    t_token *arg;
    int exit_status = 0;
    // t_env *current = env_list;
    
    arg = get_next_token(lexer);
    if (arg)
        exit_status = atoi(arg->value);
    // while (current)
    // {
    //     free(current->name);
    //     free(current->value);
    //     free(current);
    //     current = current->next;
    // }
    // free(lexer->input);
    // free(lexer);
    printf("exit\n");
    exit(exit_status);
    return (0);
}
int ft_unset(t_lexer *lexer, t_env **env_list)
{
    t_token *var;
    t_env *current = *env_list;
    t_env *prev = NULL;
    var = get_next_token(lexer);

    if (!var || !var->value)
        return (1);
    while(current)
    {
        if (strcmp(var->value, current->name) == 0)
        {
            if (prev == NULL)
                *env_list = current->next;
            else
                prev->next = current->next;
            free(current->name);
            free(current->value);
            free(current);
            return (0);
        }
        prev = current;
        current = current->next;
    }
    return(1);
}

int execute_builtin(t_token *token, t_lexer *lexer, t_env **envlist)
{
    if (strcmp(token->value, "echo") == 0)
        ft_echo(lexer, *envlist);
    else if (strcmp(token->value, "cd") == 0)
        return(ft_cd(token, lexer, *envlist));
    else if (strcmp(token->value, "pwd") == 0)
        return(ft_pwd());
    else if (strcmp(token->value, "export") == 0)
        return(ft_export(token, lexer, envlist));
    else if (strcmp(token->value, "unset") == 0)
        return(ft_unset(lexer, envlist));
    else if (strcmp(token->value, "env") == 0)
        return(ft_env(*envlist));
    else if (strcmp(token->value, "exit") == 0)
        return(ft_exit(lexer));
    return (0);
}

t_env *init_env(char **envp)
{
    t_env *head = NULL;
    t_env *new_node;
    int i = 0;

    while (envp[i])
    {
        new_node = create_env_node(envp[i]);
        if (!new_node)
            return (NULL); 
        add_to_env_list(&head, new_node);
        i++;
    }
    return (head); 
}
// int handle_variable(t_token *token, t_lexer *lexer, t_env *env_list)
// {
//     while (token)
//     {
//         char *str = token->value;
//         int i = 0;
//         while (str[i])
//         {
//             if (str[i] == '$')
//         }
//     }
// }

int	main(int argc, char **argv, char **env)
{
	char	*input;
	t_lexer	*lexer;
	t_token	*token;
    t_env *envlist = init_env(env);

	(void)argc;
	(void)argv;
	while (1)
	{
		input = readline("minishell> ");
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
