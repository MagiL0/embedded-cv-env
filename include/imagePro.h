/*
 * @Author: FYB
 * @Description: GitHub: https://github.com/magil0
 */
/*
 * @Author: FYB
 * @Description: GitHub: https://github.com/magil0
 */

#ifndef __IMAGEPRO_H
#define __IMAGEPRO_H
#include <stdint.h>
#include "main.h"


void find_left_start(image_t *img, int block_size, int clip_value, int lx, int ly, int *rlx, int* rly);
void find_right_start(image_t *img, int block_size, int clip_value, int rx, int ry, int *rrx, int* rry);
void draw_graypts(image_t *img, uint8_t pts[][2], int length);
void draw_whitepts(image_t *img, uint8_t pts[][2], int length);
void draw_pts(image_t *img, uint8_t pts[][2], int length);
void draw_paper(image_t *img);
void adaptive_threshold(image_t *img0, image_t *img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value) ;
void findline_lefthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, uint8_t pts[][2], int *num) ;
void findline_righthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, uint8_t pts[][2], int *num) ;
void resample_points(uint8_t pts_in[][2], uint8_t num1, float pts_out[][2], int *num2, float dist);
void local_angle_points(float pts_in[][2], uint8_t num, float angle_out[], int dist);
void nms_angle(float angle_in[], uint8_t num, float angle_out[], int kernel);
int clip(int x, int low, int up);
void track_rightline(float pts_in[][2], unsigned char num, float pts_out[][2], int approx_num, unsigned char* num1, float dist);
void track_leftline(float pts_in[][2], unsigned char num, float pts_out[][2], int approx_num, unsigned char* num1, float dist);
#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}
#define ROI_IMAGE(img, x1, y1, w, h) {.data=&AT_IMAGE(img, x1, y1), .width=w, .height=h, .step=img.width}

// 深拷贝图片，img0和img1不可以指向相同图片
void clone_image(image_t *img0, image_t *img1);

// 清空图片
void clear_image(image_t *img);

// 基础二值化，img0和img1可以指向相同图片
void threshold(image_t *img0, image_t *img1, uint8_t thres, uint8_t low_value, uint8_t high_value);

// 自适应二值化，img0和img1不可指向相同图片
void adaptive_threshold(image_t *img0, image_t *img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value);

// 像素逻辑与，即都为255时才为255
void image_and(image_t *img0, image_t *img1, image_t *img2);

// 像素逻辑或，即都为255时才为255
void image_or(image_t *img0, image_t *img1, image_t *img2);

// 最小值降采样x2
void minpool2(image_t *img0, image_t *img1);

// 高斯滤波
void blur(image_t *img0, image_t *img1, uint32_t kernel);

// 3x3高斯滤波，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void blur3(image_t *img0, image_t *img1);

// 3x3 Sobel梯度计算，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void sobel3(image_t *img0, image_t *img1);

// 3x3腐蚀运算，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void erode3(image_t *img0, image_t *img1);

// 3x3膨胀运算，img0和img1不可以指向相同图片，最外圈像素点不参与计算
void dilate3(image_t *img0, image_t *img1);

// 图像变换，最近邻插值。img0和img1不可以指向相同图片（去畸变，投影变换等。使用opencv生成变换表）
// 未测试，可能有BUG
void remap(image_t *img0, image_t *img1, fimage_t *mapx, fimage_t *mapy);

// 大津法计算二值化阈值
uint16_t getOSTUThreshold(image_t *img, uint8_t MinThreshold, uint8_t MaxThreshold);

// 点集三角滤波
void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel);

// 绘制X
void draw_x(image_t *img, int x, int y, int len, uint8_t value);

// 绘制O
void draw_o(image_t *img, int x, int y, int radius, uint8_t value);

#endif