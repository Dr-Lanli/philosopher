/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/12 19:52:34 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

int nb_exe_routine = 0;

void destroy_mutex(struct s_data_shared *data)
{
	int i;

	i = 0;
	while (i < data->number_of_philosophers)
		pthread_mutex_destroy(&data->lock_forks[i++]);
	pthread_mutex_destroy(&data->lock_eat_state);
	pthread_mutex_destroy(&data->lock_eat_assign);
    pthread_mutex_destroy(&data->lock_think_state);
	pthread_mutex_destroy(&data->lock_think_assign);
    pthread_mutex_destroy(&data->lock_sleep_state);
	pthread_mutex_destroy(&data->lock_sleep_assign);
	pthread_mutex_destroy(&data->lock_time_state);
	pthread_mutex_destroy(&data->lock_time_assign);
	pthread_mutex_destroy(&data->lock_dead_state);
	pthread_mutex_destroy(&data->lock_dead_assign);
}

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
}

void destroy_check_threads(struct s_philosopher **philosophe, struct s_data_shared *data)
{
    int i;
    
    i = 0;
    while (i < data->number_of_philosophers)
    {
		if (pthread_join(philosophe[i]->check_thread, NULL) != 0)
        {
            exit(EXIT_FAILURE);
        }
        usleep(25);
        i++;
    }
}

void routine_condition(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	pthread_mutex_lock(&data->lock_eat_state);
	if (philosophe->forks_in_hands == 2)
	{
		philosophe->has_eaten = 1;
		philosophe->nb_plate_eaten++;
		philosophe->time_start = get_time(data->time_to_eat);
		write_in_stdout(philosophe, data, "eat");
		pthread_mutex_unlock(&data->lock_forks[philosophe->forks[0]]);
		pthread_mutex_unlock(&data->lock_forks[philosophe->forks[1]]);
		philosophe->forks_in_hands = 0;
	}
	pthread_mutex_unlock(&data->lock_eat_state);
	pthread_mutex_lock(&data->lock_sleep_state);
	if (philosophe->has_eaten)
	{
		philosophe->has_slept = 1;
		//pthread_mutex_lock(&data->lock_time);
		//philosophe->time_now = get_time(data->time_to_sleep);
		//pthread_mutex_unlock(&data->lock_time);
		write_in_stdout(philosophe, data, "sleep");
	}
	pthread_mutex_unlock(&data->lock_sleep_state);
	pthread_mutex_lock(&data->lock_think_state);
	if (philosophe->has_eaten && philosophe->has_slept)
	{
		philosophe->has_thought = 1;
		write_in_stdout(philosophe, data, "think");
	}
	pthread_mutex_unlock(&data->lock_think_state);
}

void died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	//pthread_mutex_lock(&data->lock_time_state);
	if (!philosophe->has_died) 
	{ 
		philosophe->has_died = 1;
		write_in_stdout(philosophe, data, "died");
		data->stop_flag = 1;
	}
	//pthread_mutex_unlock(&data->lock_time_state);
	//destroy_threads(&philosophe, data);
}

