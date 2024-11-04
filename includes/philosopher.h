/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:55:51 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/04 22:10:11 by lmonsat          ###   ########.fr       */
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

struct s_philosopher 
{
	struct s_philosopher *philosophe[5];
	pthread_t thread;
    pthread_mutex_t lock_eat;
	pthread_mutex_t lock_time;
    pthread_mutex_t lock_think;
    pthread_mutex_t lock_sleep;
	pthread_mutex_t lock_dead;

	int thread_created;
	int id;
    int forks;
    int number_of_philosophers;
    int number_of_times_each_philosopher_must_eat;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
	uint32_t time_start;
	uint32_t time_now;
    int has_eaten;
    int has_slept;
    int has_thought;
	int has_died;
};

int	ft_atoi(const char *str);
void create_threads(struct s_philosopher *value);

#endif
