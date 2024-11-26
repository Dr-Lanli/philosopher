/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 23:51:58 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/26 01:04:39 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

uint32_t get_time(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

uint32_t waiting_time(struct s_data_shared *data)
{
	uint32_t	waiting_time;

	waiting_time = (data->time_to_die - data->time_to_eat) / 2;
	if (waiting_time <= 0)
		waiting_time = 1;
	if (waiting_time > 600)
		waiting_time = 200;
	return (waiting_time);
}

void psleep(uint32_t sleep_time)
{
	uint32_t	wake_up;

	
	wake_up = get_time() + sleep_time;
	while (get_time() < wake_up)
	{
		//pthread_mutex_lock(&data->lock_dead_state);
		//if (data->stop_flag)
			//break;
		//pthread_mutex_unlock(&data->lock_dead_state);
		usleep(100);
	}
}