/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:54:52 by regillio          #+#    #+#             */
/*   Updated: 2025/05/23 15:55:03 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/* Adds the node 'new' at the beginning of the list.
 * lst : the adress of a pointer to the first node of the list.
 * new : the address of a pointer to the node to be added.
 */
void	ft_lstadd_front(t_list **lst, t_list *new)
{
	t_list	*temp;

	temp = new;
	if (*lst)
	{
		new->next = *lst;
		*lst = new;
	}
	else
	{
		*lst = new;
		new->next = NULL;
	}
}
/*
int	main()
{
	t_list **lst = malloc(sizeof(t_list*));
	int *temp;
	t_list *first = malloc(sizeof(t_list));
	first->content = (int*)malloc(sizeof(int));
	temp = first->content;
	*temp = 10;


	t_list *second_node = malloc(sizeof(t_list));
	second_node->content = (int*)malloc(sizeof(int));
	int *temp2;
	temp2 = second_node->content;
	*temp2 = 20;

	t_list *third_node = malloc(sizeof(t_list));
	third_node->content = (int *)malloc(sizeof(int));
	int *temp3;
	temp3 = third_node->content;
	*temp3 = 30;


	first->next = second_node;
	second_node->next = third_node;
	third_node->next = NULL;

	printf("Linked list: ");
	ft_lstadd_front(lst, first);
	ft_lstadd_front(lst, second_node);
	ft_lstadd_front(lst, third_node);
}
*/
