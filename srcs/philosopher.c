/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/19 23:22:09 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

void sim_start_delay(uint32_t time_start)
{
	while (get_time() < time_start)
		continue ;
}

void died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	if (!philosophe->has_died) 
	{ 
		philosophe->has_died = 1;
		write_in_stdout(philosophe, data, "died");
		data->stop_flag = 1;
	}
	//destroy_threads(&philosophe, data);
}

void check_died(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	uint32_t time_now;
	
	time_now = get_time();
	if (philosophe->last_plate)
	{
		printf("philosophe[%d], now - last_plate: %d\n", philosophe->id, time_now - philosophe->last_plate);
		if ((time_now - philosophe->last_plate) > data->time_to_die)
		{
			died(philosophe, data);
		}
	}
}

void psleep(struct s_data_shared *data, uint32_t sleep_time)
{
	uint32_t	wake_up;

	wake_up = get_time() + sleep_time;
	while (get_time() < wake_up)
	{
		if (data->stop_flag)
			break;
		usleep(100);
	}
}


void think_routine(struct s_philosopher *philosophe, struct s_data_shared *data, unsigned int silent)
{
	uint32_t	time_to_think;

	pthread_mutex_lock(&data->lock_eat_state);
	time_to_think = (data->time_to_die - (get_time() - philosophe->last_plate) - data->time_to_eat) / 2;
	pthread_mutex_unlock(&data->lock_eat_state);
	if (time_to_think < 0)
		time_to_think = 0;
	if (time_to_think == 0 && silent == 1)
		time_to_think = 1;
	if (time_to_think > 600)
		time_to_think = 200;
	if (silent == 0)
		write_in_stdout(philosophe, data, "think");
	printf("test time to sleep: %d\n", time_to_think);
	psleep(data, time_to_think);
}


void routine_condition(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	write_in_stdout(philosophe, data, "eat");
	pthread_mutex_lock(&data->lock_eat_state);
	
		philosophe->nb_plate_eaten++;
		philosophe->last_plate = get_time();
	pthread_mutex_unlock(&data->lock_eat_state);
	//usleep(data->time_to_eat * 1000);
	psleep(data, data->time_to_eat);
	pthread_mutex_unlock(&data->lock_forks[philosophe->forks[0]]);
	pthread_mutex_unlock(&data->lock_forks[philosophe->forks[1]]);

	//check_died(philosophe, data);
	write_in_stdout(philosophe, data, "sleep");
		//usleep(data->time_to_sleep * 1000);
		psleep(data, data->time_to_sleep);

	write_in_stdout(philosophe, data, "think");
		usleep(250);
}

void *routine(void *arg)
{
    struct s_philosopher *philosophe;
    struct s_data_shared *data;

    philosophe = arg;
    data = philosophe->data;
	pthread_mutex_lock(&data->lock_eat_state);
	philosophe->last_plate = data->time_start;
	pthread_mutex_unlock(&data->lock_eat_state);
	sim_start_delay(data->time_start);
	if (philosophe->id % 2)
		think_routine(philosophe, data, 1);
		//usleep(20000);
		
	while (!has_died(philosophe, data) && !data->stop_flag)
    {
		check_died(philosophe, data);
		pthread_mutex_lock(&data->lock_forks[philosophe->forks[0]]);
        write_in_stdout(philosophe, data, "fork");
		check_died(philosophe, data);
        pthread_mutex_lock(&data->lock_forks[philosophe->forks[1]]);
        write_in_stdout(philosophe, data, "fork");
		check_died(philosophe, data);

		routine_condition(philosophe, data);
		
		//pthread_mutex_lock(&data->lock_print);
		//philosophe->time_now = get_time(data->time_to_eat + data->time_to_sleep);
		//printf("time now: %d, time start: %d, now - start: %d\n", philosophe->time_now, philosophe->last_plate, philosophe->time_now - philosophe->last_plate);
		//printf("time to die: %d\n", data->time_to_die);
		//pthread_mutex_unlock(&data->lock_print);

		check_died(philosophe, data);

		//printf("plate eaten: %d\n", philosophe->nb_plate_eaten);
		//printf("must eat: %d\n", data->number_of_times_each_philosopher_must_eat);

		pthread_mutex_lock(&data->lock_dead_state);
		if (data->number_of_times_each_philosopher_must_eat)
		{
			if (philosophe->nb_plate_eaten > data->number_of_times_each_philosopher_must_eat)
				{
					pthread_mutex_unlock(&data->lock_dead_state);
					return (NULL);
				}
		}
		pthread_mutex_unlock(&data->lock_dead_state);
        //nb_exe_routine++;
        usleep(250);
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
				if (philosophe[i]->nb_plate_eaten > data->number_of_times_each_philosopher_must_eat)
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
}

void *deadly_routine(void *arg)
{
	struct s_philosopher *philosophe;
	struct s_data_shared *data;

	philosophe = arg;
	data = philosophe->data;

	printf("{%u ms} philosophe[%d] has taken a fork\n", get_time(), philosophe->id);
	usleep(data->time_to_die * 1000);
	write_in_stdout(philosophe, data, "died");
	return (NULL);
}

void assign_forks(struct s_philosopher *philosophe, struct s_data_shared *data)
{
	//printf(COLOR_YELLOW "address of lock_forks: %p\n" RESET_ALL, data->lock_forks);
	philosophe->forks[0] = philosophe->id;
	//printf(COLOR_YELLOW "philosophe [%d] address of forks[0]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[0]);
	philosophe->forks[1] = (philosophe->id + 1) % data->number_of_philosophers;
	//printf(COLOR_YELLOW "philosophe [%d] address of forks[1]: %p\n" RESET_ALL, philosophe->id, (void *)&philosophe->forks[1]);
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
	philosophe->thread = 0;
	philosophe->last_plate = 0;
	philosophe->time_now = 0;
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
	if (ft_atoi(argv[1]) > 200)
	{
		printf(COLOR_RED "Error\n" RESET_ALL);
		exit(EXIT_FAILURE);
	}
	browse_inputs(argv);
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
    destroy_threads(philosophe, &data);
	//destroy_check_threads(philosophe, &data);
	destroy_mutex(&data);
	free_struct(philosophe, &data);
    return (0);
}
