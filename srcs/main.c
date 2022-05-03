/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoh <jihoh@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/01 21:41:26 by jihoh             #+#    #+#             */
/*   Updated: 2022/05/03 21:22:11 by jihoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	**create_args(t_token *tokens)
{
	int		i;
	char	**ret;
	t_token	*ptr;

	i = 0;
	ptr = tokens->first;
	while (ptr)
	{
		i++;
		ptr = ptr->next;
	}
	ret = (char **)malloc(sizeof(char *) * (i + 1));
	i = 0;
	ptr = tokens->first;
	while (ptr)
	{
		ret[i++] = ptr->str;
		ptr = ptr->next;
	}
	ret[i] = NULL;
	return (ret);
}

int	parsing_cmd(char *str, t_token *tokens)
{
	int		i;
	char	quot;
	t_token	*tmp;

	i = 0;
	quot = '\0';
	parsing_line(str, &quot, tokens, i);
	tmp = tokens->first;
	while (tmp)
	{
		printf("%d %s\n", tmp->type, tmp->str);
		tmp = tmp->next;
	}
	if (quot)
	{
		fprintf(stderr, "minishell: single quotate error\n");
		return (ERROR);
	}
	return (SUCCESS);
}

void	prompt(t_env *envs, char **env)
{
	char	*str;
	char	**args;
	t_token	tokens;

	set_signal();
	while (1)
	{
		str = readline("🐚minishell$ ");
		args = (char **)malloc(sizeof(char *) * ARG_MAX);
		if (!*str)
			continue ;
		tokens.first = NULL;
		add_history(str);
		if (parsing_cmd(str, &tokens) == ERROR)
			continue ;
		args = create_args(&tokens);
		if (!args[0] || builtin(envs, args) == SUCCESS)
			continue ;
		else
			exec(args, env, envs);
		free_token(&tokens);
		free(args);
		free(str);
	}
}

static void	print_envs(t_env *envs)
{
	int	i;

	i = 0;
	while (envs)
	{
		ft_putendl_fd(envs->key, 2);
		envs = envs->next;
	}
}

int	main(int ac, char **av, char **env)
{
	t_env	envs;
	int		i;

	i = 0;
	envs.first = NULL;
	while (env[i])
		add_env(&envs, env[i++]);
	init_shlvl(&envs);
	prompt(&envs, env);
	return (0);
}
