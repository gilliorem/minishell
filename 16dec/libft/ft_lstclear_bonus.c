/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: regillio <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:55:31 by regillio          #+#    #+#             */
/*   Updated: 2025/05/23 15:56:26 by regillio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/* Deletes and frees its given nodes and all its successors.
 * using the del(void *) function and free()
 * Finally, set the pointer to the list to NULL. (*lst = NULL) ?
 * 
 * lst is the address of a pointer to a node. (pointer to pointer)
 * basically it points to a node->next which is the following node in the list
 * del is the address of the function use to delete the node.
 *
 */
void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*current;
	t_list	*next;

	if (!lst || !del)
		return ;
	current = *lst;
	while (current)
	{
		next = current->next;
		del(current->content);
		free(current);
		current = next;
	}
	*lst = NULL;
}
