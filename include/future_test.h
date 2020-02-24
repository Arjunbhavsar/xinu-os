#include <xinu.h>
#include <future.h>
static int zero = 0, one = 1, two = 2;
future_t **fibfut;

uint future_prod(future_t *fut, char *value);
uint future_cons(future_t *fut);
int ffib(int n);
