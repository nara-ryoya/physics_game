//ここでは問題文に「星」というワードがあるので、1,2とは異なり、今回は重力加速度を考慮していない
//今回は上の壁も実装した

//衝突してしまった物体同士は、indexが小さい方を残し、indexが大きい方は質量を負にしてしまうことでその後の処理において区別できるようにした

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define epsilon 1 //epsilonより近づいたら融合
// #include "physics2.h"




typedef struct condition
{
  const int width; // 見えている範囲の幅
  const int height; // 見えている範囲の高さ
  const double G; // 重力定数
  const double dt; // シミュレーションの時間幅
  const double cor_floor; 
  const double cor_wall;
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

int my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond);
//表示する点の数が変わってくるので、intとして表示する値を返すようにした(最終的にsystem("clear")を使ったので結局不要であった)
void my_update_velocities(Object objs[], const size_t numobj, const Condition cond);
void my_update_positions(Object objs[], const size_t numobj, const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);
void merge(Object objs[], size_t numobj, const Condition cond); //融合を表す関数


int main(int argc, char **argv)
{
srand(time(NULL));
if (argc > 2) {
    printf("usage: [filename for init]\n\n");
    return -1;
}

  const Condition cond = {
		    .width  = 75,
		    .height = 40,
		    .G = 10,
		    .dt = 0.02,
		    .cor_floor = 0.8,
        .cor_wall = 0.5,
  };

  FILE *fp;
  
  size_t objnum;
  int max_obj_num = 30;
  double status_for_object[max_obj_num][5];
  if (argc == 2) {
      FILE *fp;
      fp = fopen(argv[1], "r");
      if (fp == NULL) {
          printf("can't open the file\n");
          return -1;
      }
      int bufsize = 500;
      int cnt_obj = 0;
      char buf[bufsize];
      while (fgets(buf, bufsize, fp) != NULL) {
          if (buf[0] == '#') {
              continue;
          }
          int zero_index_array[5];
          zero_index_array[0] = 0;
          int index_for_zero_index_array = 1;
          for (int i = 0; i < strlen(buf); ++i) {
              if (buf[i] == ' ') {
                  zero_index_array[index_for_zero_index_array] = i + 1; 
                  index_for_zero_index_array ++;
                  if (index_for_zero_index_array == 5) {
                      break;
                  }
              }
          }
          for (int i = 1; i < 5; ++i) {
              buf[zero_index_array[i] - 1] = 'a';
          }
          for (int i = 0; i < 5; ++i) {
              status_for_object[cnt_obj][i] = atof(buf + zero_index_array[i]);
          }
          cnt_obj ++;
      }
      objnum = cnt_obj;
      }

else { 
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
        .vy = status_for_object[i][4],
    };
}


  const double stop_time = 400;
  double t = 0;
  printf("\n");
  for (int i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum, cond);
    merge(objects, objnum, cond);

    int cnt_printed_num = my_plot_objects(objects, objnum, t, cond);

    usleep(200 * 1000);
    printf("\e[%dA", cond.height + 4 + cnt_printed_num);
  }
  return EXIT_SUCCESS;
}


void my_update_velocities(Object objs[], const size_t numobj, const Condition cond)
{
for (int i = 0; i < numobj ; ++i) {
if (objs[i].m < 0) {
  continue;
}
double F_y = 0, F_x = 0;

for (int j = 0; j < numobj ; ++j) {
  if (objs[j].m < 0) {
    continue;
  }
  if (i != j) {
    double distance_x = objs[j].x - objs[i].x;
    double distance_y = objs[j].y - objs[i].y;
    double distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
    F_x += cond.G * objs[j].m * objs[i].m * distance_x /pow(distance, 3);
    F_y += cond.G * objs[j].m * objs[i].m * distance_y /pow(distance, 3);
  }
}
double acceleration_x = F_x / objs[i].m;
double acceleration_y = F_y / objs[i].m;
objs[i].vy += cond.dt * acceleration_y;
objs[i].vx += cond.dt * acceleration_x;
}
}

