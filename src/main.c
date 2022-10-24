/*
 * @Author: FYB
 * @Description: GitHub: https://github.com/magil0
 */

#include <stdio.h>
#include <stdlib.h>
#include "imagePro.h"
#include "main.h"
#include "common.h"
#include <time.h>

_Bool DBG = FALSE;

#define _CRT_SECURE_NO_WARNINGS

// 两个本地调用函数，为了移植性，避免其他图像处理函数使用，因此搞成静态
static void write_image(image_t* img, char* path);
static void import_image(image_t* img, char* path);

const int dir_front[4][2];
const int dir_frontleft[4][2];
const int dir_frontright[4][2];

float min_angler;
float min_anglel;
float max_angler;
float max_anglel;
int min_angler_num;
int min_anglel_num;
int max_angler_num;
int max_anglel_num;

uint8_t ptsl[200][2];
uint8_t ptsr[200][2];
int ptsl_num = 200;
int ptsr_num = 200;

float rptsl[200][2];
float rptsr[200][2];
int rptsl_num = 200;
int rptsr_num = 200;

float anglesr[200];
float anglesl[200];

uint8_t data[HEIGHT][WIDTH] = {0};
uint8_t blank_data[HEIGHT][WIDTH] = {0};
uint8_t copy_data[HEIGHT][WIDTH] = {0};
image_t image_raw = {
    (uint8_t *)data,
    WIDTH,
    HEIGHT,
    WIDTH
};
image_t image_blank = {
    (uint8_t *)blank_data,
    WIDTH,
    HEIGHT,
    WIDTH
};
image_t image_copy = {
    (uint8_t *)copy_data,
    WIDTH,
    HEIGHT,
    WIDTH
};

int rpt[2] = {0};
int lpt[2] = {0};

int main() 
{
    // 计算运算时间
    clock_t start, finish;

    // call python to get image
    system("python.exe ..\\scripts\\transphoto.py ..\\data\\1.jpg");

    // import image to image_raw
    import_image(&image_raw, "..\\data\\1.txt");

    start = clock();
    // adaptive_threshold(&image_raw, &image_blank, 3, 9, 0, 255);  

    // clone_image(&image_blank, &image_raw);
    clone_image(&image_raw, &image_blank);

    // blur(&image_blank, &image_raw, 5);


    // 找左左右侧到中间的白线
    find_left_start(&image_blank, 3, 9, 189, HEIGHT - 10, rpt, rpt + 1);
    // printf("find point: (%d, %d)\n", rpt[0], rpt[1]);
    findline_righthand_adaptive(&image_blank, 3, 8, rpt[0], rpt[1], ptsr, &ptsr_num);
    find_right_start(&image_blank, 3, 9, 4, HEIGHT - 10, lpt, lpt + 1);
    // printf("find point: (%d, %d)\n", rpt[0], rpt[1]);
    findline_lefthand_adaptive(&image_blank, 3, 8, lpt[0], lpt[1], ptsl, &ptsl_num);

    printf(">> find left line pixels: %d\n", ptsr_num);
    printf(">> find right line pixels: %d\n", ptsl_num);
    draw_paper(&image_copy);
    draw_pts(&image_copy, ptsr, ptsr_num);
    draw_pts(&image_copy, ptsl, ptsl_num);
    
    // 等距采样
    resample_points(ptsr,ptsr_num,rptsr,&rptsr_num, 3);// 等间距采样  左线   rptsa0
    resample_points(ptsl,ptsl_num,rptsl,&rptsl_num, 3);// 等间距采样  右线  rptsa1
    // {
    //     float max0 = 0, max1 = 0;
    //     float min0 = 120, min1 = 120;
    //     unsigned char i = 0;
    //     for (i = 0; i < rptsa0_num; i++) {
    //         max0 = (max0 > rptsa0[i][1]) ? max0 : rptsa0[i][1];
    //         min0 = (min0 < rptsa0[i][1]) ? max0 : rptsa0[i][1];
    //     }
    //     for (i = 0; i < rptsa1_num; i++) {
    //         max1 = (max1 > rptsa1[i][1]) ? max1 : rptsa1[i][1];
    //         min1 = (min1 < rptsa1[i][1]) ? max1 : rptsa1[i][1];
    //     }
    //     if (max0 - min0 < 5.) rptsa0_num = 0;
    //     if (max1 - min1 < 5.) rptsa1_num = 0;
    // }
    printf(">> resampled points in left line: %d \n", rptsl_num);
    printf(">> resampled points in right line: %d \n", rptsr_num);

    // 寻找角点
    local_angle_points(rptsr, rptsr_num, anglesr, 5);
    local_angle_points(rptsl, rptsl_num, anglesl, 5);

    min_angler = min_anglel = 255;
    max_angler = max_anglel = 0;
    for (int i = 0; i < rptsl_num; i++) {
        
        if (anglesl[i] < min_anglel) {
            min_anglel = anglesl[i];
            min_anglel_num = i;
        }
        if (anglesl[i] > max_anglel) {
            max_anglel = anglesl[i];
            max_anglel_num = i;
        }
    }
    for (int i = 0; i < rptsr_num; i++) {
        if (anglesr[i] > max_angler) {
            max_angler = anglesr[i];
            max_angler_num = i;
        }
        if (anglesr[i] < min_angler) {
            min_angler = anglesr[i];
            min_angler_num = i;
        }
    }
    printf(">> find max angle %frad in left line at pixel (%d,%d)\n", max_anglel, (int)rptsl[max_anglel_num][0],(int) rptsl[max_anglel_num][1]);
    printf(">> find max angle %frad in right line at pixel (%d,%d)\n", max_angler, (int)rptsr[max_angler_num][0],(int) rptsr[max_angler_num][1]);

    draw_o(&image_copy, (int) rptsr[max_angler_num][0], (int) rptsr[max_angler_num][1], 3, 0);
    draw_o(&image_copy, (int) rptsl[max_anglel_num][0], (int) rptsl[max_anglel_num][1], 3, 0);

    finish = clock();
    // 显示运行时间
    printf(">> time costed: %lf\n", (double)(finish - start) / CLOCKS_PER_SEC);
    // write_image to txt ,which can be loaded by numpy with np.loadtxt()
    write_image(&image_copy, "..\\data\\1o.txt");

    // printf("%d, %d\n", image_blank.data, image_raw.data);
    system("python.exe ..\\scripts\\showphoto.py ..\\data\\1o.txt");
}




