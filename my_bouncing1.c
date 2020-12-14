//正確に物理現象を表したかったので、地球の存在は重力加速度として表した(地球の値を正確に実装しようとするとオーバーフロウしてしまった)
//なので、地球に当たる物体を用意することは想定していない
//Gは実際の値を代入した
//壁の反発係数と床の反発係数を別々に定義した

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
// #include "physics2.h"



/*真帆へ
//typedefでconditionとobjectをこのファイルの中で定義してるけど、include "physics.h"をしてればいらない(最終的にこれをコメントアウトするときに)
付け加えてくれれば大丈夫
*/
typedef struct condition
{
  const int width; // 見えている範囲の幅
  const int height; // 見えている範囲の高さ
  const double G; // 重力定数
  const double dt; // シミュレーションの時間幅
  const double cor_floor; // 床の反発係数
  const double cor_wall; //壁の反発係数
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
int main(int argc, char **argv)
{
  const Condition cond = {
            .g = 9.8,
		    .width  = 75,
		    .height = 40,
		    .G = 2.2 * 0.00001,
		    .dt = 0.1,
		    .cor_floor = 0.8,
        .cor_wall = 0.5
  };

  size_t objnum = 4;
  Object objects[objnum];

  objects[0] = (Object){ .m = 60.0, .y = -1.9, .vy = 0, .x = 0.0, .vx = 1.0};
  objects[1] = (Object){ .m = 10.0, .y = -4.9, .vy = 2, .x = 5, .vx = -2.0};
  objects[2] = (Object){ .m = 30.0, .y = -3.9, .vy = 0.3, .x = -5.0, .vx = 0.5};
  objects[3] = (Object){ .m = 70.0, .y = 9.9, .vy = -0.4, .x = 10.0, .vx = -0.5};

  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 400;
  double t = 0;
  printf("\n");
  for (int i = 0 ; t <= stop_time ; i++){
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum, cond);

    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);

    usleep(200 * 1000); // 200 x 1000us = 200 ms ずつ停止
    printf("\e[%dA", cond.height + 4 + objnum);// 壁とパラメータ表示分で3行
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
double acceleration_y = cond.g + F_y / objs[i].m; //冒頭でコメントしている通り、ここでは重力加速度を考慮している
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
for (int i = 0; i < numobj; ++i) { //床の反発の判定
  if ((objs[i].y - boarder_y) * (objs[i].prev_y - boarder_y) <= 0) { //ボーダーを跨いでいたら
    double time_after_conflict = (objs[i].y - boarder_y)  / objs[i].vy;
    objs[i].vy *= (- cond.cor_floor);
    objs[i].y = boarder_y + time_after_conflict * objs[i].vy;
    }
  }
for (int i = 0; i < numobj; ++i) {  //壁の反発の判定、左右の二通りある
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
    int obj_exist_or_not = 1; //その座標に何かしらの物体があれば1,なければ0
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
  printf("|\n");
}
printf("+");
for (int x = 0; x < cond.width ; ++x) {
  printf("-");
}
printf("+\n");
printf("t = %.1f\n", t);
for (int i = 0; i < numobj; ++i) {
  printf("objs[%d] = (%.2f, %.2f)                    \n", i, objs[i].x, objs[i].y);
  //桁数によって前の表示が残ってしまうので、空白を入れることで解消
}
}

// 最終的に phisics2.h 内の事前に用意された関数プロトタイプをコメントアウト
