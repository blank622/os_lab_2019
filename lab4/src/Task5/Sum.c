#include "Sum.h"

int Sum(const struct SumArgs *args) {
  int sum = 0;
  // TODO: your code here
  int i = (*args).begin;
  for (; i < (*args).end; i++){
      sum += (*args).array[i];
  }
  return sum;
}