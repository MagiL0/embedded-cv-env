/*
 * @Author: FYB
 * @Description: GitHub: https://github.com/magil0
 */
/*
 * @Author: FYB
 * @Description: GitHub: https://github.com/magil0
 */
#ifndef __MAIN_H
#define __MAIN_H

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#define uint8_t u8
#define uint16_t u16
#define uint32_t u32

#define TRUE 1
#define FALSE 0

#define HEIGHT 120
#define WIDTH  192


extern _Bool DBG;

#define AT_IMAGE(img, x, y)          ((img)->data[((img)->step) * (y)+(x)])
#define AT                  AT_IMAGE

//img为一维图片数据指针 ，x,y为所查找像素点的位置，该宏可以避免越界
#define AT_IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))

//img为一维图片数据指针 ，x,y为所查找像素点的位置，该宏可以避免越界
#define AT_CLIP(img, x, y)  AT_IMAGE((img), clip((x), 0, (img)->width-1), clip((y), 0, (img)->height-1))


typedef struct image {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t step;
} image_t;



typedef struct fimage {
    float *data;
    uint32_t width;
    uint32_t height;
    uint32_t step;
} fimage_t;

#endif