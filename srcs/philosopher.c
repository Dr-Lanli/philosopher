/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/06 05:01:44 by lmonsat          ###   ########.fr       */
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
	{
        pthread_mutex_lock(&data->lock_time);
		printf("philosophe[%d] has died\n", philosophe->id);
		destroy_threads(&philosophe, data);
        pthread_mutex_unlock(&data->lock_time);
        return (1);
	}
    return (0);
}
/*void *routine_died(void *arg)
{
    struct s_philosopher *philosophe;
    struct s_data_shared *data;

    philosophe = arg;
    data = philosophe->data;
    printf("checking if philosophe[%d] has died\n", philosophe->id);
    if (philosophe->time_now - philosophe->time_start > data->time_to_die)
    {
        pthread_mutex_lock(&data->lock_dead);
        philosophe->has_died = 1;
        has_died(philosophe, data);
        pthread_mutex_unlock(&data->lock_dead);
    }
    return (NULL);
}

void check_if_has_died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
    if (pthread_create(&philosophe->thread, NULL, &routine_died, philosophe) != 0)
    {
        exit(EXIT_FAILURE);
    }
}*/

void *routine(void *arg)
{
    struct s_philosopher **philosophe;
    struct s_data_shared *data;
	struct timeval tv;
    
    philosophe = arg;
    data = (*philosophe)->data;
	while (!has_died(*philosophe, data))
    {
		pthread_mutex_lock(&data->lock_time);
        gettimeofday(&tv, NULL);
		(*philosophe)->time_start = tv.tv_usec / 1000;
		pthread_mutex_unlock(&data->lock_time);
        
        pthread_mutex_lock((*philosophe)->left_f);
        printf("philosophe[%d] has taken a fork\n", (*philosophe)->id);
        pthread_mutex_lock((*philosophe)->right_f);
        printf("philosophe[%d] has taken a fork\n", (*philosophe)->id);
        (*philosophe)->forks_in_hands = 2;
        //printf("test fork: %d\n", data->forks_on_table);
        if ((*philosophe)->forks_in_hands == 2)
        {
            pthread_mutex_lock(&data->lock_eat);
            (*philosophe)->has_eaten = 1;
			gettimeofday(&tv, NULL);
			(*philosophe)->time_now = data->time_to_eat + tv.tv_usec / 1000;
            printf("philosophe[%d] eat\n", (*philosophe)->id);
            pthread_mutex_unlock(&data->lock_eat);
            pthread_mutex_unlock((*philosophe)->left_f);
            pthread_mutex_unlock((*philosophe)->right_f);
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
		
		printf("time now: %d, time start: %d, now - start: %d\n", (*philosophe)->time_now, (*philosophe)->time_start, (*philosophe)->time_now - (*philosophe)->time_start);
		printf("time to die: %d\n", data->time_to_die);
        //check_if_has_died(*philosophe, data);

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
    if (argv[5])
        data->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
    else
        data->number_of_times_each_philosopher_must_eat = 0;
}

void *monitor_philosophers(void *arg)
{
    struct s_philosopher *philosophe = arg;
    struct s_data_shared *data;
    struct timeval tv;
    int i;

    data = philosophe->data;
    while (1)
    {
        for (i = 0; i < data->number_of_philosophers; i++)
        {
            // Mettre à jour le temps actuel du philosophe
            gettimeofday(&tv, NULL);
            philosophe->time_now = tv.tv_usec / 1000;

            // Vérifier si le philosophe est mort
            if (philosophe->time_now - philosophe->time_start > data->time_to_die)
            {
                pthread_mutex_lock(&data->lock_dead);
                if (!philosophe->has_died)  // Double vérification
                {
                    philosophe->has_died = 1;
                    printf("philosophe[%d] has died\n", philosophe->id);
                    destroy_threads(&philosophe, data);
                    pthread_mutex_unlock(&data->lock_dead);
                    return NULL;
                }
                pthread_mutex_unlock(&data->lock_dead);
            }
        }
        usleep(1000);
    }
    return NULL;
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
    pthread_t monitor_thread;
    if (pthread_create(&monitor_thread, NULL, monitor_philosophers, &philosophe) != 0)
    {
        perror("Erreur lors de la création du thread moniteur");
        return EXIT_FAILURE;
    }
    // Attendre la fin du thread moniteur
    pthread_join(monitor_thread, NULL);
    printf("nb_exe_routine: %d\n", nb_exe_routine);
    destroy_threads(philosophe, &data);
    return (0);
}