void *routine(void *arg)
{
    struct s_philosopher *philosophe;
    struct s_data_shared *data;

    philosophe = arg;
    data = philosophe->data;
	pthread_mutex_lock(&data->lock_print);
	philosophe->time_start = get_time(0);
	pthread_mutex_unlock(&data->lock_print);
	while (!has_died(philosophe, data) && !data->stop_flag)
    {
        pthread_mutex_lock(&data->lock_forks[philosophe->forks[0]]);
        write_in_stdout(philosophe, data, "fork");
		//printf(COLOR_YELLOW "philosophe [%d] address of forks[0]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[0]);
        pthread_mutex_lock(&data->lock_forks[philosophe->forks[1]]);
        write_in_stdout(philosophe, data, "fork");
		//printf(COLOR_YELLOW "philosophe [%d] address of forks[1]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[1]);
        //printf("test fork: %d\n", data->forks_on_table);
		pthread_mutex_lock(&data->lock_eat_state);
		philosophe->forks_in_hands = 2;
		pthread_mutex_unlock(&data->lock_eat_state);

		routine_condition(philosophe, data);
		
		pthread_mutex_lock(&data->lock_print);
		philosophe->time_now = get_time(data->time_to_eat + data->time_to_sleep);
		//printf("time now: %d, time start: %d, now - start: %d\n", philosophe->time_now, philosophe->time_start, philosophe->time_now - philosophe->time_start);
		//printf("time to die: %d\n", data->time_to_die);
		pthread_mutex_unlock(&data->lock_print);

		//pthread_mutex_lock(&data->lock_time_state);
		if (philosophe->time_now - philosophe->time_start > data->time_to_die)
		{
			died(philosophe, data);
			data->stop_flag = 1;
		}
		//pthread_mutex_unlock(&data->lock_time_state);
		//printf("plate eaten: %d\n", philosophe->nb_plate_eaten);
		//printf("must eat: %d\n", data->number_of_times_each_philosopher_must_eat);

		pthread_mutex_lock(&data->lock_dead_state);
		if (data->number_of_times_each_philosopher_must_eat)
		{
			pthread_mutex_lock(&data->lock_dead_assign);
			if (philosophe->nb_plate_eaten >= data->number_of_times_each_philosopher_must_eat)
				{
					pthread_mutex_unlock(&data->lock_dead_assign);
					pthread_mutex_unlock(&data->lock_dead_state);
					return (NULL);
				}
			pthread_mutex_unlock(&data->lock_dead_assign);
		}
		pthread_mutex_unlock(&data->lock_dead_state);
        //nb_exe_routine++;
        usleep(100);
    }
    return (NULL);
}

/*void *is_alive_routine_old(void *arg)
{
    struct s_philosopher *philosophe = arg;
    struct s_data_shared *data;
	
    data = philosophe->data;
    while (1)
    {
		pthread_mutex_lock(&data->lock_dead_state);
		if (philosophe->has_died)
		{
			pthread_mutex_lock(&data->lock_dead_assign);
			printf("{%u ms} philosophe[%d] has died\n", get_time(0), philosophe->id);
			philosophe->has_died = 0;
			pthread_mutex_unlock(&data->lock_dead_assign);
			destroy_threads(&philosophe, data);
			return (NULL);
		}
		pthread_mutex_unlock(&data->lock_dead_state);
		pthread_mutex_lock(&data->lock_dead_state);
		if (philosophe->nb_plate_eaten >= data->number_of_times_each_philosopher_must_eat)
		{
            //pthread_join(philosophe->thread, NULL);
			destroy_threads(&philosophe, data);
            //pthread_join(philosophe->check_thread, NULL);
		}
		pthread_mutex_unlock(&data->lock_dead_state);
        usleep(25);
    }
    return (NULL);
}*/

void *is_alive_routine(void *arg)
{
	struct s_philosopher *philosophe;
    struct s_data_shared *data;
    int finish;
    int i;

	philosophe = arg;
	data = philosophe->data;
	i = 0;
    while (1)
    {
        finish = 1;
        while (i < data->number_of_philosophers)
        {
            if (philosophe[i].nb_plate_eaten >= data->number_of_times_each_philosopher_must_eat)
            {
				//printf("philosophe[%d] finish eaten\n", i);
                finish = 0;
                break;
            }
			pthread_mutex_lock(&data->lock_dead_state);
			if (philosophe->has_died)
			{
				has_died(philosophe, data);
			}
			pthread_mutex_unlock(&data->lock_dead_state);
        }
        if (finish)
        {
            pthread_mutex_lock(&data->lock_dead_assign);
            data->stop_flag = 1;
            pthread_mutex_unlock(&data->lock_dead_assign);
            break;
        }
        usleep(100);
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
		if (!philosophe->forks_in_hands)
		{
			//printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(0), philosophe->id);
			philosophe->forks_in_hands = 1;
		}
		routine_condition(philosophe, data);
		pthread_mutex_lock(&data->lock_time_assign);
		philosophe->time_now = get_time(data->time_to_eat + data->time_to_sleep);
		//printf("time now: %d, time start: %d, now - start: %d\n", philosophe->time_now, philosophe->time_start, philosophe->time_now - philosophe->time_start);
		//printf("time to die: %d\n", data->time_to_die);
		pthread_mutex_unlock(&data->lock_time_assign);
		pthread_mutex_lock(&data->lock_dead_state);
		if (philosophe->time_now - philosophe->time_start > data->time_to_die)
		{
			pthread_mutex_lock(&data->lock_dead_assign);
			philosophe->has_died = 1;
			pthread_mutex_unlock(&data->lock_dead_assign);
			//printf("{%u ms} philosophe[%d] has died\n", get_time(0), philosophe->id);
			pthread_mutex_lock(&data->lock_dead_assign);
			philosophe->has_died = 0;
			pthread_mutex_unlock(&data->lock_dead_assign);
			//destroy_threads(&philosophe, data);
			return (NULL);
		}
		pthread_mutex_unlock(&data->lock_dead_state);
	}
	return (NULL);
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
		//philosophe[i]->id = i;
        if (pthread_create(&philosophe[i]->thread, NULL, &routine, philosophe[i]) != 0)
        {
            exit(EXIT_FAILURE);
        }
		/*if (pthread_create(&philosophe[i]->check_thread, NULL, &is_alive_routine_old, philosophe[i]) != 0)
		{
			exit(EXIT_FAILURE);
		}*/
		//printf("philosophe[%d] has been created\n", philosophe[i]->id);
       	usleep(100);
        i++;
    }
}

