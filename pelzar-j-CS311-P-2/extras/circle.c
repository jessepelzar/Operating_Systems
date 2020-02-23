#include <stdio.h>
#include <string.h>

char arr[10];
int indexx = 0;
char user[81];

int main(void) {
  printf("add string of letters\n" );
  scanf("%s", &user);
  printf("%s\n", user);

  for( int i = 0; i < 500; i++ ) {
    indexx = (i % 10);

    arr[indexx] = user[i];
    printf("%d : ", indexx);
    printf("%c\n", arr[indexx]);

    //TODO : push into front
    
  }


  return 0;
}
