/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoh <jihoh@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/01 21:40:49 by jihoh             #+#    #+#             */
/*   Updated: 2022/05/18 15:34:07 by jihoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/stat.h>

enum e_token_type
{
	CMD = 0,
	ARG = 1,
	DIRE = 2,
	REDIROUT = 3,
	APPEND = 4,
	REDIRIN = 5,
	HEREDOC = 6,
	PIPE = 7
} ;

enum e_std_type
{
	STDIN = 0,
	STDOUT = 1,
	STDERR = 2
} ;

enum e_return_type
{
	SUCCESS = 0,
	ERROR = 1
} ;

typedef struct s_env
{
	struct s_env	*first;
	char			*key;
	char			*value;
	struct s_env	*next;
}				t_env;

typedef struct s_token
{
	struct s_token	*first;
	int				type;
	char			*str;
	struct s_token	*next;
}				t_token;

typedef struct s_fd
{
	int	sd[2];
	int	fd[2];
	int	pd[2];
	int	hd[2];
}				t_fd;

typedef struct s_mini
{
	t_env	envs;
	t_token	tokens;
	t_fd	fd;
	int		exit_code;
}				t_mini;

/*
*** parsing ***
*/
int		sep_check(char *str);
void	replace_char(char **pstr, char *quot, int *i);
int		trim_space_sub(char **pstr, char *quot, int *i);
void	trim_space(char *str, char *quot, int i);
int		parsing_line(char *str, t_mini *mini);

/*
*** syntax ***
*/
int		join_putstr_fd(char *a, char *b, char *c, int fd);
int		check_type(int type);
int		syntax_check_next(t_mini *mini, t_token *prev, t_token *token);
int		syntax_check(t_mini *mini, t_token *token);

/*
*** dollar ***
*/
int		dollar_check(char *str);
char	*end_of_dollar(char *str);
char	*search_dollar_value(t_mini *mini, char *str);

/*
*** token ***
*/
void	free_token(t_token *tokens);
void	set_token_type(t_token *tokens, t_token *token, int is_sep);
void	add_token(t_token *tokens, char *str, int is_sep);
int		check_empty_token(char *start, char *str, int i, t_mini *mini);
void	create_tokens(t_mini *mini, char *str, char *quot, int i);

/*
*** str_to_token ***
*/
int		token_len(t_mini *mini, char *str);
void	str_to_token_sub(t_mini *mini, char *str, char *ret);
char	*str_to_token(t_mini *mini, char *start, char *end);

/*
*** cmd ***
*/
char	**create_args(t_token *token);
void	run_cmd(t_mini *mini, t_token *cmd, char **args, int flag);
void	run_cmd_with_pipe(t_mini *mini, t_token *cmd);
int		next_has_pipe(t_token *token);

/*
*** builtin ***
*/
void	status_error_check(t_mini *mini, int sign, char *str);
void	ft_exit(t_mini *mini, char **args);
void	unset(t_mini *mini, t_env *envs, char **args);
void	echo(t_mini *mini, char **args);
int		builtin(t_mini *mini, char **args);

/*
*** cd ***
*/
void	cd(t_mini *mini, t_env *envs, char **args);
void	cd_sub(t_mini *mini, t_env *envs, char **args);

/*
*** env ***
*/
void	remove_env(t_env *envs, char *key);
t_env	*search_env(t_env *envs, char *key);
void	add_env_sub(t_env *envs, char *key, char *value);
int		add_env(t_env *envs, char *name);
void	env(t_mini *mini, t_env *envs);

/*
*** export ***
*/
char	*validate_key(char *key, char *cmd);
void	free_sort_env(t_env *envs);
t_env	*sort_env_list(t_env *temp);
t_env	*copy_env_list(t_env *envs);
void	export(t_mini *mini, t_env *envs, char **args);

/*
*** redirect ***
*/
void	heredoc(t_mini *mini, t_token *token);
void	set_heredoc_fd(t_mini *mini, t_token *token);
int		change_inout_sub(t_mini *mini, t_token *token);
int		change_inout(t_mini *mini, t_token *token);
int		handle_redirect(t_mini *mini, t_token *token);

/*
*** exec ***
*/
char	**convert_env(t_env *envs);
void	pre_exec(t_mini *mini, char **args, int flag);
void	exe_command(t_mini *mini, char **args);
void	stat_check(char *args);
void	find_abs_exe(char *command, char *envs[], char buffer[], int buf_size);

/*
*** tools ***
*/
t_env	*get_env_node(char *key, char *value);
t_token	*get_token_node(int type, char *str);
int		is_sep(char s);
int		is_quot(char s);
void	check_newline(char *buffer);

/*
*** signal ***
*/
void	set_exit_code(t_mini *mini, int status);
void	handler_1(int signo);
void	handler_2(int signo);
void	set_signal(void);
void	ignore_signal(void);

#endif