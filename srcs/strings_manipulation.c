/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strings_manipulation.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 23:15:48 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/27 19:26:32 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

int	ft_strcmp(char *s1, char *s2)
{
	while (*s1 == *s2 && *s1)
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

static size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}

static int	is_valid_int(char *str)
{
	size_t	len;
	char	*int_max_str;
	char	*int_min_str;

	len = ft_strlen(str);
	int_max_str = "2147483647";
	int_min_str = "-2147483648";
	if (str[0] == '-')
	{
		if (len > ft_strlen(int_min_str))
			return (0);
		if (len == ft_strlen(int_min_str) && ft_strcmp(str, int_min_str) > 0)
			return (0);
	}
	else
	{
		if (len > ft_strlen(int_max_str))
			return (0);
		if (len == ft_strlen(int_max_str) && ft_strcmp(str, int_max_str) > 0)
			return (0);
	}
	return (1);
}

void	browse_inputs(char *argv[])
{
	char	*current_arg;

	argv++;
	while (*argv)
	{
		if (!is_valid_int(*argv))
		{
			printf("Error: arg not an int\n");
			exit(EXIT_FAILURE);
		}
		current_arg = *argv;
		while (*current_arg)
		{
			if (*current_arg < '0' || *current_arg > '9')
			{
				printf("Error: arg not an int\n");
				exit(EXIT_FAILURE);
			}
			current_arg++;
		}
		argv++;
	}
}

int	ft_atoi(const char *str)
{
	int	i;
	int	is_negative;
	int	n;

	i = 0;
	n = 0;
	is_negative = 0;
	while (str[i] == ' ')
		i++;
	if (str[i] == '-')
	{
		is_negative = 1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		n = n * 10 + str[i++] - '0';
	}
	if (is_negative)
		return (-n);
	else
		return (n);
}
