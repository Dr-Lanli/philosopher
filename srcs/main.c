/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 21:57:04 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/27 03:37:59 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

static void	check_inputs(int argc, char *argv[])
{
	if (argc < 5 || argc > 6)
	{
		printf(COLOR_RED "Error\n" COLOR_YELLOW "Usage:\n"
			"./philo [number_of_philosophers] [time_to_die]\
 [time_to_eat] [time_to_sleep] optional:\
 [number_of_times_each_philosopher_must_eat]\n" RESET_ALL);
		exit(EXIT_FAILURE);
	}
	if (ft_atoi(argv[1]) > 200)
	{
		printf(COLOR_RED "Error\n" RESET_ALL);
		exit(EXIT_FAILURE);
	}
	browse_inputs(argv);
}

static void	assign_inputs(char *argv[], struct s_data_shared *data)
{
	data->nb_of_philo = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	data->stop_flag = 0;
	data->has_died = 0;
	if (argv[5])
		data->nb_of_times_each_philo_must_eat = ft_atoi(argv[5]);
	else
		data->nb_of_times_each_philo_must_eat = 0;
}

static struct s_philo	**alloc_struct(struct s_philo **philo,
		struct s_data_shared *data)
{
	philo = malloc(data->nb_of_philo * sizeof(struct s_philo *));
	if (!philo)
		exit(EXIT_FAILURE);
	data->lock_forks = malloc(data->nb_of_philo * sizeof(pthread_mutex_t));
	if (!data->lock_forks)
	{
		free_struct(philo, data);
		exit(EXIT_FAILURE);
	}
	return (philo);
}

int	main(int argc, char *argv[])
{
	struct s_data_shared	data;
	struct s_philo			**philo;

	philo = NULL;
	check_inputs(argc, argv);
	assign_inputs(argv, &data);
	philo = alloc_struct(philo, &data);
	assign_struct(philo, &data);
	mutex_init(&data);
	create_threads(philo, &data);
	destroy_threads(philo, &data);
	destroy_mutex(&data);
	free_struct(philo, &data);
	return (0);
}
