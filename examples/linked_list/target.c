#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct LL {
  struct LL* next;
  uint32_t data;
};

struct LL* const init_ll(const size_t length) {
  struct LL* const head = (struct LL*)calloc(1, sizeof(struct LL));

  struct LL* node = (struct LL*)calloc(1, sizeof(struct LL));
  head->next = node;

  for (uint32_t i = 0; i < length; i++) {
    struct LL* next = (struct LL*)calloc(1, sizeof(struct LL));
    node->next = next;
    node->data = i;
    node = next;
  }
  return head;
}

void check_ll(struct LL* head, const size_t length) {
  for (uint32_t i = 0; i < length; i++) {
    if (head->data > length) {
      printf("LL has been modified at node %d. Data is now %d", i, head->data);
      exit(0);
    }
  }
}

int main() {
  const pid_t pid = getpid();
  printf("Out PID is %d\n", pid);
  const size_t length = 5000;
  struct LL* ll = init_ll(length);
  ll->data = 42;

  printf("Initialised LinkedList of %lu long\n", length);

  while (1) {
    check_ll(ll, length);
    sleep(1);
  }

  return 0;
}