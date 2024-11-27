/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   assign.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 23:57:06 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/27 20:24:34 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

/* Assigne les fourchettes aux philos
	philo pair = fork[0] = fourchette gauche (sa fourchette)
				= fork[1] = fourchette droite (fourchette du prochain)
	philo impair = fork[0] = fourchette droite
					= fork[1] = fourchette gauche
	permet d'éviter l'acces aux même fourchettes.
	Le modulo du nb_philo permet de boucler autour de la table des philos
	printf("Philo [%d] holding forks: [%d, %d]\n",
		philo->id, philo->forks[0], philo->forks[1]);
*/
static void	assign_forks(struct s_philo *philo, struct s_data_shared *data)
{
	philo->forks[0] = philo->id;
	philo->forks[1] = (philo->id + 1) % data->nb_of_philo;
	if (philo->id % 2)
	{
		philo->forks[0] = (philo->id + 1) % data->nb_of_philo;
		philo->forks[1] = philo->id;
	}
}

static void	assign_struct_suite(struct s_philo *philo)
{
	philo->thread = 0;
	philo->last_plate = 0;
	philo->time_now = 0;
	philo->nb_plate_eaten = 0;
	philo->stop_flag = 0;
	philo->has_died = false;
}

void	assign_struct(struct s_philo **philo, struct s_data_shared *data)
{
	unsigned int	i;
	unsigned int	j;

	i = 0;
	j = 0;
	while (i < data->nb_of_philo)
	{
		philo[i] = malloc(sizeof(struct s_philo));
		if (!philo[i])
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
