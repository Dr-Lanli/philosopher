/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 17:13:54 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/11 22:03:26 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

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

uint32_t get_time(uint32_t time_to_add)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000 + time_to_add);
}

int has_died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	pthread_mutex_lock(&data->lock_dead_state);
	if (philosophe->has_died)
	{
		pthread_mutex_unlock(&data->lock_dead_state);
		return (1);
	}
	pthread_mutex_unlock(&data->lock_dead_state);
    return (0);
}

void free_struct(struct s_philosopher **philosophe, struct s_data_shared *data)
{
	int i;

	i = 0;
	while (i < data->number_of_philosophers)
	{
		free(philosophe[i++]);
	}
	free(philosophe);
	if (data->lock_forks)
	{
		free(data->lock_forks);
	}
}

int	ft_strcmp(char *s1, char *s2)
{
	while (*s1 == *s2 && *s1)
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

void write_in_stdout(struct s_philosopher *philosophe, struct s_data_shared *data, char *state)
{
	pthread_mutex_lock(&data->lock_print);
	if (ft_strcmp(state, "fork") == 0)
	{
		printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(0), philosophe->id);
	}
	else if (ft_strcmp(state, "eat") == 0)
	{
		printf("{%u ms} philosophe[%d] eat\n", get_time(0), philosophe->id);
	}
	else if (ft_strcmp(state, "sleep") == 0)
	{
		printf("{%u ms} philosophe[%d] sleep\n", get_time(0), philosophe->id);
	}
	else if (ft_strcmp(state, "think") == 0)
	{
		printf("{%u ms} philosophe[%d] think\n", get_time(0), philosophe->id);
	}
	else if (ft_strcmp(state, "died") == 0)
	{
		printf("{%u ms} philosophe[%d] has died\n", get_time(0), philosophe->id);
	}
	pthread_mutex_unlock(&data->lock_print);
}
