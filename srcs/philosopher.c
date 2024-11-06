/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/07 00:05:25 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

int nb_exe_routine = 0;

void destroy_threads(struct s_philosopher **philosophe, struct s_data_shared *data)
{
    int i;
    
    i = 0;
    while (i < data->number_of_philosophers)
    {
        if (pthread_join(philosophe[i]->thread, NULL) != 0)
        {
            exit(EXIT_FAILURE);
        }
		if (pthread_join(philosophe[i]->check_thread, NULL) != 0)
        {
            exit(EXIT_FAILURE);
        }
        pthread_mutex_destroy(&data->forks[i]);
        usleep(25);
        i++;
    }
	pthread_mutex_destroy(&data->lock_eat);
    pthread_mutex_destroy(&data->lock_think);
    pthread_mutex_destroy(&data->lock_sleep);
	pthread_mutex_destroy(&data->lock_time);
	pthread_mutex_destroy(&data->lock_dead);
}

int has_died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	if (philosophe->has_died)
        return (1);
    return (0);
}

void routine_condition(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	pthread_mutex_lock(&data->lock_eat);
	if (philosophe->forks_in_hands == 2)
	{
		philosophe->has_eaten = 1;
		pthread_mutex_lock(&data->lock_time);
		philosophe->time_start = get_time(data->time_to_eat);
		pthread_mutex_unlock(&data->lock_time);
		printf("{%u ms} philosophe[%d] eat\n", get_time(0), philosophe->id);
		//pthread_mutex_unlock(philosophe->left_f);
		//pthread_mutex_unlock(philosophe->right_f);
	}
	pthread_mutex_unlock(&data->lock_eat);
	pthread_mutex_lock(&data->lock_sleep);
	if (philosophe->has_eaten)
	{
		philosophe->has_slept = 1;
		philosophe->nb_plate_eaten++;
		//pthread_mutex_lock(&data->lock_time);
		//philosophe->time_now = get_time(data->time_to_sleep);
		//pthread_mutex_unlock(&data->lock_time);
		printf("{%u ms} philosophe[%d] sleep\n", get_time(0), data->id);
	}
	pthread_mutex_unlock(&data->lock_sleep);
	pthread_mutex_lock(&data->lock_think);
	if (philosophe->has_eaten && philosophe->has_slept)
	{
		philosophe->has_thought = 1;
		printf("{%u ms} philosophe[%d] think\n", get_time(0), philosophe->id);
	}
	pthread_mutex_unlock(&data->lock_think);
}

void died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	pthread_mutex_lock(&data->lock_dead);
	philosophe->has_died = 1;
	printf("{%u ms} philosophe[%d] has died\n", get_time(0), philosophe->id);
	philosophe->has_died = 0;
	pthread_mutex_unlock(&data->lock_dead);
	destroy_threads(&philosophe, data);
}

void *routine(void *arg)
{
    struct s_philosopher *philosophe;
    struct s_data_shared *data;

    philosophe = arg;
    data = philosophe->data;
	philosophe->time_start = get_time(0);
	while (!has_died(philosophe, data) && !data->stop_flag)
    {
        //pthread_mutex_lock(philosophe->left_f);
        printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(0), philosophe->id);
        //pthread_mutex_lock(philosophe->right_f);
        printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(0), philosophe->id);
        philosophe->forks_in_hands = 2;
        //printf("test fork: %d\n", data->forks_on_table);

		routine_condition(philosophe, data);
		
		pthread_mutex_lock(&data->lock_time);
		philosophe->time_now = get_time(data->time_to_eat + data->time_to_sleep);
		printf("time now: %d, time start: %d, now - start: %d\n", philosophe->time_now, philosophe->time_start, philosophe->time_now - philosophe->time_start);
		printf("time to die: %d\n", data->time_to_die);
		if (philosophe->time_now - philosophe->time_start > data->time_to_die)
		{
			died(philosophe, data);
		}
		if (philosophe->nb_plate_eaten == data->number_of_times_each_philosopher_must_eat)
			data->stop_flag = 1;
		
		pthread_mutex_unlock(&data->lock_time);
        nb_exe_routine++;
        usleep(25);
    }
    return (NULL);
}

void *is_alive_routine(void *arg)
{
    struct s_philosopher *philosophe = arg;
    struct s_data_shared *data;

    data = philosophe->data;
    while (1)
    {
		pthread_mutex_lock(&data->lock_dead);
		if (philosophe->has_died)
		{
			printf("{%u ms} philosophe[%d] has died\n", get_time(0), philosophe->id);
			philosophe->has_died = 0;
			pthread_mutex_unlock(&data->lock_dead);
			destroy_threads(&philosophe, data);
			return (NULL);
		}
		pthread_mutex_unlock(&data->lock_dead);
		if (philosophe->nb_plate_eaten == data->number_of_times_each_philosopher_must_eat)
		{
            pthread_join(philosophe->thread, NULL);

            pthread_join(philosophe->check_thread, NULL);
		}

        usleep(25);
    }
    return (NULL);
}

