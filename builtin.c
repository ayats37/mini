/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taya <taya@student.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 12:09:19 by taya              #+#    #+#             */
/*   Updated: 2025/04/13 14:04:43 by taya             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_echo(t_lexer *lexer, t_env *env_list)
{
    int new_line;
    t_token *token;
    char *str;
    int i;
    int offset;

    new_line = 1;
    token = get_next_token(lexer);
    if (token && strcmp(token->value, "-n") == 0)
    {
        new_line = 0;
        token = get_next_token(lexer);
    }
    while (token)
    {
        str = token->value;
        i = 0;
        while (str[i])
        {
            if (str[i] == '$')
            {
                offset = handle_variable(&str[i], env_list);
                i += offset + 1; 
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
    return (0);
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

int ft_exit(t_lexer *lexer, t_env *env_list)
{
    t_token *arg;
    int exit_status = 0;
    t_env *current = env_list;
    
    arg = get_next_token(lexer);
    if (arg)
        exit_status = atoi(arg->value);
    while (current)
    {
        free(current->name);
        free(current->value);
        free(current);
        current = current->next;
    }
    free(lexer->input);
    free(lexer);
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

