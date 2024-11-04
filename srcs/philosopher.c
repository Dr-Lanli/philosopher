/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/04 22:14:46 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

int nb_exe_routine = 0;

void destroy_threads(struct s_philosopher *value)
{
    int i;
    
    i = 0;
    while (i < value->number_of_philosophers)
    {
        if (pthread_join(value->philosophe[i]->thread, NULL) != 0)
        {
            exit(EXIT_FAILURE);
        }
        usleep(25);
        i++;
    }
	pthread_mutex_destroy(&value->lock_eat);
    pthread_mutex_destroy(&value->lock_think);
    pthread_mutex_destroy(&value->lock_sleep);
	pthread_mutex_destroy(&value->lock_time);
	pthread_mutex_destroy(&value->lock_dead);
}

void has_died(struct s_philosopher *value)
{
	if (value->has_died)
	{
		printf("philosophe[%d] has died\n", value->id);
		destroy_threads(value);
	}
}

void *routine(void *arg)
{
    struct s_philosopher *value;
	struct s_philosopher *reset_status;
	struct timeval tv;

	//int id = value->id;
    
    value = arg;
	reset_status = value;
	while (1)
    {
		pthread_mutex_lock(&value->lock_time);
		gettimeofday(&tv, NULL);
		value->philosophe[value->id]->time_start = tv.tv_usec / 1000;
		pthread_mutex_unlock(&value->lock_time);
        has_died(value);
        if (value->forks >= 2)
        {
            value->forks -= 2;
            pthread_mutex_lock(&value->lock_eat);
            value->philosophe[value->id]->has_eaten = 1;
			gettimeofday(&tv, NULL);
			value->philosophe[value->id]->time_now = value->time_to_eat + tv.tv_usec / 1000;
            printf("philosophe[%d] eat\n", value->id);
            pthread_mutex_unlock(&value->lock_eat);
            value->forks += 2;
        }
        if (value->philosophe[value->id]->has_eaten)
        {
            pthread_mutex_lock(&value->lock_think);
            value->philosophe[value->id]->has_thought = 1;
            printf("philosophe[%d] think\n", value->id);
            pthread_mutex_unlock(&value->lock_think);
        }
        if (value->philosophe[value->id]->has_eaten && value->philosophe[value->id]->has_thought)
        {
            pthread_mutex_lock(&value->lock_sleep);
            value->philosophe[value->id]->has_slept = 1;
			gettimeofday(&tv, NULL);
			value->philosophe[value->id]->time_now = value->time_to_sleep + tv.tv_usec / 1000;
            printf("philosophe[%d] sleep\n", value->id);
            pthread_mutex_unlock(&value->lock_sleep);
        }
        has_died(value);
		
		printf("time now: %d, time start: %d, now - start: %d\n", value->philosophe[value->id]->time_now, value->philosophe[value->id]->time_start, value->philosophe[value->id]->time_now - value->philosophe[value->id]->time_start);
		printf("time to die: %d\n", value->time_to_die);
        if (value->philosophe[value->id]->time_now - value->philosophe[value->id]->time_start > value->time_to_die)
        {
			pthread_mutex_lock(&value->lock_dead);
            value->philosophe[value->id]->has_died = 1;
            has_died(value);
            break;
			pthread_mutex_unlock(&value->lock_dead);
        }
       //usleep(10);
		pthread_mutex_lock(&value->lock_time);
        value->philosophe[value->id]->time_start = tv.tv_usec / 1000;
		gettimeofday(&tv, NULL);
		value->philosophe[value->id]->time_now = tv.tv_usec / 1000;
		pthread_mutex_unlock(&value->lock_time);
		nb_exe_routine++;
    }
    return (NULL);
}

void create_threads(struct s_philosopher *value)
{
    int i;
	
    i = 0;
    while (i < value->number_of_philosophers)
    {
		value->philosophe[i]->id = i;
        if (pthread_create(&value->philosophe[i]->thread, NULL, &routine, value) != 0)
        {
            exit(EXIT_FAILURE);
        }
       	usleep(25);
        i++;
    }
}

void assign_inputs(int argc, char *argv[], struct s_philosopher *value)
{
	int i;

	i = 0;
    if (argc < 5 || argc > 6)
    {
        printf(COLOR_RED "Error\n" COLOR_YELLOW "Usage:\n" "./philosopher [number_of_philosophers] [time_to_die] [time_to_eat] [time_to_sleep] optional: [number_of_times_each_philosopher_must_eat]\n" RESET_ALL);
        exit(EXIT_FAILURE);
    }
    value->number_of_philosophers = ft_atoi(argv[1]);
    value->forks = value->number_of_philosophers;
    value->time_to_die = ft_atoi(argv[2]);
    value->time_to_eat = ft_atoi(argv[3]);
    value->time_to_sleep = ft_atoi(argv[4]);
    value->has_eaten = 0;
    value->has_thought = 0;
    value->has_slept = 0;
	value->has_died = 0;
	value->thread_created = 0;
    if (argv[5])
        value->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
    else
        value->number_of_times_each_philosopher_must_eat = 0;
	while (i < value->number_of_philosophers)
		value->philosophe[i++] = value;
}

int main (int argc, char *argv[])
{
    struct s_philosopher value;
	struct s_philosopher *ptr;
    
    pthread_mutex_init(&value.lock_eat, NULL);
    pthread_mutex_init(&value.lock_think, NULL);
    pthread_mutex_init(&value.lock_sleep, NULL);
    pthread_mutex_init(&value.lock_time, NULL);
	pthread_mutex_init(&value.lock_dead, NULL);
	
    assign_inputs(argc, argv, &value);
    printf("test main: %d\n", value.number_of_philosophers);
    create_threads(&value);
    printf("nb_exe_routine: %d\n", nb_exe_routine);
    //destroy_threads(&value);

    return (0);
}