void *deadly_routine(void *arg)
{
	struct s_philosopher *philosophe;
	struct s_data_shared *data;

	philosophe = arg;
	data = philosophe->data;

	philosophe->time_start = get_time(0);
	while(!has_died(philosophe, data))
	{
		//pthread_mutex_lock(&data->lock_time);
		//philosophe->time_start = get_time(0);
		//pthread_mutex_unlock(&data->lock_time);

		 //pthread_mutex_lock(philosophe->left_f);
		if (!philosophe->forks_in_hands)
		{
		    printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(0), philosophe->id);
        	philosophe->forks_in_hands = 1;
		}
		routine_condition(philosophe, data);
		
		pthread_mutex_lock(&data->lock_time);
		philosophe->time_now = get_time(data->time_to_eat + data->time_to_sleep);
		//printf("time now: %d, time start: %d, now - start: %d\n", philosophe->time_now, philosophe->time_start, philosophe->time_now - philosophe->time_start);
		//printf("time to die: %d\n", data->time_to_die);
		if (philosophe->time_now - philosophe->time_start > data->time_to_die)
		{
			pthread_mutex_lock(&data->lock_dead);
			philosophe->has_died = 1;
			printf("{%u ms} philosophe[%d] has died\n", get_time(0), philosophe->id);
			philosophe->has_died = 0;
			pthread_mutex_unlock(&data->lock_dead);
			destroy_threads(&philosophe, data);
		}
		pthread_mutex_unlock(&data->lock_time);
	}

}

void create_threads(struct s_philosopher **philosophe, struct s_data_shared *data)
{
    int i;

	i = 0;
	if (data->number_of_philosophers == 1)
	{
	    if (pthread_create(&philosophe[i]->thread, NULL, &deadly_routine, philosophe[i]) != 0)
            exit(EXIT_FAILURE);
		return ;
	}
    while (i < data->number_of_philosophers)
    {
		philosophe[i]->id = i;
        if (pthread_create(&philosophe[i]->thread, NULL, &routine, philosophe[i]) != 0)
        {
            exit(EXIT_FAILURE);
        }
		if (pthread_create(&philosophe[i]->check_thread, NULL, &is_alive_routine, philosophe[i]) != 0)
		{
			exit(EXIT_FAILURE);
		}
		printf("philosophe[%d] has been created\n", philosophe[i]->id);
       	usleep(250);
        i++;
    }
}

void assign_struct(struct s_philosopher **philosophe, struct s_data_shared *data)
{
	int i;

	i = 0;
    while (i < data->number_of_philosophers)
    {
        philosophe[i] = malloc(sizeof(struct s_philosopher));
        if(!philosophe[i])
        {
            exit(EXIT_FAILURE);
        }
        data->forks = malloc(data->number_of_philosophers * sizeof(pthread_mutex_t));
        if (!data->forks) 
        {
            perror("Erreur d'allocation de forks");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_init(&data->forks[i], NULL);
        philosophe[i]->data = data;
        philosophe[i]->thread = 0;
        philosophe[i]->id = i;
        philosophe[i]->time_start = 0;
        philosophe[i]->time_now = 0;
        philosophe[i]->has_eaten = 0;
        philosophe[i]->has_thought = 0;
        philosophe[i]->has_slept = 0;
        philosophe[i]->has_died = 0;
		philosophe[i]->nb_plate_eaten = 0;
        philosophe[i]->left_f = &data->forks[i];
        philosophe[i]->right_f = &data->forks[(i + 1) % data->number_of_philosophers];
        i++;
    }
}

void assign_inputs(int argc, char *argv[], struct s_data_shared *data)
{
    if (argc < 5 || argc > 6)
    {
        printf(COLOR_RED "Error\n" COLOR_YELLOW "Usage:\n" "./philosopher [number_of_philosophers] [time_to_die] [time_to_eat] [time_to_sleep] optional: [number_of_times_each_philosopher_must_eat]\n" RESET_ALL);
        exit(EXIT_FAILURE);
    }
    data->number_of_philosophers = ft_atoi(argv[1]);
    data->total_forks = data->number_of_philosophers;
    data->time_to_die = ft_atoi(argv[2]);
    data->time_to_eat = ft_atoi(argv[3]);
    data->time_to_sleep = ft_atoi(argv[4]);
	data->stop_flag = 0;
    if (argv[5])
        data->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
    else
        data->number_of_times_each_philosopher_must_eat = 0;
}
/* integrer nb_plate et gérer les flag */
int main (int argc, char *argv[])
{
    struct s_data_shared data;
    struct s_philosopher **philosophe;
    
    pthread_mutex_init(&data.lock_eat, NULL);
    pthread_mutex_init(&data.lock_think, NULL);
    pthread_mutex_init(&data.lock_sleep, NULL);
    pthread_mutex_init(&data.lock_time, NULL);
	pthread_mutex_init(&data.lock_dead, NULL);
	
    assign_inputs(argc, argv, &data);
    philosophe = malloc(data.number_of_philosophers * sizeof(struct s_philosopher *));
    if(!philosophe)
        return (0);
    assign_struct(philosophe, &data);
    printf("test main: %d\n", data.number_of_philosophers);
    create_threads(philosophe, &data);
    printf("nb_exe_routine: %d\n", nb_exe_routine);
    destroy_threads(philosophe, &data);
    return (0);
}
