/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   existence.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: manujime <manujime@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:45:56 by manujime          #+#    #+#             */
/*   Updated: 2023/05/12 20:59:32 by manujime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

//checks if the simulation should continue, if not, it returns 0
int	ft_are_we_even_alive(t_table *table)
{
	pthread_mutex_lock(&table->cynical_lock);
	if (table->existence == 1)
	{
		pthread_mutex_unlock(&table->cynical_lock);
		return (1);
	}
	else
		return (0);
	pthread_mutex_unlock(&table->cynical_lock);
}

//creates a thread for each philo, if there is only one philo, it calls
// the single philo function
void	ft_beginning_of_existence(t_philo **philo, t_table *table)
{
	int	c;

	c = 0;
	table->time_start = (table->philo_count * 2 * 10) + ft_get_basic_time();
	if (table->philo_count == 1)
	{
		table->time_start = ft_get_basic_time();
		if (pthread_create(&philo[c]->p_thread, NULL, &ft_single_philo,
				philo[c]) != 0)
			ft_exit_error(5, &table->existence);
		return ;
	}
	while (philo[c])
	{
		if (pthread_create(&philo[c]->p_thread, NULL, &ft_philo_start,
				philo[c]) != 0)
			ft_exit_error(5, &table->existence);
		c++;
	}
}

//checks if a philo has died
int	ft_ur_no_more(t_philo *philo)
{
	pthread_mutex_lock(&philo->l_meal_lock);
	if ((ft_get_current_time(philo->table) - philo->last_meal)
		>= philo->table->time_to_die)
	{
		pthread_mutex_unlock(&philo->l_meal_lock);
		pthread_mutex_lock(&philo->table->cynical_lock);
		philo->table->existence = 0;
		ft_print_status(philo, 5);
		pthread_mutex_unlock(&philo->table->cynical_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->l_meal_lock);
	return (0);
}

//checks if all philos have eaten enough
int	ft_philo_check(t_philo **academy)
{
	int	c;
	int	satiated;

	satiated = 1;
	c = 0;
	while (academy[c])
	{
		if (ft_ur_no_more(academy[c]))
			return (1);
		pthread_mutex_lock(&academy[c]->m_count_lock);
		if (academy[c]->table->must_eat == -1
			|| academy[c]->meal_count < academy[c]->table->must_eat)
			satiated = 0;
		pthread_mutex_unlock(&academy[c]->m_count_lock);
		c++;
	}
	c = 0;
	if (satiated == 1)
	{
		pthread_mutex_lock(&academy[c]->table->cynical_lock);
		academy[c]->table->existence = 0;
		pthread_mutex_unlock(&academy[c]->table->cynical_lock);
	}
	return (satiated);
}

//it checks if the simulation should continue, if not, it breaks the loop
void	ft_keep_this_going(t_philo **academy)
{
	while (1)
	{
		if (ft_philo_check(academy))
		{
			usleep(1000);
			break ;
		}
		usleep(1000);
	}
}
