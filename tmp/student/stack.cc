#include <stdio.h>

int func(int val)
{
  int buf[1024*1024*1024];

  val++;
  for (int i = 0; i < (int)sizeof(buf); i++) buf[i] = val;
  if (val % 1000000 == 0) printf("%d\n", val);
  return func(val);
}

int
main(void)
{
  func(0);

  return 0;
}
