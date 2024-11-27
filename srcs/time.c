/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 23:51:58 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/27 19:26:37 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

uint32_t	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/* Permet d'ajuster le temps de démarrage de chaque thread
	en fonction de time_to_die et time_to_eat
	on calcul la différence divisé par deux afin de créer
	un délais entre les deux valeur.
*/
uint32_t	waiting_time(struct s_data_shared *data)
{
	uint32_t	waiting_time;

	waiting_time = (data->time_to_die - data->time_to_eat) / 2;
	if (waiting_time <= 0)
		waiting_time = 1;
	if (waiting_time > 600)
		waiting_time = 200;
	return (waiting_time);
}

/* Permet de gérer le temps d'attente d'un thread
	de manière plus optimal qu'un simple usleep.
*/
void	psleep(struct s_philo *philo, uint32_t sleep_time)
{
	uint32_t	end;

	end = get_time() + sleep_time;
	while (get_time() < end)
	{
		check_died(philo, philo->data);
		pthread_mutex_lock(&philo->data->lock_dead_state);
		if (philo->data->has_died)
		{
			pthread_mutex_unlock(&philo->data->lock_dead_state);
			break ;
		}
		pthread_mutex_unlock(&philo->data->lock_dead_state);
		usleep(100);
	}
}
