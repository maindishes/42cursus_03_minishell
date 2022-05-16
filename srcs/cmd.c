/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoh <jihoh@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/15 03:01:22 by jihoh             #+#    #+#             */
/*   Updated: 2022/05/15 18:13:01 by jihoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	next_has_pipe(t_token *token)
{
	while (token && token->type != PIPE)
		token = token->next;
	if (!token)
		return (0);
	return (1);
}

t_token	*next_cmd(t_token *ptr)
{
	while (ptr && ptr->type != PIPE)
		ptr = ptr->next;
	if (ptr)
		ptr = ptr->next;
	return (ptr);
}

char	**create_args(t_token *tokens, t_token *token)
{
	int		i;
	char	**ret;
	t_token	*ptr;

	i = 0;
	ptr = token;
	while (ptr && ptr->type != PIPE)
	{
		if (ptr->type <= ARG)
			i++;
		ptr = ptr->next;
	}
	ret = (char **)malloc(sizeof(char *) * (i + 1));
	i = 0;
	ptr = token;
	while (ptr && ptr->type != PIPE)
	{
		if (ptr->type <= ARG)
			ret[i++] = ptr->str;
		ptr = ptr->next;
	}
	ret[i] = NULL;
	return (ret);
}

void	run_cmd(t_mini *mini, t_token *cmd, char **args, int flag)
{
	if (handle_redirect(cmd, &mini->fd) == ERROR)
		return ;
	if (builtin(&mini->envs, args) != SUCCESS)
		pre_exec(args, &mini->envs, flag);
	free(args);
}

void	wait_pipe_pid(void)
{
	int	status;
	int	flag;

	flag = 0;
	ignore_signal();
	while (waitpid(-1, &status, 0) > 0)
	{
		if (WIFSIGNALED(status) && !flag)
		{
			handler_2(WTERMSIG(status));
			flag = 1;
		}
	}
	set_signal();
}

void	run_cmd_with_pipe(t_mini *mini, t_token *cmd)
{
	char	**args;
	int		status;

	while (cmd)
	{
		args = create_args(&mini->tokens, cmd);
		pipe(mini->fd.pd);
		signal(SIGQUIT, SIG_DFL);
		mini->pid = fork();
		if (mini->pid == 0)
		{
			if (next_has_pipe(cmd))
				dup2(mini->fd.pd[1], 1);
			run_cmd(mini, cmd, args, 1);
			exit(0);
		}
		else
		{
			dup2(mini->fd.pd[0], 0);
			close(mini->fd.pd[1]);
		}
		cmd = next_cmd(cmd);
	}
	wait_pipe_pid();
}
