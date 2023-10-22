#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct point{
    int x;
    char *s;
    struct point *ptr;
} point;

// int strcmp(char *a , char *b){



//     while(*(a) == *(b)){
//         if(*a == '\0'){
//             return 0;
//         }
//         a++;
//         b++;
//     }

//     if((*a - *b) > 0) return 1;
//     else return -1;

// }

int main(){
    // char arr[] = "Hello";

    // char a[] = "Hell";

    // char b[] = "Hello";

    // printf("%d",(strcmp(&a,&b)));

    // int p[][2] = {{1,6},{4,4},{5,6}};

    // int *ptr = (p);

    // int **ptr2 = p;

    // char str[] = "peaceful";

    // char *s = str;

    // printf("%s",s++ +4);
    // printf("%s",s +4);

    // char str[] = "Hello";

    // char *str2 = (char *) malloc(10);

    // if(str2==NULL) return 0;
    
    // str2 = "Hello";

    // str2[1] = 'i';

    // printf("%c",str2[1]);

    point pt;

    pt.x = 5;

    pt.s = malloc(10);

    pt.ptr = malloc(sizeof(pt));

    pt.ptr->s = malloc(20); // for pointer struct member use arrow 

    strcpy(pt.ptr->s,"Hello");

    // printf("Hello : %p\n",pt.ptr->s);

    //printf("%s",pt.ptr->s);

    // char *s = "Hello" -> immutable

    // free(pt.ptr->s);

    // pt.ptr->s = NULL;

    // pt.ptr->s = malloc(14); // for pointer struct member use arrow 
    
    // pt.ptr->s = "jfiajfia";

    // printf("J : %s\n",pt.ptr->s-5);
    
    // printf("J : %p\n",pt.ptr->s - 6);

    //pt.ptr->ptr->s = malloc(10);

    //printf("%s",(pt.ptr->s - 6));

    

    // char *c = "Hello";

    // char *p = c;

    // //free(c);

    // c = malloc(6);
    
    // //strcpy(c,"JOJO");

    // printf("%s",(c));

    // printf("%s",(p));



    // printf("%s",pt.ptr->s);



    // strcpy(pt.s,"Hello");

    // printf("%c\n",*(pt.s+3));

    // //int i = 0;
    // while(*(pt.s) != '\0'){
    //     printf("%c",*(pt.s));
    //     pt.s++;
    // }


    
    // arr[i][j] = *(*(p + i) + j)
    //  printf("%d\n",**(p+2)+1);
    // printf("%d\n",*(ptr+1));
    // printf("%d\n",*(ptr2 +1));
   



    // for(int i = 0 ; arr[i] ; i++){
    //     printf("%c", arr[i]);
    // }

    char *f ;

    char s[10] ;
    //"nijja";


    int *i = malloc(40);

    int **p = &i;



    f = "Kohli100";

    


    f = "Rohit100";

    // s = "Rohit100";

    //f[5] = '3';

    printf("%s",f);




}