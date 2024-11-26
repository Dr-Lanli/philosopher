/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutexes.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 21:51:03 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/26 01:09:48 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

void mutex_init(struct s_data_shared *data)
{
	unsigned int i;

	i = 0;
	pthread_mutex_init(&data->lock_eat_state, NULL);
    pthread_mutex_init(&data->lock_think_state, NULL);
    pthread_mutex_init(&data->lock_sleep_state, NULL);
    pthread_mutex_init(&data->lock_time_state, NULL);
	pthread_mutex_init(&data->lock_dead_state, NULL);
	pthread_mutex_init(&data->lock_print, NULL);
	while (i < data->nb_of_philo)
		pthread_mutex_init(&data->lock_forks[i++], NULL);
}

void destroy_mutex(struct s_data_shared *data)
{
	unsigned int i;

	i = 0;
	while (i < data->nb_of_philo)
		pthread_mutex_destroy(&data->lock_forks[i++]);
	pthread_mutex_destroy(&data->lock_eat_state);
    pthread_mutex_destroy(&data->lock_think_state);
    pthread_mutex_destroy(&data->lock_sleep_state);
	pthread_mutex_destroy(&data->lock_time_state);
	pthread_mutex_destroy(&data->lock_dead_state);
}