void my_update_positions(Object objs[], const size_t numobj, const Condition cond)
{
for (int i = 0; i < numobj; ++i) {
  if (objs[i].m < 0) {
    continue;
  }
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
  if (objs[i].m < 0){
    continue;
  }
  if ((objs[i].y - boarder_y) * (objs[i].prev_y - boarder_y) <= 0 || (objs[i].y + boarder_y) * (objs[i].prev_y + boarder_y) <= 0) { //ボーダーを跨いでいたら
    double dir = -1.0;//床にぶつかったら1、天井にぶつかったら-1を返す
    if (objs[i].y > 0) {

      dir = 1.0;
    }
    double time_after_conflict = fabs((objs[i].y - boarder_y * dir) / objs[i].vy);
    objs[i].vy *= (- cond.cor_wall);


    objs[i].y = (boarder_y * dir) + (time_after_conflict * objs[i].vy);
    }
 }
for (int i = 0; i < numobj; ++i) {
  if (objs[i].m < 0) {
    continue;
  }
  if ((objs[i].x - boarder_x) * (objs[i].prev_x - boarder_x) <= 0 || (objs[i].x + boarder_x) * (objs[i].prev_x + boarder_x) <= 0) { //ボーダーを跨いでいたら
    double dir = -1.0; //壁の左右どっち側にぶつかったのかを返す、右なら1,左なら-1
    if (objs[i].x > 0) {

      dir = 1.0;
    }
    double time_after_conflict = fabs((objs[i].x - boarder_x * dir) / objs[i].vx);
    objs[i].vx *= (- cond.cor_wall);


    objs[i].x = (boarder_x * dir) + (time_after_conflict * objs[i].vx);
    }
  }
}

int my_plot_objects(Object objs[], const size_t numobj, const double t, const Condition cond) {
double mid_y = cond.height / 2.0;
double mid_x = cond.width / 2.0;
system("clear");
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
    if (y == (int)mid_y + (int)objs[i].y && x == (int)mid_x + (int)objs[i].x && objs[i].m > 0) {
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
printf("t = %.2f                   \n", t);
int cnt_printed_objs = 0;//プリントする時にレイアウトを揃えるために使用
for (int i = 0; i < numobj; ++i) {
  if (objs[i].m < 0) {
    continue;
  }
  printf("objs[%d] = (%.2f, %.2f),                    \n", i, objs[i].x, objs[i].y);
  cnt_printed_objs++;
}
return cnt_printed_objs;
}

void merge(Object objs[], size_t numobj, const Condition cond) {
    for (int i = 0; i < numobj; ++i) {
      if (objs[i].m < 0) {
        continue;
      }
        for (int j = i + 1; j < numobj ; ++j) { //こうすることでi < jの関係を作れる
        if (objs[j].m < 0) {
          continue;
        }
                    double distance_x = objs[j].x - objs[i].x;
                    double distance_y = objs[j].y - objs[i].y;
                    double distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
                    if (distance < epsilon) {
                      //運動量保存則を用いて、速度を計算
                        double new_vx = (objs[i].m * objs[i].vx + objs[j].m * objs[j].vx) / (objs[j].m + objs[i].m);
                        double new_vy = (objs[i].m * objs[i].vy + objs[j].m * objs[j].vy) / (objs[j].m + objs[i].m);
                      //結合後の座標は、一応二物体の平均値を取るようにした
                        double new_x = (objs[i].x + objs[j].x)/2;
                        double new_y = (objs[i].y + objs[j].y)/2;
                        //結合後の質量は合計
                        double new_m = objs[j].m + objs[i].m;
                        Object new_object = {
                            .m = new_m, 
                            .x = new_x,
                            .y = new_y,
                            .vx = new_vx,
                            .vy = new_vy,
                            .prev_y = 0, //適当な値
                            .prev_x = 0
                        };
                        double tmp_prev_i[2] = {objs[i].prev_x, objs[i].prev_y};//prev_xとprev_yを残しておく
                        objs[i] = new_object;
                        //prev_x,prev_yを元に戻す
                        objs[i].prev_x = tmp_prev_i[0];
                        objs[i].prev_y = tmp_prev_i[1];
                        //質量を負にしてしまう(このプログラムの根幹!)
                        objs[j].m = -1;
        }
    }
}
}

