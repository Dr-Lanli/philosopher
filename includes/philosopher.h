/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:55:51 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/19 21:55:45 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <stdlib.h>
# include <stdio.h>
# include <stdint.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>
# define RESET_ALL  "\x1b[0m"
# define COLOR_RED  "\x1b[31m"
# define COLOR_YELLOW   "\x1b[33m"

struct s_data_shared
{
    pthread_mutex_t lock_eat_state;
	pthread_mutex_t lock_print;
	//pthread_mutex_t lock_eat_assign;
	pthread_mutex_t lock_time_state;
	//pthread_mutex_t lock_time_assign;
    pthread_mutex_t lock_think_state;
	//pthread_mutex_t lock_think_assign;
    pthread_mutex_t lock_sleep_state;
	//pthread_mutex_t lock_sleep_assign;
	pthread_mutex_t lock_dead_state;
	//pthread_mutex_t lock_dead_assign;
    pthread_mutex_t *lock_forks;
	unsigned int stop_flag;
	unsigned int id;
    unsigned int number_of_philosophers;
    unsigned int number_of_times_each_philosopher_must_eat;
	uint32_t time_start;
    uint32_t time_to_die;
    uint32_t time_to_eat;
    uint32_t time_to_sleep;
	pthread_t check_thread;
};

struct s_philosopher 
{
    struct s_data_shared *data;
	struct s_philosopher **philosophe;
	pthread_t thread;


	unsigned int id;
    unsigned int forks[2];
	//unsigned int forks_in_hands;
	uint32_t last_plate;
	uint32_t time_now;
	unsigned int stop_flag;
	unsigned int nb_plate_eaten;
    //unsigned int has_eaten;
    //unsigned int has_slept;
    //unsigned int has_thought;
	unsigned int has_died;
};

int	ft_atoi(const char *str);
uint32_t get_time(void);
int has_died(struct s_philosopher *philosophe, struct s_data_shared *data);
void free_struct(struct s_philosopher **philosophe, struct s_data_shared *data);
void write_in_stdout(struct s_philosopher *philosophe, struct s_data_shared *data, char *state);
int	is_valid_int(char *str);

void *deadly_routine(void *arg);
void *routine(void *arg);

/* Threads */
void create_threads(struct s_philosopher **philosophe, struct s_data_shared *data);
void destroy_threads(struct s_philosopher **philosophe, struct s_data_shared *data);
void destroy_check_threads(struct s_philosopher **philosophe, struct s_data_shared *data);

/* Mutexes */
void mutex_init(struct s_data_shared *data);
void destroy_mutex(struct s_data_shared *data);

#endif
