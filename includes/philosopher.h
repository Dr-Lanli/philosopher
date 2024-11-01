/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmonsat <lmonsat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:55:51 by lmonsat           #+#    #+#             */
/*   Updated: 2024/11/01 03:16:28 by lmonsat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>

struct s_philosopher 
{
    int has_eaten;
    int has_slept;
    int has_thought;
};


#endif
