#include <stdio.h>
#include <stdlib.h>
#include <testcase.h>
#include <conf.h>

void free_testcases(struct testcase *head)
{
		struct testcase *temp;

		while (head) {
				temp = head->next;
				free(head);
				head = temp;
		}
}
