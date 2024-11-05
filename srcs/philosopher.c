/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/05 22:06:13 by lmonsat          ###   ########.fr       */
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
        usleep(25);
        i++;
    }
	pthread_mutex_destroy(&data->lock_eat);
    pthread_mutex_destroy(&data->lock_think);
    pthread_mutex_destroy(&data->lock_sleep);
	pthread_mutex_destroy(&data->lock_time);
	pthread_mutex_destroy(&data->lock_dead);
}

void has_died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	if (philosophe->has_died)
	{
        pthread_mutex_lock(&data->lock_time);
		printf("philosophe[%d] has died\n", philosophe->id);
		destroy_threads(&philosophe, data);
        pthread_mutex_unlock(&data->lock_time);
	}
}

void *routine(void *arg)
{
    struct s_philosopher **philosophe;
    struct s_data_shared *data;
	struct timeval tv;
    
    philosophe = arg;
    data = (*philosophe)->data;
	while (1)
    {
		pthread_mutex_lock(&data->lock_time);
        gettimeofday(&tv, NULL);
		(*philosophe)->time_start = tv.tv_usec / 1000;
		pthread_mutex_unlock(&data->lock_time);
        
        has_died(*philosophe, data);
        printf("test fork: %d\n", data->forks_on_table);
        if (data->forks_on_table)
        {
            printf("test fork philo: %d\n", (*philosophe)->id);
        }
        if (data->forks_on_table >= 2)
        {
            pthread_mutex_lock(&data->lock_eat);
            data->forks_on_table -= 2;
            (*philosophe)->has_eaten = 1;
			gettimeofday(&tv, NULL);
			(*philosophe)->time_now = data->time_to_eat + tv.tv_usec / 1000;
            printf("philosophe[%d] eat\n", (*philosophe)->id);
            pthread_mutex_unlock(&data->lock_eat);
            data->forks_on_table += 2;
        }
        if ((*philosophe)->has_eaten)
        {
            pthread_mutex_lock(&data->lock_sleep);
            (*philosophe)->has_slept = 1;
			gettimeofday(&tv, NULL);
			(*philosophe)->time_now = data->time_to_sleep + tv.tv_usec / 1000;
            printf("philosophe[%d] sleep\n", data->id);
            pthread_mutex_unlock(&data->lock_sleep);

        }
        if ((*philosophe)->has_eaten && (*philosophe)->has_slept)
        {
            pthread_mutex_lock(&data->lock_think);
            (*philosophe)->has_thought = 1;
            printf("philosophe[%d] think\n", (*philosophe)->id);
            pthread_mutex_unlock(&data->lock_think);
        }
        has_died(*philosophe, data);
		
		printf("time now: %d, time start: %d, now - start: %d\n", (*philosophe)->time_now, (*philosophe)->time_start, (*philosophe)->time_now - (*philosophe)->time_start);
		printf("time to die: %d\n", data->time_to_die);
        if ((*philosophe)->time_now - (*philosophe)->time_start > data->time_to_die)
        {
			pthread_mutex_lock(&data->lock_dead);
            (*philosophe)->has_died = 1;
            has_died((*philosophe), data);
            break;
			pthread_mutex_unlock(&data->lock_dead);
        }

		pthread_mutex_lock(&data->lock_time);
        (*philosophe)->time_start = tv.tv_usec / 1000;
		gettimeofday(&tv, NULL);
		(*philosophe)->time_now = tv.tv_usec / 1000;
		pthread_mutex_unlock(&data->lock_time);
        
		nb_exe_routine++;
        usleep(250);
    }
    return (NULL);
}

void create_threads(struct s_philosopher **philosophe, struct s_data_shared *data)
{
    int i;
	
    i = 0;
    while (i < data->number_of_philosophers)
    {
		//philosophe[i]->id = i;
        if (pthread_create(&philosophe[i]->thread, NULL, &routine, philosophe) != 0)
        {
            exit(EXIT_FAILURE);
        }
       	usleep(25);
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
        philosophe[i]->data = data;
        philosophe[i]->thread = 0;
        philosophe[i]->id = i;
        philosophe[i]->forks = 0;
        philosophe[i]->time_start = 0;
        philosophe[i]->time_now = 0;
        philosophe[i]->has_eaten = 0;
        philosophe[i]->has_thought = 0;
        philosophe[i]->has_slept = 0;
        philosophe[i]->has_died = 0;
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
    data->forks_on_table = data->number_of_philosophers;
    data->time_to_die = ft_atoi(argv[2]);
    data->time_to_eat = ft_atoi(argv[3]);
    data->time_to_sleep = ft_atoi(argv[4]);
    if (argv[5])
        data->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
    else
        data->number_of_times_each_philosopher_must_eat = 0;
}

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