/**
 * @description: 输入图像，避免其他图像处理函数使用，因此搞成静态
 * @param {image_t*} img 注意，其中内容必须已经初始化过
 * @param {char*} path 文件路径带名称
 * @return {*}
 */
static void import_image(image_t* img, char* path)
{
    FILE * fp;
    errno_t err;
    err = fopen_s(&fp, path, "r");

    // 错误处理
    if (err == 1) {
        printf(">> Error open reading file %s in .c!\n", path);
        exit(0);
    }

    // 创建临时变量
    uint8_t* temp_img = img->data;
    uint8_t temp_str[10] = {};
    char temp_ch = 0;
    _Bool number_flag = 0;
    int temp_int = 0, temp_count = 0;
    // 直到文件结尾
    while((temp_ch = getc(fp)) != EOF) {
        // 如果是数字，添加暂存变量中数字移位相加，标记读到数字
        if (temp_ch >= '0' && temp_ch <= '9') {
            temp_int = temp_int * 10 + (temp_ch - '0');
            number_flag = TRUE;
        }
        // 如果是逗号，说明结束数字，清零暂存变量，储存暂存变量到图像数组
        if (temp_ch == ',' || temp_ch == '\n' || temp_ch == ' ') {
            if (number_flag == TRUE) {
                img->data[temp_count] = temp_int;
                temp_int = 0;
                temp_count ++;
            }
            number_flag = FALSE;
        }
    }
    fclose(fp);
    printf(">> import image done!\n");
}


/**
 * @description: 输入图像，避免其他图像处理函数使用，因此搞成静态
 * @param {image_t*} img 注意，其中内容必须已经初始化过
 * @param {char*} path 文件路径带名称
 * @return {*}
 */
static void write_image(image_t* img, char* path)
{
    FILE * fp;
    errno_t err;
    err = fopen_s(&fp, path, "w");

    // 错误处理
    if (err == 1) {
        printf(">> Error open writting file %s in .c!\n", path);
        exit(0);
    }

    for (int j = 0; j < img->height; j++) {
        for (int i = 0; i < img->width; i++) {
            fprintf(fp, "%4u", AT_IMAGE(img, i, j));
        }
        fputc('\n',fp);
    }
    fclose(fp);
    printf(">> write image done!\n");
}

