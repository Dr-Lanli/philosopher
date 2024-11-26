/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 21:50:58 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/26 01:12:04 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

/* add thread in create_thread function */
/*if (pthread_create(&data->check_thread, NULL, 
&is_alive_routine, philosophe) != 0)
{
	exit(EXIT_FAILURE);
}*/
/*void *is_alive_routine(void *arg)
{
    struct s_philosopher **philosophe = arg;
	struct s_data_shared *data = (*philosophe)->data;
	int i;

    while (1)
    {
		i = 0;
        while (i < data->number_of_philosophers)
        {
			if (philosophe[i]->id % 2)
				usleep(250);
            pthread_mutex_lock(&data->lock_dead_state);
			if (data->number_of_times_each_philosopher_must_eat)
			{
				if (philosophe[i]->nb_plate_eaten >
				 data->number_of_times_each_philosopher_must_eat)
				{
					pthread_mutex_unlock(&data->lock_dead_state);
					data->stop_flag = 1;
					break;
				}
			}
			pthread_mutex_unlock(&data->lock_dead_state);
			check_died(philosophe[i], data);
			i++;
        }
		if (data->stop_flag)
			break;
        usleep(2500);
    }
    return (NULL);
}*/

static bool is_all_plate_eaten(struct s_philo *philo, struct s_data_shared *data)
{
	if (data->nb_of_times_each_philo_must_eat)
	{
		if (philo->nb_plate_eaten > data->nb_of_times_each_philo_must_eat)
		{
			return (true);
		}
	}
	return (false);
}

static void routine_condition(struct s_philo *philo, struct s_data_shared *data)
{
	write_in_stdout(philo, data, "eat");
	pthread_mutex_lock(&data->lock_eat_state);
	philo->nb_plate_eaten++;
	philo->last_plate = get_time();
	pthread_mutex_unlock(&data->lock_eat_state);
	psleep(data->time_to_eat);
	check_died(philo, data);
	pthread_mutex_unlock(&data->lock_forks[philo->forks[0]]);
	pthread_mutex_unlock(&data->lock_forks[philo->forks[1]]);
	write_in_stdout(philo, data, "sleep");
	psleep(data->time_to_sleep);
	write_in_stdout(philo, data, "think");
}

void *routine(void *arg)
{
    struct s_philo *philo;
    struct s_data_shared *data;

    philo = arg;
    data = philo->data;
	if (philo->id % 2)
		psleep(waiting_time(data));
	while (!has_died(philo, data) && !data->stop_flag)
    {
		check_died(philo, data);
		pthread_mutex_lock(&data->lock_forks[philo->forks[0]]);
        write_in_stdout(philo, data, "fork");
		check_died(philo, data);
        pthread_mutex_lock(&data->lock_forks[philo->forks[1]]);
        write_in_stdout(philo, data, "fork");
		check_died(philo, data);
		routine_condition(philo, data);
		check_died(philo, data);
		if (is_all_plate_eaten(philo, data))
			return (NULL);
        usleep(250);
    }
    return (NULL);
}

void *deadly_routine(void *arg)
{
	struct s_philo *philo;
	struct s_data_shared *data;

	philo = arg;
	data = philo->data;
	printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(), philo->id);
	usleep(data->time_to_die * 1000);
	write_in_stdout(philo, data, "died");
	return (NULL);
}

void create_threads(struct s_philo **philo, struct s_data_shared *data)
{
    unsigned int i;

	//data->time_start = get_time() + (data->number_of_philosophers * 2 * 10);
	i = 0;
	if (data->nb_of_philo == 1)
	{
	    if (pthread_create(&philo[i]->thread, NULL, &deadly_routine, philo[i]) != 0)
            exit(EXIT_FAILURE);
		return ;
	}
    while (i < data->nb_of_philo)
    {
        if (pthread_create(&philo[i]->thread, NULL, &routine, philo[i]) != 0)
        {
            exit(EXIT_FAILURE);
        }
        i++;
    }
}
