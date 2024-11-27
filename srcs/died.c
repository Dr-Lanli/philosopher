/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   died.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 23:31:34 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/27 19:26:14 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

static void	died(struct s_philo *philo, struct s_data_shared *data)
{
	pthread_mutex_lock(&data->lock_dead_state);
	if (!data->stop_flag)
	{
		write_in_stdout(philo, data, "died");
		pthread_mutex_lock(&data->lock_print);
		data->has_died = 1;
		pthread_mutex_unlock(&data->lock_print);
		data->stop_flag = 1;
	}
	pthread_mutex_unlock(&data->lock_dead_state);
}

void	check_died(struct s_philo *philo, struct s_data_shared *data)
{
	uint32_t	time_now;

	pthread_mutex_lock(&data->lock_eat_state);
	time_now = get_time();
	if (philo->last_plate)
	{
		if ((time_now - philo->last_plate) > data->time_to_die)
		{
			died(philo, data);
		}
	}
	pthread_mutex_unlock(&data->lock_eat_state);
}

int	has_died(struct s_philo *philo, struct s_data_shared *data)
{
	pthread_mutex_lock(&data->lock_dead_state);
	if (philo->has_died)
	{
		pthread_mutex_unlock(&data->lock_dead_state);
		return (1);
	}
	pthread_mutex_unlock(&data->lock_dead_state);
	return (0);
}
