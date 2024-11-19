/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 21:50:58 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/19 21:55:57 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

void create_threads(struct s_philosopher **philosophe, struct s_data_shared *data)
{
    int i;

	data->time_start = get_time() + (data->number_of_philosophers * 2 * 10);
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
		//printf("philosophe[%d] has been created\n", philosophe[i]->id);
       	//usleep(100);
        i++;
    }
	/*if (pthread_create(&data->check_thread, NULL, &is_alive_routine, philosophe) != 0)
	{
		exit(EXIT_FAILURE);
	}*/
}

void destroy_threads(struct s_philosopher **philosophe, struct s_data_shared *data)
{
    int i;
    
    i = 0;
    while (i < data->number_of_philosophers)
    {
        if (pthread_join(philosophe[i]->thread, NULL) != 0)
        {
			free_struct(philosophe, data);
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
		if (pthread_join(data->check_thread, NULL) != 0)
        {
            exit(EXIT_FAILURE);
        }
        usleep(25);
        i++;
    }
}