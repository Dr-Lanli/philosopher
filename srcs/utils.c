/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 17:13:54 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/27 03:47:48 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

void	free_struct(struct s_philo **philo, struct s_data_shared *data)
{
	unsigned int	i;

	i = 0;
	while (i < data->nb_of_philo)
	{
		free(philo[i++]);
	}
	free(philo);
	if (data->lock_forks)
	{
		free(data->lock_forks);
	}
}

void	destroy_threads(struct s_philo **philo, struct s_data_shared *data)
{
	unsigned int	i;

	i = 0;
	while (i < data->nb_of_philo)
	{
		if (pthread_join(philo[i]->thread, NULL) != 0)
		{
			free_struct(philo, data);
			exit(EXIT_FAILURE);
		}
		usleep(25);
		i++;
	}
}

void	write_in_stdout(struct s_philo *philo, struct s_data_shared *data,
		char *state)
{
	pthread_mutex_lock(&data->lock_print);
	if (data->has_died)
	{
		pthread_mutex_unlock(&data->lock_print);
		return ;
	}
	else if (ft_strcmp(state, "fork") == 0)
		printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(),
			philo->id);
	else if (ft_strcmp(state, "eat") == 0)
		printf("{%u ms} philosophe[%d] eat\n", get_time(), philo->id);
	else if (ft_strcmp(state, "sleep") == 0)
		printf("{%u ms} philosophe[%d] sleep\n", get_time(), philo->id);
	else if (ft_strcmp(state, "think") == 0)
		printf("{%u ms} philosophe[%d] think\n", get_time(), philo->id);
	else if (ft_strcmp(state, "died") == 0)
		printf("{%u ms} philosophe[%d] has died\n", get_time(), philo->id);
	pthread_mutex_unlock(&data->lock_print);
}
