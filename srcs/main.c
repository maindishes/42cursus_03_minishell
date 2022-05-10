/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoh <jihoh@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/01 21:41:26 by jihoh             #+#    #+#             */
/*   Updated: 2022/05/03 21:07:45 by junyopar         ###   ########.fr       */
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
void	eof_history(char *str)
{
	if (str == NULL)
	{
		// ft_putstr_fd("\033[1A",2);
		// ft_putstr_fd("\033[6C",2);
		ft_putstr_fd("\033[15D",1);
		ft_putstr_fd("🐚minishell$ ",1);
		ft_putstr_fd("exit\n",2);
		exit(EXIT_SUCCESS);
	}
	else
	{
		if (ft_strcmp(str, "exit") == 0)
		{
			ft_putendl_fd("exit",2);
			free(str);
			exit(EXIT_SUCCESS);
		}
		add_history(str);
	}
}
void	print_lst(t_token *lst)
{
	while (lst != NULL)
	{
		ft_putendl_fd(lst->str,2);
		lst = lst->next;
	}
}
void	prompt(t_env *envs,char **env)
{
	char	*str;
	char	**args;
	t_token	tokens;

	set_signal();
	while (1)
	{	
		int k = 0;
		set_signal();
		str = readline("🐚minishell$ ");
		eof_history(str);
		if (!*str)
			continue ;
		tokens.first = NULL;
		if (parsing_cmd(str, &tokens) == ERROR)
			continue ;
		args = create_args(&tokens);
		if (!args[0] || builtin(envs, args) == SUCCESS)
			continue ;
		else
		{
			print_lst(&tokens);
			// while(args[k])
				// fprintf(stderr,"check args : %s\n",args[k++]);
			pre_exec(args, envs,&tokens);
		}
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
