/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assign.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 23:57:06 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/26 01:12:28 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

static void assign_forks(struct s_philo *philo, struct s_data_shared *data)
{
	//printf(COLOR_YELLOW "address of lock_forks: %p\n" RESET_ALL, data->lock_forks);
	philo->forks[0] = philo->id;
	//printf(COLOR_YELLOW "philo [%d] address of forks[0]: %p\n" RESET_ALL, philo->id, (void *)&philo->forks[0]);
	philo->forks[1] = (philo->id + 1) % data->nb_of_philo;
	//printf(COLOR_YELLOW "phi [%d] address of forks[1]: %p\n" RESET_ALL, phi->id, (void *)&phi->forks[1]);
	if (philo->id % 2)
	{
		philo->forks[0] = (philo->id + 1) % data->nb_of_philo;
		//printf(COLOR_YELLOW "address of forks[0]: %p\n" RESET_ALL, (void *)&philo->forks[0]);
		philo->forks[1] = philo->id;
		//printf(COLOR_YELLOW "address of forks[1]: %p\n" RESET_ALL, (void *)&philo->forks[1]);
	}
}

static void assign_struct_suite(struct s_philo *philo)
{
	philo->thread = 0;
	philo->last_plate = 0;
	philo->time_now = 0;
	philo->nb_plate_eaten = 0;
	philo->stop_flag = 0;
	philo->has_died = false;
}

void assign_struct(struct s_philo **philo, struct s_data_shared *data)
{
	unsigned int i;
	unsigned int j;

	i = 0;
	j = 0;
    while (i < data->nb_of_philo)
    {
        philo[i] = malloc(sizeof(struct s_philo));
        if(!philo[i])
        {
			while (j < i)
				free(philo[j++]);
			free(philo);
			exit(EXIT_FAILURE);
		}
		philo[i]->data = data;
		philo[i]->id = i;
		assign_struct_suite(philo[i]);
		assign_forks(philo[i], data);
        i++;
    }
}
