/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:55:51 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/02 20:47:13 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# define RESET_ALL  "\x1b[0m"
# define COLOR_RED  "\x1b[31m"
# define COLOR_YELLOW   "\x1b[33m"

struct s_philosopher 
{
    pthread_mutex_t lock_eat;
    pthread_mutex_t lock_think;
    pthread_mutex_t lock_sleep;
    int forks;
    int number_of_philosophers;
    int number_of_times_each_philosopher_must_eat;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int has_eaten;
    int has_slept;
    int has_thought;
};

int	ft_atoi(const char *str);


#endif