void assign_forks(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	//printf(COLOR_YELLOW "address of lock_forks: %p\n" RESET_ALL, data->lock_forks);
	philosophe->forks[0] = philosophe->id;
	printf(COLOR_YELLOW "philosophe [%d] address of forks[0]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[0]);
	philosophe->forks[1] = (philosophe->id + 1) % data->number_of_philosophers;
	printf(COLOR_YELLOW "philosophe [%d] address of forks[1]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[1]);
	if (philosophe->id % 2)
	{
		philosophe->forks[0] = (philosophe->id + 1) % data->number_of_philosophers;
		//printf(COLOR_YELLOW "address of forks[0]: %p\n" RESET_ALL, (void *)&philosophe->forks[0]);
		philosophe->forks[1] = philosophe->id;
		//printf(COLOR_YELLOW "address of forks[1]: %p\n" RESET_ALL, (void *)&philosophe->forks[1]);
	}

	/*philosophe->forks[0] = &data->lock_forks[philosophe->id];
	printf(COLOR_YELLOW "philosophe [%d] address of forks[0]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[0]);
	philosophe->forks[1] = &data->lock_forks[philosophe->id + 1 % data->number_of_philosophers];
	printf(COLOR_YELLOW "philosophe [%d] address of forks[1]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[1]);
	if (philosophe->id % 2)
	{
		philosophe->forks[0] = &data->lock_forks[(philosophe->id + 1) % data->number_of_philosophers];
		//printf(COLOR_YELLOW "address of forks[0]: %p\n" RESET_ALL, (void *)&philosophe->forks[0]);
		philosophe->forks[1] = &data->lock_forks[philosophe->id];
		//printf(COLOR_YELLOW "address of forks[1]: %p\n" RESET_ALL, (void *)&philosophe->forks[1]);
	}*/
}

void assign_struct_suite(struct s_philosopher *philosophe)
{
	philosophe->forks_in_hands = 0;
	philosophe->thread = 0;
	philosophe->time_start = 0;
	philosophe->time_now = 0;
	philosophe->has_eaten = 0;
	philosophe->has_thought = 0;
	philosophe->has_slept = 0;
	philosophe->has_died = 0;
	philosophe->nb_plate_eaten = 0;
	philosophe->stop_flag = 0;
}

void assign_struct(struct s_philosopher **philosophe, struct s_data_shared *data)
{
	int i;
	int j;

	i = 0;
	j = 0;
    while (i < data->number_of_philosophers)
    {
        philosophe[i] = malloc(sizeof(struct s_philosopher));
        if(!philosophe[i])
        {
			while (j < i)
				free(philosophe[j++]);
			free(philosophe);
			exit(EXIT_FAILURE);
		}
		philosophe[i]->data = data;
		philosophe[i]->id = i;
		assign_struct_suite(philosophe[i]);
		assign_forks(philosophe[i], data);
        i++;
    }
}

void browse_inputs(char *argv[])
{
	char *current_arg;
	argv++;
	while (*argv)
	{
		if (!is_valid_int(*argv))
		{
			printf("Error: arg not an int\n");
			exit(EXIT_FAILURE);
		}
		current_arg = *argv;
		while (*current_arg)
		{
			if (*current_arg < '0' || *current_arg > '9')
			{
				printf("Error: arg not an int\n");
				exit(EXIT_FAILURE);
			}
			current_arg++;
		}
		argv++;
	}
}

void check_inputs(int argc, char *argv[])
{
	if (argc < 5 || argc > 6)
    {
        printf(COLOR_RED "Error\n" COLOR_YELLOW "Usage:\n" "./philosopher [number_of_philosophers] [time_to_die] [time_to_eat] [time_to_sleep] optional: [number_of_times_each_philosopher_must_eat]\n" RESET_ALL);
        exit(EXIT_FAILURE);
    }
	browse_inputs(argv);
	if (ft_atoi(argv[1]) >= 200)
	{
		printf("Error: too many philosophers\n");
		exit(EXIT_FAILURE);
	}
	else if (ft_atoi(argv[2]) < 200 || ft_atoi(argv[3]) < 100 || ft_atoi(argv[4]) < 100)
	{
		printf("Error: Not enough time\n");
		exit(EXIT_FAILURE);
	}
}

void assign_inputs(int argc, char *argv[], struct s_data_shared *data)
{
    data->number_of_philosophers = ft_atoi(argv[1]);
    data->time_to_die = ft_atoi(argv[2]);
    data->time_to_eat = ft_atoi(argv[3]);
    data->time_to_sleep = ft_atoi(argv[4]);
	data->stop_flag = 0;
    if (argv[5])
        data->number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
    else
        data->number_of_times_each_philosopher_must_eat = 0;
}

void mutex_init(struct s_data_shared *data)
{
	int i;

	i = 0;
	pthread_mutex_init(&data->lock_eat_state, NULL);
	pthread_mutex_init(&data->lock_eat_assign, NULL);
    pthread_mutex_init(&data->lock_think_state, NULL);
	pthread_mutex_init(&data->lock_think_assign, NULL);
    pthread_mutex_init(&data->lock_sleep_state, NULL);
	pthread_mutex_init(&data->lock_sleep_assign, NULL);
    pthread_mutex_init(&data->lock_time_state, NULL);
	pthread_mutex_init(&data->lock_time_assign, NULL);
	pthread_mutex_init(&data->lock_dead_state, NULL);
	pthread_mutex_init(&data->lock_dead_assign, NULL);
	pthread_mutex_init(&data->lock_print, NULL);
	while (i < data->number_of_philosophers)
	{
		pthread_mutex_init(&data->lock_forks[i], NULL);
		i++;
	}
}

struct s_philosopher **alloc_struct(struct s_philosopher **philosophe, struct s_data_shared *data)
{
	philosophe = malloc(data->number_of_philosophers * sizeof(struct s_philosopher *));
    if(!philosophe)
        exit(EXIT_FAILURE);
	data->lock_forks = malloc(data->number_of_philosophers * sizeof(pthread_mutex_t));
	if (!data->lock_forks) 
	{
		free_struct(philosophe, data);
		exit(EXIT_FAILURE);
	}
	return (philosophe);
}
/* vérifier l'adresse des fourchettes
	print console après que un philosopher ait mourru
	retry avec les check_thread is_alive et is_alive_old */
int main (int argc, char *argv[])
{
    struct s_data_shared data;
    struct s_philosopher **philosophe;
	
	check_inputs(argc, argv);
    assign_inputs(argc, argv, &data);
	philosophe = alloc_struct(philosophe, &data);
    assign_struct(philosophe, &data);
	mutex_init(&data);
    create_threads(philosophe, &data);
    //printf("nb_exe_routine: %d\n", nb_exe_routine);
    destroy_threads(philosophe, &data);
	//destroy_check_threads(philosophe, &data);
	destroy_mutex(&data);
	free_struct(philosophe, &data);
	//printf("free executed\n");
    return (0);
}
