#include <stdio.h>

struct student
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
};

struct tagged_student1
{
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
};

struct tagged_student2
{
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
};

int main (int argc, char**argv)
{
  struct student s_begin;
  struct student s1;
  struct tagged_student1 s2;
  struct tagged_student2 s3;
  struct tagged_student2 s_end;

  /* 以下に各構造体の中身のポインタを表示するプログラムを書く */
  /* printf で %p フォーマットを指定する*/
  /* 逆順に表示（send, s3, s2, s1, sbegin の順）*/
  printf("tag: char , name: char[100], id: int, height: double, weight: double, age: int\n");
  printf("s_end\n");
  printf("tag: %p, id: %p, name: %p, age: %p, height: %p, weight: %p\n",&s_end.tag, &s_end.id, &s_end.name,
   &s_end.age, &s_end.height, &s_end.weight);
  printf("___________________________________________________________________________________________________________________\n");
  printf("s3\n");
  printf("tag: %p, id: %p, name: %p, age: %p, height: %p, weight: %p\n",&s3.tag, &s3.id, 
  &s3.name, &s3.age, &s3.height, &s3.weight);
  printf("___________________________________________________________________________________________________________________\n");
  printf("s2\n");
  printf("id: %p, name: %p, age: %p, height: %p, weight: %p, tag: %p\n", &s2.id, &s2.name, &s2.age,
   &s2.height, &s2.weight, &s2.tag);
  printf("___________________________________________________________________________________________________________________\n");
  printf("s1\n");
  printf("id: %p, name: %p, age: %p, height: %p, weight: %p\n", &s1.id, &s1.name, &s1.age, &s1.height, &s1.weight);
  printf("___________________________________________________________________________________________________________________\n");
  printf("s_begin\n");
  printf("id: %p, name: %p, age: %p, height: %p, weight: %p\n", &s_begin.id, &s_begin.name,
   &s_begin.age, &s_begin.height, &s_begin.weight);
  // do_something!!

  /* 以下には各構造体のサイズをsizeof演算子で計算し、表示する */
  /* printf 表示には%ld を用いる*/
  printf("___________________________________________________________________________________________________________________\n");
  printf("s_begin : %ld\n", sizeof(s_begin));
  printf("s1 : %ld\n", sizeof(s1));
  printf("s2 : %ld\n", sizeof(s2));
  printf("s3 : %ld\n", sizeof(s3));
  printf("s_end : %ld\n", sizeof(s3));

  // do_something!!
  
  return 0;
}
