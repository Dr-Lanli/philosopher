/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:55:51 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/07 00:02:27 by lmonsat          ###   ########.fr       */
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
    pthread_mutex_t lock_eat;
	pthread_mutex_t lock_time;
    pthread_mutex_t lock_think;
    pthread_mutex_t lock_sleep;
	pthread_mutex_t lock_dead;
    pthread_mutex_t *forks;
	unsigned int stop_flag;
    int total_forks;
	unsigned int id;
    unsigned int number_of_philosophers;
    unsigned int number_of_times_each_philosopher_must_eat;
    uint32_t time_to_die;
    uint32_t time_to_eat;
    uint32_t time_to_sleep;
};

struct s_philosopher 
{
    struct s_data_shared *data;
	struct s_philosopher **philosophe;
	pthread_t thread;
	pthread_t check_thread;

	unsigned int id;
    pthread_mutex_t *left_f;
    pthread_mutex_t *right_f;
    int forks_in_hands;
	uint32_t time_start;
	uint32_t time_now;
	unsigned int nb_plate_eaten;
    unsigned int has_eaten;
    unsigned int has_slept;
    unsigned int has_thought;
	unsigned int has_died;
};

int	ft_atoi(const char *str);
uint32_t get_time(uint32_t time_to_add);

#endif
