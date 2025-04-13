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

int  is_alphanumeric(int c)
{
    return ((c >= 'A' && c <= 'Z') || 
            (c >= 'a' && c <= 'z') || 
            (c >= '0' && c <= '9'));
}

int handle_variable(char *str, t_env *env_list)
{
    int var_len;
    char *var;
    char *value;

    var_len = 0;
    while (str[var_len + 1] && (is_alphanumeric(str[var_len + 1]) || str[var_len + 1] == '_'))
        var_len++;
    if (var_len > 0)
    {
        var = ft_substr(str, 1, var_len);
        value = get_env_value(var, env_list);
        if (value)
            printf("%s", value);
        free(var);
        return (var_len);
    }
    else
    {
        printf("$");
        return (0);
    }
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

int execute_builtin(t_token *token, t_env **envlist)
{
    if (strcmp(token->value, "echo") == 0)
        return(ft_echo(token, *envlist));
    else if (strcmp(token->value, "cd") == 0)
        return(ft_cd(token));
    else if (strcmp(token->value, "pwd") == 0)
        return(ft_pwd());
    else if (strcmp(token->value, "export") == 0)
        return(ft_export(token, envlist));
    else if (strcmp(token->value, "unset") == 0)
        return(ft_unset(token, envlist));
    else if (strcmp(token->value, "env") == 0)
        return(ft_env(*envlist));
    else if (strcmp(token->value, "exit") == 0)
        return(ft_exit(token, *envlist));
    return (0);
}


