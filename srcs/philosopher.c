/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:54:17 by lmonsat           #+#    #+#             */
/*   Updated: 2024/10/31 18:43:30 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philosopher.h"

int test = 0;

void *routine()
{
    test++;
}

void create_threads(pthread_t th[5])
{
    int i;

    i = 0;
    while (i < 5)
    {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0)
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

int main (int argc, char *argv[])
{
    pthread_t th[5];

    create_threads(th);
    printf("val tes: %d\n", test);
    destroy_threads(th);

    return (0);
}
