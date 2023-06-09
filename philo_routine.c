/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: manujime <manujime@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 12:10:18 by manujime          #+#    #+#             */
/*   Updated: 2023/05/15 11:14:12 by manujime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

//prints the status of the philosopher and waits for the time_to_sleep to pass
//while checking if the simulation is still active
void	ft_bed_time(t_philo *philo)
{
	long long int	alarm;

	if (philo->table->time_to_sleep == 0)
		return ;
	ft_print_status(philo, 3);
	alarm = ft_get_current_time(philo->table) + philo->table->time_to_sleep;
	while (ft_get_current_time(philo->table) < alarm)
	{
		if (!ft_are_we_even_alive(philo->table))
			break ;
		usleep(50);
	}
}

//calculates the time to think based on the time_to_die, time_to_eat and 
//the last meal time, prints the status and waits for the time to think to pass
//while checking if the simulation is still active
void	ft_think(t_philo *philo, int status)
{
	long long int	the_munchies;

	pthread_mutex_lock(&philo->l_meal_lock);
	the_munchies = (philo->table->time_to_die
			- (ft_get_current_time(philo->table) - philo->last_meal)
			- philo->table->time_to_eat) / 2;
	pthread_mutex_unlock(&philo->l_meal_lock);
	if (the_munchies < 0)
		the_munchies = 0;
	if (the_munchies == 0 && status == -1)
		the_munchies = 1;
	if (the_munchies > 700)
		the_munchies = 200;
	if (philo->table->time_to_sleep == 0
		&& (philo->table->time_to_die / 2) > philo->table->time_to_eat)
		the_munchies = 10;
	if (the_munchies != 0)
		ft_print_status(philo, status);
	while (ft_get_current_time(philo->table) < the_munchies)
	{
		if (!ft_are_we_even_alive(philo->table))
			break ;
		usleep(50);
	}
}

//make the philo take 2 forks and eat, sets the new last meal time,
//increments the meal count, prints the status,
//waits for the time_to_eat to pass while checking if the si,ulation active
//then releases the forks
void	ft_eat_spaguetti(t_philo *philo)
{
	long long int	stop_munch;

	if (philo->table->must_eat == 0)
		return ;
	pthread_mutex_lock(&philo->table->forks[philo->fork_1]);
	ft_print_status(philo, 1);
	pthread_mutex_lock(&philo->table->forks[philo->fork_2]);
	ft_print_status(philo, 1);
	pthread_mutex_lock(&philo->l_meal_lock);
	philo->last_meal = ft_get_current_time(philo->table);
	ft_print_status(philo, 2);
	pthread_mutex_unlock(&philo->l_meal_lock);
	stop_munch = ft_get_current_time(philo->table) + philo->table->time_to_eat;
	while (ft_get_current_time(philo->table) < stop_munch)
	{
		if (!ft_are_we_even_alive(philo->table))
			break ;
		usleep(50);
	}
	pthread_mutex_lock(&philo->l_meal_lock);
	philo->meal_count += 1;
	pthread_mutex_unlock(&philo->l_meal_lock);
	pthread_mutex_unlock(&philo->table->forks[philo->fork_1]);
	pthread_mutex_unlock(&philo->table->forks[philo->fork_2]);
}

//this function is the starting point for each philo thread when philo_count > 1
//it waits until a small delay has passed to make sure all threads are created
//then it locks the last_meal mutex to set the last meal time
//if the philo is odd it makes it think for a bit to avoid deadlocks
//then it enters the main loop of the philo
void	*ft_philo_start(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->table->time_to_die == 0)
		return (NULL);
	while (philo->table->time_start > ft_get_basic_time())
		usleep(100);
	pthread_mutex_lock(&philo->l_meal_lock);
	philo->last_meal = ft_get_current_time(philo->table);
	pthread_mutex_unlock(&philo->l_meal_lock);
	if (philo->id % 2)
		ft_think(philo, -1);
	while (ft_are_we_even_alive(philo->table))
	{
		ft_eat_spaguetti(philo);
		ft_bed_time(philo);
		ft_think(philo, 4);
	}
	return (NULL);
}
