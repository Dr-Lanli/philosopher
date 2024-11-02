/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/02 21:11:58 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

int nb_exe_routine = 0;

void *routine(void *arg)
{
    struct s_philosopher *value;
    
    value = arg;
    if (value->forks >= 2)
    {
        pthread_mutex_lock(&value->lock_eat);
        value->has_eaten = 1;
        printf("test eat\n");
        pthread_mutex_unlock(&value->lock_eat);
    }
    if (value->has_eaten)
    {
        pthread_mutex_lock(&value->lock_think);
        value->has_thought = 1;
        printf("test think\n");
        pthread_mutex_unlock(&value->lock_think);
    }
    if (value->has_eaten && value->has_thought)
    {
        pthread_mutex_lock(&value->lock_sleep);
        value->has_slept = 1;
        printf("test sleep\n");
        pthread_mutex_unlock(&value->lock_sleep);
    }
    nb_exe_routine++;
    printf("test routine: %d\n", nb_exe_routine);
    return (NULL);
    
}

void create_threads(pthread_t th[5], struct s_philosopher *value)
{
    int i;

    i = 0;
    while (i < 5)
    {
        if (pthread_create(&th[i], NULL, &routine, value) != 0)
        {
            exit(EXIT_FAILURE);
        }
       usleep(25);
        i++;
    }
}

void destroy_threads(pthread_t th[5])
{
    int i;
    
    i = 0;
    while (i < 5)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            exit(EXIT_FAILURE);
        }
        usleep(25);
        i++;
    }
}

void assign_inputs(int argc, char *argv[], struct s_philosopher *value)
{
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
    if (argv[5])
        value->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
    else
        value->number_of_times_each_philosopher_must_eat = 0;
}

int main (int argc, char *argv[])
{
    struct s_philosopher value;
    pthread_t th[5];
    
    pthread_mutex_init(&value.lock_eat, NULL);
    pthread_mutex_init(&value.lock_think, NULL);
    pthread_mutex_init(&value.lock_sleep, NULL);
    
    assign_inputs(argc, argv, &value);
    printf("test main: %d\n", value.number_of_philosophers);
    create_threads(th, &value);
    printf("nb_exe_routine: %d\n", nb_exe_routine);
    destroy_threads(th);

    pthread_mutex_destroy(&value.lock_eat);
    pthread_mutex_destroy(&value.lock_think);
    pthread_mutex_destroy(&value.lock_sleep);

    return (0);
}
