/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:55:51 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/26 01:13:29 by lmonsat          ###   ########.fr       */
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
# include <stdbool.h>
# define RESET_ALL  "\x1b[0m"
# define COLOR_RED  "\x1b[31m"
# define COLOR_YELLOW   "\x1b[33m"

struct s_data_shared
{
    pthread_mutex_t lock_eat_state;
	pthread_mutex_t lock_print;
	pthread_mutex_t lock_time_state;
    pthread_mutex_t lock_think_state;
    pthread_mutex_t lock_sleep_state;
	pthread_mutex_t lock_dead_state;
    pthread_mutex_t *lock_forks;
	bool stop_flag;
	bool has_died;
	unsigned int id;
    unsigned int nb_of_philo;
    unsigned int nb_of_times_each_philo_must_eat;
    uint32_t time_to_die;
    uint32_t time_to_eat;
    uint32_t time_to_sleep;
};

struct s_philo
{
    struct s_data_shared *data;
	//struct s_philo **philo;
	pthread_t thread;
	unsigned int id;
    unsigned int forks[2];
	uint32_t last_plate;
	uint32_t time_now;
	bool stop_flag;
	bool has_died;
	unsigned int nb_plate_eaten;
};

/* strings manipulation */
int	ft_atoi(const char *str);
int	ft_strcmp(char *s1, char *s2);
void browse_inputs(char *argv[]);

/* checks */
void check_died(struct s_philo *philo, struct s_data_shared *data);
int has_died(struct s_philo *philo, struct s_data_shared *data);

/* assign */
void assign_struct(struct s_philo **philo, struct s_data_shared *data);

/* routine */
void *deadly_routine(void *arg);
void *routine(void *arg);

/* Threads */
void create_threads(struct s_philo **philo, struct s_data_shared *data);
void destroy_threads(struct s_philo **philo, struct s_data_shared *data);

/* Mutexes */
void mutex_init(struct s_data_shared *data);
void destroy_mutex(struct s_data_shared *data);

/* utils */
void psleep(uint32_t sleep_time);
void free_struct(struct s_philo **philo, struct s_data_shared *data);
void write_in_stdout(struct s_philo *philo, struct s_data_shared *data, char *state);
uint32_t get_time(void);
uint32_t waiting_time(struct s_data_shared *data);

#endif
