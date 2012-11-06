
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ugroup.h"

DECL_UGROUP_TYPE(int)

UGROUP_TYPE(int)

#define release(a) if (a != NULL) free(a); a = NULL;

void test_1(void)
{
	int val = 5;
	int_group *five = int_group_Array(1, &val);	
	assert(*five->items[0] == 5);
	assert(*five->min == 5);
	assert(*five->max == 5);
	
	release(five);
}

void test_2(void)
{
	int arr[] = {1,2,3,4,5};
	int_group *a = int_group_Array(sizeof(arr)/sizeof(int), arr);
	int_group *b = NULL;
	int_group *c = NULL;
	
	assert(*(a->items[0]) == 1);
	assert(*(a->items[1]) == 2);
	
	b = int_group_Slice(arr, 1, 3);
	
	// printf("b->n %i\n", b->n);
	
	assert(*b->min == 2);
	assert(*b->max == 4);
	
	assert(*b->items[0] == 2);
	assert(*b->items[1] == 3);
	
	c = int_group_And(a, b);
	
	// printf("c->min %i\n", *(c->min));
	
	assert(c->n == 3);
	assert(c->min == b->min);
	assert(c->max == b->max);
	
	release(a);
	release(b);
	release(c);
}

void test_3(void)
{
	int arr[] = {1,2,3,4,5,6};
	// 1, 2, 3, 4
	int_group *a = int_group_Slice(arr, 0, 3);
	// 3, 4, 5, 6
	int_group *b = int_group_Slice(arr, 2, 5);
	// 3, 4
	int_group *c = int_group_And(a, b);
	// 1, 2, 3, 4, 5, 6
	int_group *d = int_group_Or(a, b);
	// 1, 2
	int_group *e = int_group_Except(a, b);
	// 5, 6
	int_group *f = int_group_Except(b, a);
	
	// printf("%i\n", *(c->min));
	// printf("%i\n", *(c->max));
	
	assert(*c->min == 3);
	assert(*c->max == 4);
	
	assert(c->n == 2);
	assert(*c->items[0] == 3);
	assert(*c->items[1] == 4);
	
	// printf("%i\n", d->n);
	
	assert(d->n == 6);
	
	// printf("%i\n", *(d->min));
	// printf("%i\n", *d->items[4]);
	
	assert(*d->min == 1);
	assert(*d->max == 6);
	assert(*d->items[0] == 1);
	assert(*d->items[1] == 2);
	assert(*d->items[2] == 3);
	assert(*d->items[3] == 4);
	assert(*d->items[4] == 5);
	assert(*d->items[5] == 6);
	
	// printf("%i\n", e->n);
	
	assert(e->n == 2);
	assert(*e->min == 1);
	assert(*e->max == 2);
	assert(*e->items[0] == 1);
	assert(*e->items[1] == 2);
	
	// printf("%i\n", *e->items[1]);
	
	assert(f->n == 2);
	assert(*f->min == 5);
	assert(*f->max == 6);
	assert(*f->items[0] == 5);
	assert(*f->items[1] == 6);
	
	release(a); release(b); release(c); release(d);
	release(e); release(f);
}

int main(int argc, char *argv[])
{
	test_1();
	test_2();
	test_3();
	
	int i;
	int end = 1 << 25;
	
	int_group *a = int_group_Array(30, (int[]){
		  0,   2,   4,   6,   8,  10,  12,  14,  16,  18,
		 20,  22,  24,  26,  28,  30,  32,  34,  36,  38,
		 40,  42,  44,  46,  48,  50,  52,  54,  56,  58
	});
	int_group *b = int_group_Array(30, (int[]){
		  1,   3,   5,   7,   9,  11,  13,  15,  17,  19,
		 21,  23,  25,  27,  29,  31,  33,  35,  37,  39,
		 41,  43,  45,  47,  49,  51,  53,  55,  57,  59
	});
	
	for (i = 0; i < end; i++) {
		int_group *c = int_group_And(a, b);
		free(c);
	}
	
	free(a); free(b);
	
	return 0;
}