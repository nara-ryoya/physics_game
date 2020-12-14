// 1番に引き続き、重力加速度をgとして置いているので注意
//fgetsで空白となっているインデックスを格納して、atofを利用して値を取り出す
//ファイル名が書かれなかったら、ランダムに値を代入するようにした

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
// #include "physics2.h"



typedef struct condition
{
  const int width; // 見えている範囲の幅
  const int height; // 見えている範囲の高さ
  const double G; // 重力定数
  const double dt; // シミュレーションの時間幅
  const double cor_floor; // 床の反発係数
  const double cor_wall;//壁の反発係数
  const double g; //重力加速度
} Condition;

// 個々の物体を表す構造体
typedef struct object
{
  double m;
  double y;
  double x;
  double prev_y;
  double prev_x;
  double vy;
  double vx;
} Object;

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond);
void my_update_velocities(Object objs[], const size_t numobj, const Condition cond);
void my_update_positions(Object objs[], const size_t numobj, const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);

// シミュレーション条件を格納する構造体
// 反発係数CORを追加

int main(int argc, char **argv)
{
if (argc > 3 || argc == 2) {
    printf("usage: [filename for init]\n\n");
    return -1;
}

  const Condition cond = {
            .g = 9.8,
		    .width  = 75,
		    .height = 40,
		    .G = 2.2 * 0.00001,
		    .dt = 0.1,
		    .cor_floor = 0.8,
            .cor_wall = 0.5
  };
  /*ここからファイル読み込みの処理*/
  FILE *fp;
  size_t objnum;
  int max_objnum = 30;
  double status_for_object[max_objnum][5];
  if (argc == 3) {
      objnum = atoi(argv[1]);
      FILE *fp;
      fp = fopen(argv[2], "r");
      if (fp == NULL) {
          printf("can't open the file\n");
          return -1;
      }
      int bufsize = 500;
      int cnt_obj = 0;
      char buf[bufsize];
      while (fgets(buf, bufsize, fp) != NULL || cnt_obj == objnum) {
          if (buf[0] == '#') {
              continue;
          }
          int zero_index_array[5];
          zero_index_array[0] = 0;
          int index_for_zero_index_array = 1;
          for (int i = 0; i < strlen(buf); ++i) {
              if (buf[i] == ' ') {
                  zero_index_array[index_for_zero_index_array] = i + 1; //空白のインデックスの次のインデックスを格納
                  index_for_zero_index_array ++;
                  if (index_for_zero_index_array == 5) {
                      break;
                  }
              }
          }
          for (int i = 1; i < 5; ++i) {
              buf[zero_index_array[i] - 1] = 'a'; //atofでは空白は見過ごされてしまうので、仕切り文字を入れておく
          }
          for (int i = 0; i < 5; ++i) {
              status_for_object[cnt_obj][i] = atof(buf + zero_index_array[i]);
          }
          cnt_obj ++;
      }
      }

else { //引数が書かれなかった場合
objnum = 2;
for (int i = 0; i < objnum; ++i) {
    for (int j = 0; j < 5; ++j) {
        status_for_object[i][j] = (double) (rand() % 10);
    }
}
}

Object objects[objnum];

for (int i = 0 ; i < objnum ; ++i) {

    objects[i] = (Object) {
        .m = status_for_object[i][0],
        .x = status_for_object[i][1],
        .y = status_for_object[i][2],
        .vx = status_for_object[i][3],
        .vy = status_for_object[i][4]
    };
}
/*ファイル読み込み終了*/

  const double stop_time = 400;
  double t = 0;
  printf("\n");
  for (int i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum, cond);

    my_plot_objects(objects, objnum, t, cond);

    usleep(200 * 1000);
    printf("\e[%luA", cond.height + 4 + objnum);
  }
  return EXIT_SUCCESS;
}

// 実習: 以下に my_ で始まる関数を実装する
void my_update_velocities(Object objs[], const size_t numobj, const Condition cond)
{
for (int i = 0; i < numobj ; ++i) {
double F_y = 0, F_x = 0;
//i以外の物体から受ける万有引力を、成分ごとに計算して足していく
for (int j = 0; j < numobj ; ++j) {
  if (i != j) {
    double distance_x = objs[j].x - objs[i].x;
    double distance_y = objs[j].y - objs[i].y;
    double distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
    F_x += cond.G * objs[j].m * objs[i].m * distance_x /pow(distance, 3);
    F_y += cond.G * objs[j].m * objs[i].m * distance_y /pow(distance, 3);
  }
}
double acceleration_x = F_x / objs[i].m;
double acceleration_y = cond.g + F_y / objs[i].m;
objs[i].vy += cond.dt * acceleration_y;
objs[i].vx += cond.dt * acceleration_x;
}
}

void my_update_positions(Object objs[], const size_t numobj, const Condition cond)
{
for (int i = 0; i < numobj; ++i) {
double tmp_position_y = objs[i].y;
double tmp_position_x = objs[i].x;
objs[i].y += objs[i].vy * cond.dt;
objs[i].x += objs[i].vx * cond.dt;
objs[i].prev_y = tmp_position_y;
objs[i].prev_x = tmp_position_x;
}
}

void my_bounce(Object objs[], const size_t numobj, const Condition cond)
{
double boarder_y = cond.height / 2.0;
double boarder_x = cond.width / 2.0;
for (int i = 0; i < numobj; ++i) {
  if ((objs[i].y - boarder_y) * (objs[i].prev_y - boarder_y) <= 0) {
    double time_after_conflict = (objs[i].y - boarder_y)  / objs[i].vy;
    objs[i].vy *= (- cond.cor_floor);
    objs[i].y = boarder_y + time_after_conflict * objs[i].vy;
    }
 }
for (int i = 0; i < numobj; ++i) {
  if ((objs[i].x - boarder_x) * (objs[i].prev_x - boarder_x) <= 0 || (objs[i].x + boarder_x) * (objs[i].prev_x + boarder_x) <= 0) { //ボーダーを跨いでいたら
    double dir = -1.0; 
    if (objs[i].x > 0) {

      dir = 1.0;
    }
    double time_after_conflict = fabs((objs[i].x - boarder_x * dir) / objs[i].vx);
    objs[i].vx *= (- cond.cor_wall);


    objs[i].x = (boarder_x * dir) + (time_after_conflict * objs[i].vx);
    }
  }
}

void my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond) {
double mid_y = cond.height / 2.0;
double mid_x = cond.width / 2.0;
printf("\n");
printf("+");
for (int x = 0; x < cond.width ; ++x) {
  printf("-");
}
printf("+\n");
for (int y = 0; y < cond.height; ++y) {
  printf("|");
  for (int x = 0; x < cond.width; ++x) {
    int obj_exist_or_not = 1;
    for (int i = 0; i < numobj; ++i) {
    if (y == (int)mid_y + (int)objs[i].y && x == (int)mid_x + (int)objs[i].x ) {
      printf("%d", i);
      obj_exist_or_not --;
      break;
    }
    }
    if (obj_exist_or_not) {
      printf(" ");
    }
  }
  printf("|");
    printf("      \n");
}
printf("+");
for (int x = 0; x < cond.width ; ++x) {
  printf("-");
}
printf("+\n");
printf("t = %.1f\n", t);
for (int i = 0; i < numobj; ++i) {
  printf("objs[%d] = (%.2f, %.2f)　　　　　　　　　　\n", i, objs[i].x, objs[i].y);
}
}
