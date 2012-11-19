/*
 * dinner_party version 1.00.0
 * Copyright 2012 - 2012 Dejun Qian - electronseu@gmail.com 
 *                                  - http://web.cecs.pdx.edu/~dejun
 */

#include <stdlib.h>
#include <stdio.h>

typedef _Bool bool;

struct node {
  void* data;
  struct node* prev;
};

static struct node* head = NULL;
static struct node* tail = NULL;

#if 0
bool empty() {
  return (head==NULL);
}
#else
bool empty() {
  bool res = (head==NULL);
  return res;
}
#endif

void push(void* data) {
  if(head == NULL) {
    head = malloc(sizeof(struct node));
    head->data = data;
    head->prev = NULL;
    tail = head;
  } else {
    struct node* temp = malloc(sizeof(struct node));
    temp->data = data;
    temp->prev = tail;
    tail = temp;
  }
}

void* pop() {
  if(empty()) {
    return NULL;
  } else {
    void* data = tail->data;
    struct node* temp = tail;
    tail = tail->prev;
    if(tail == NULL) {
      head = NULL;
    }
    free(temp);
    return data;
  }
}

void dump() {
  struct node* temp = tail;
  printf("dump stack\n");
  while(temp != NULL) {
    printf("0x%08x ", (unsigned int)(temp->data));
    temp = temp->prev;
  }
  printf("\n");
}
