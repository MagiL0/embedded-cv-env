/*
 * @Author: 28x wither1260407765@outlook.com
 * @Date: 2022-08-12 15:30:27
 */

#include <stdlib.h>
#include <string.h>
#include "imagePro.h"
#include "stdio.h"
#include "math.h"
#include "main.h"
#include "common.h"


#define PI 3.1415

extern const int dir_front[4][2];
extern const int dir_frontleft[4][2];
extern const int dir_frontright[4][2];



int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}

//清零为白色
void draw_paper(image_t *img){
    int i = 0, j = 0;
    for (i = 0; i < img ->width; i++){
        for (j = 0; j < img ->height; j++){
            AT(img,i,j) = 255;
        }
    }
}
//在白纸上画黑线
void draw_pts(image_t *img, uint8_t pts[][2], int length){
    int i = 0;
    for (i = 0; i < length; i++){
        AT(img, pts[i][0], pts[i][1]) = 0;
    }
}
//在白纸上画白线
void draw_whitepts(image_t *img, uint8_t pts[][2], int length){
    int i = 0;
    for (i = 0; i < length; i++){
        AT(img, pts[i][0], pts[i][1]) = 255;
    }
}
//在白纸上画灰线
void draw_graypts(image_t *img, uint8_t pts[][2], int length){
    int i = 0;
    for (i = 0; i < length; i++){
        AT(img, pts[i][0], pts[i][1]) = 120;
    }
}


// 找到左右起点, rx,ry为右侧开始寻找的点，rrx，rry为右侧找到的点
/**
 * @description: rx,ry为右侧开始寻找的点，rrx，rry为右侧找到的点
 * @param {image_t} *img
 * @param {int} block_size
 * @param {int} clip_value
 * @param {int} lx
 * @param {int} ly
 * @param {int} *rlx
 * @param {int*} rly
 * @return {*}
 */
void find_left_start(image_t *img, int block_size, int clip_value, int lx, int ly, int *rlx, int* rly) {
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0, count_height = 0, limit = 3, temprx = lx, templx = lx;

       //printf
    // printf("%d,%d,%d,%d,%d\n",
    //         step < img-> width / 2,
    //         half < lx,
    //         lx < img->width -half- 1,
    //         half < ly,
    //         ly < img->height -half- 1);


    while (step < img-> width / 2
                   && half < lx
                   && lx < img->width -half- 1
                   && half < ly
                   && ly < img->height -half- 1)
    {

        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, lx + dx, ly + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        if (AT(img, lx, ly) >= local_thres) {//如果大于，是白色
            lx -= 1;
            // printf("4");
        }
        else {
            *rlx = lx;
            *rly = ly;
            // printf("n");
            break;
        }
        if (lx >= img->width -half- 1 && count_height < limit) {
            count_height++;
            ly --;//`向下寻找一行，不超过limit
            lx = templx;
            // printf("5");
        }
    }

        // printf("count_height: %d\n", count_height);

}


// 找到左右起点, rx,ry为右侧开始寻找的点，rrx，rry为右侧找到的点
/**
 * @description:  找到左右起点, lx,ly为开始寻找的点，rlx，rly为找到的点
 * @param {image_t} *img
 * @param {int} block_size
 * @param {int} clip_value
 * @param {int} lx
 * @param {int} ly
 * @param {int} *rlx
 * @param {int*} rly
 * @return {*}
 */
void find_right_start(image_t *img, int block_size, int clip_value, int rx, int ry, int *rrx, int* rry) {
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0, count_height = 0, limit = 3, temprx = rx, templx = rx;

    //找右侧x,y
    while (step < img-> width / 2
                && half < rx
                && rx < img->width -half- 1
                && half < ry
                && ry < img->height -half- 1)
    {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, rx + dx, ry + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        if (AT(img, rx, ry) >= local_thres) {//如果大于，是白色
            rx += 1;
//                printf("1");
        }
        else {
            *rrx = rx;
            *rry = ry;
//                printf("y");
            break;
        }
        if (rx >= img->width -half- 1 && count_height < limit) {
            count_height++;
            ry --;//`向下寻找一行，不超过limit
            rx = temprx;
//                printf("2");
        }
    }

}


/**
 * @description: 霍夫变换
 * @param {int*} pPoints
 * @param {int} pointNum
 * @param {int} RMax
 * @param {int} &lineR
 * @param {int} &lineTheta
 * @return {*}
 */
_Bool HoughTrans(int* pPoints,int pointNum,int RMax,int* lineR,int* lineTheta)
{
	//x*cos(theta)+y*sin(theta)=r;
	int k,maxValue;
	int *pAccumulateArr=NULL;		//累加数组
	int thetaMax=360;
	int theta;
	int rValue;
	float fRate = (float)(PI/180);
	int AccuArrLength=(thetaMax+1)*(RMax+1);
	
	pAccumulateArr=(int*)malloc(AccuArrLength*sizeof(int));
	memset(pAccumulateArr,0,AccuArrLength*sizeof(int));
	for (k=0;k<pointNum;k++)
	{
		for (theta=0;theta<=thetaMax;theta++)
		{
			rValue=(float)(pPoints[k*2]) * cos(theta * fRate) + (float)(pPoints[k*2+1]) * sin(theta * fRate)+0.5;
 
			if (rValue>=0)
			{
				int indexTemp=rValue*(thetaMax+1)+theta;
				if (rValue<=RMax && indexTemp>=0)
				{
					pAccumulateArr[indexTemp]++;
				}
			}
		}
	}
	
	//得到最佳参数
	maxValue=0;
	*lineR=0;
	*lineTheta=0;
	for (rValue=0;rValue<=RMax;rValue++)
	{
		for (theta=0;theta<=thetaMax;theta++)
		{
			int iCount = pAccumulateArr[rValue * (thetaMax+1) + theta];
			if (maxValue<iCount)
			{
				maxValue=iCount;
				*lineR=rValue;
				*lineTheta=theta;
			}
		}
	}
	
	free(pAccumulateArr);
	pAccumulateArr=0;
	if (maxValue!=0)
	{
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//自适应阈值二值化
void adaptive_threshold(image_t *img0, image_t *img1, int block_size, int down_value, uint8_t low_value, uint8_t high_value) {
    // // assert(img0 && img0->data);
    // // assert(img1 && img1->data);
    // // assert(img0->data != img1->data);
    // // assert(img0->width == img1->width && img0->height == img1->height);
    // // assert(block_size > 1 && block_size % 2 == 1);

    int half = block_size / 2;
    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y ++) {
        for (int x = 0; x < img0->width; x++) {
            int thres_value = 0;
            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    thres_value += AT_CLIP(img0, x + dx, y + dy);
                }
            }
            thres_value /= block_size * block_size;
            thres_value -= down_value;
            AT(img1, x, y) = AT(img0, x, y) < thres_value ? low_value : high_value;
        }
    }
}

// 左手迷宫巡线
void findline_lefthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, uint8_t pts[][2], int *num) {
    // // assert(img && img->data);
    // // assert(num && *num >= 0);
    // // assert(block_size > 1 && block_size % 2 == 1);
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0, count_white = 0;

    //printf
    // printf("%d,%d,%d,%d,%d,%d\n",
    //         step < *num ,
    //         half < x ,
    //         x < img->width - half - 1 ,
    //         half < y ,
    //         y < img->height - half - 1 ,
    //         turn < 4);

    
    while (step < *num 
            && half < x 
            && x < img->width - half - 1 
            && half < y 
            && y < img->height - half - 1 
            && turn < 4) 
    {
        int local_thres = 0;
        //求自区域适应阈值，大小为 block_size * block_size，结果为 local_thres
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;


        //计算该点前方，左前方，右前方的点是否是黑的
        int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);

        //添加修正，使用count_white如果周围都是白线就向右找
        if (count_white >= 4) {
            step =0; //没找到黑线，清零
            x -= 1; //向左走
            if(x >= img->width || x < 0 || y >= (img->height) || y < 0) {
                return;
            }
            count_white = 0;
            // printf("5");
            turn = 0;
            dir = 0;
        }


        if (front_value < local_thres) {//如果前面是黑色需要右转一次
            dir = (dir + 1) % 4;//防止大于3，方向为 0123
            turn++;
            count_white = 0;
            // printf("1");
        } else if (frontleft_value < local_thres) {//如果前方像素为白色，且左前方像素为黑色
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;//重写黑边
            pts[step][1] = y;
            step++;
            turn = 0;
            count_white = 0;
            // printf("2");
        } else {//前方为白色，左前方为白色（墙角）
            x += dir_frontleft[dir][0];// 遇到墙角要斜着走
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;// 遇到墙角要左转一次
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            count_white ++;
            // printf("3");
        }
    }
    *num = step;

}

// 右手迷宫巡线
void findline_righthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, uint8_t pts[][2], int *num) {
    // // assert(img && img->data);
    // // assert(num && *num >= 0);
    // // assert(block_size > 1 && block_size % 2 == 1);
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0, count_white = 0;

     //printf
    // printf("%d,%d,%d,%d,%d,%d\n",
    //         step < *num ,
    //         half < x ,
    //         x < img->width - half - 1 ,
    //         half < y ,
    //         y < img->height - half - 1 ,
    //         turn < 4);

    //求自区域适应阈值，大小为 block_size * block_size
    while (step < *num 
            && 0 < x 
            && x < img->width - 1 
            && 0 < y 
            && y < img->height - 1 
            && turn < 4) 
    {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);

        //添加修正，使用count_white如果周围都是白线就向右找
        if (count_white >= 4) {
            step =0; //没找到黑线，清零
            x += 1; //向右走
            if(x >= (img->width) || x < 0 || y >= (img->height) || y < 0) {
                return;
            }
            count_white = 0;
            // printf("5");
            turn = 0;
            dir = 0;
        }

        if (front_value < local_thres) {
            dir = (dir + 3) % 4;
            turn++;
            // printf("1");
            count_white  = 0;
        } else if (frontright_value < local_thres) {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            count_white  = 0;
            // printf("2");
        } else {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            // printf("3");
            count_white ++;
        }
    }
    *num = step;
}



/**
 * @description: 等距采样，使采样后的点几何距离相等
 * @param {uint8_t} pts_in[][2] 需要采样的线
 * @param {uint8_t} num1            被采样线长度
 * @param {float} pts_out[][2]  采样结果
 * @param {uint8_t} *num2          采样出的线的长度的返回值
 * @param {float} dist          指定的采样点间隔
 * @return {*}
 */
void resample_points(uint8_t pts_in[][2], uint8_t num1, float pts_out[][2], int *num2, float dist)
{
    int remain = 0, len = 0;
    for(int i=0; i<num1-1 && len < *num2; i++){
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float dx = pts_in[i+1][0] - x0;
        float dy = pts_in[i+1][1] - y0;
        float dn = sqrt(dx*dx+dy*dy);
        dx /= dn;
        dy /= dn;

        while(remain < dn && len < *num2){
            x0 += dx * remain;
            pts_out[len][0] = x0;
            y0 += dy * remain;
            pts_out[len][1] = y0;
            
            len++;
            dn -= remain;
            remain = dist;
        }
        remain -= dn;
    }
    *num2 = len;
}

/**
 * @description: 局部角度变化率计算
 * @param {float} pts_in[][2]   等距采样后的线
 * @param {uint8_t} num             等距采样后线的长度
 * @param {float} angle_out[]     输出的角度集合
 * @param {int} dist            用于计算角度的点之间间隔dist - 1个点
 * @return {*}
 */
void local_angle_points(float pts_in[][2], uint8_t num, float angle_out[], int dist) 
{
    for (int i = 0; i < num; i++) {
        if (i <= 0 || i >= num - 1) {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = acosf((dx1 * dx2 + dy1 * dy2) / (dn1) / dn2);
        // angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}


/**
 * @description: 角度非极大值抑制
 * @param {float} angle_in[]   输入的角度数组
 * @param {uint8_t} num             输入数组长度
 * @param {float} angle_out[]  输出的角度数组
 * @param {int} kernel          非极大值抑制的范围
 * @return {*}
 */
void nms_angle(float angle_in[], uint8_t num, float angle_out[], int kernel) {
    // // assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
    }
}



/**
 * @description: 左边线跟踪中线
 * @param {float} pts_in
 * @param {unsigned char} num
 * @param {float} pts_out
 * @param {int} approx_num  极限大小，代表前后几个点
 * @param (unsigned char) * num1
 * @param {float} dist      距离     
 * @return {*}
 */
void track_leftline(float pts_in[][2], unsigned char num, float pts_out[][2], int approx_num, unsigned char* num1, float dist) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;  
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] - dy * dist;
        pts_out[i][1] = pts_in[i][1] + dx * dist;
    }
    *num1 = num;

}

/**
 * @description: 右边线跟踪中线
 * @param {float} pts_in
 * @param {unsigned char} num
 * @param {float} pts_out
 * @param {int} approx_num  极限大小，代表前后几个点
 * @param (unsigned char) * num1
 * @param {float} dist      距离     
 * @return {*}
 */
void track_rightline(float pts_in[][2], unsigned char num, float pts_out[][2], int approx_num, unsigned char* num1, float dist) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] + dy * dist;
        pts_out[i][1] = pts_in[i][1] - dx * dist;
    }
    *num1 = num;

}









void find_corners(void);
void process_image(void);




void clone_image(image_t *img0, image_t *img1) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0 != img1 && img0->data != img1->data);

    if (img0->width == img0->step && img1->width == img1->step) {
        memcpy(img1->data, img0->data, img0->width * img0->height);
    } else {
        for (int y = 0; y < img0->height; y++) {
            memcpy(&AT(img1, 0, y), &AT(img0, 0, y), img0->width);
        }
    }
}

void clear_image(image_t *img) {
    // assert(img && img->data);
    if (img->width == img->step) {
        memset(img->data, 0, img->width * img->height);
    } else {
        for (int y = 0; y < img->height; y++) {
            memset(&AT(img, 0, y), 0, img->width);
        }
    }
}

// 固定阈值二值化
void threshold(image_t *img0, image_t *img1, uint8_t thres, uint8_t low_value, uint8_t high_value) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
                    AT(img1, x, y) = AT(img0, x, y) < thres ? low_value : high_value;
        }
    }
}


// 图像逻辑与
void image_and(image_t *img0, image_t *img1, image_t *img2) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img2 && img2->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0->width == img2->width && img0->height == img2->height);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
                    AT(img2, x, y) = (AT(img0, x, y) == 0 || AT(img1, x, y) == 0) ? 0 : 255;
        }
    }
}

// 图像逻辑或
void image_or(image_t *img0, image_t *img1, image_t *img2) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img2 && img2->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0->width == img2->width && img0->height == img2->height);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 0; y < img0->height; y++) {
        for (int x = 0; x < img0->width; x++) {
                    AT(img2, x, y) = (AT(img0, x, y) == 0 && AT(img1, x, y) == 0) ? 0 : 255;
        }
    }
}

// 2x2最小池化(赛道边界是黑色，最小池化可以较好保留赛道边界)
void minpool2(image_t *img0, image_t *img1) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width / 2 == img1->width && img0->height / 2 == img1->height);
    // assert(img0 != img1 && img0->data != img1->data);

    uint8_t min_value;
    // 先遍历y后遍历x比较cache-friendly
    for (int y = 1; y < img0->height; y += 2) {
        for (int x = 1; x < img0->width; x += 2) {
            min_value = 255;
            if (AT(img0, x, y) < min_value) min_value = AT(img0, x, y);
            if (AT(img0, x - 1, y) < min_value) min_value = AT(img0, x - 1, y);
            if (AT(img0, x, y - 1) < min_value) min_value = AT(img0, x, y - 1);
            if (AT(img0, x - 1, y - 1) < min_value) min_value = AT(img0, x - 1, y - 1);
                    AT(img1, x / 2, y / 2) = min_value;
        }
    }
}

// 图像滤波降噪
void blur(image_t *img0, image_t *img1, uint32_t kernel) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0 != img1 && img0->data != img1->data);

    // 先遍历y后遍历x比较cache-friendly
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
                    AT(img1, x, y) = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
                                      2 * AT(img0, x - 1, y) + 4 * AT(img0, x, y) + 2 * AT(img0, x + 1, y) +
                                      1 * AT(img0, x - 1, y + 1) + 2 * AT(img0, x, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 16;
        }
    }
}

// 3x3sobel边缘提取
void sobel3(image_t *img0, image_t *img1) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0 != img1 && img0->data != img1->data);

    int gx, gy;
    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
            gx = (-1 * AT(img0, x - 1, y - 1) + 1 * AT(img0, x + 1, y - 1) +
                  -2 * AT(img0, x - 1, y) + 2 * AT(img0, x + 1, y) +
                  -1 * AT(img0, x - 1, y + 1) + 1 * AT(img0, x + 1, y + 1)) / 4;
            gy = (1 * AT(img0, x - 1, y - 1) + 2 * AT(img0, x, y - 1) + 1 * AT(img0, x + 1, y - 1) +
                  -1 * AT(img0, x - 1, y + 1) - 2 * AT(img0, x, y + 1) - 1 * AT(img0, x + 1, y + 1)) / 4;
                    AT(img1, x, y) = (abs(gx) + abs(gy)) / 2;
        }
    }
}

// 3x3腐蚀
void erode3(image_t *img0, image_t *img1) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0 != img1 && img0->data != img1->data);


    int min_value;
    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
            min_value = 255;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (AT_CLIP(img0, x + dx, y + dy) < min_value) min_value = AT_CLIP(img0, x + dx, y + dy);
                }
            }
            AT_CLIP(img1, x, y) = min_value;
        }
    }
}

// 3x3膨胀
void dilate3(image_t *img0, image_t *img1) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(img0 != img1 && img0->data != img1->data);

    int max_value;
    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
            max_value = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (AT(img0, x + dx, y + dy) > max_value) max_value = AT(img0, x + dx, y + dy);
                }
            }
                    AT(img1, x, y) = max_value;
        }
    }
}

//
void remap(image_t *img0, image_t *img1, fimage_t *mapx, fimage_t *mapy) {
    // assert(img0 && img0->data);
    // assert(img1 && img1->data);
    // assert(mapx && mapx->data);
    // assert(mapy && mapy->data);
    // assert(img0 != img1 && img0->data != img1->data);
    // assert(img0->width == img1->width && img0->height == img1->height);
    // assert(mapx->width == mapy->width && mapx->height == mapy->height);
    // assert(img0->width == mapx->width && img0->height == mapx->height);

    // 先遍历y后遍历x比较cache-friendly    
    for (int y = 1; y < img0->height - 1; y++) {
        for (int x = 1; x < img0->width - 1; x++) {
                    AT(img1, x, y) = AT(img0, (int) (AT(mapx, x, y) + 0.5), (int) (AT(mapy, x, y) + 0.5));
        }
    }
}

/* 前进方向定义：
 *   0
 * 3   1
 *   2
 */
const int dir_front[4][2] = {{0,  -1},
                                {1,  0},
                                {0,  1},
                                {-1, 0}};
const int dir_frontleft[4][2] = {{-1, -1},
                                {1,  -1},
                                {1,  1},
                                {-1, 1}};
const int dir_frontright[4][2] = {{1,  -1},
                                {1,  1},
                                {-1, 1},
                                {-1, -1}};


// 折线段近似
void approx_lines(int pts[][2], int pts_num, float epsilon, int lines[][2], int *lines_num) {
    // assert(pts);
    // assert(epsilon > 0);

    int dx = pts[pts_num - 1][0] - pts[0][0];
    int dy = pts[pts_num - 1][1] - pts[0][1];
    float nx = -dy / sqrtf(dx * dx + dy * dy);
    float ny = dx / sqrtf(dx * dx + dy * dy);
    float max_dist = 0, dist;
    int idx = -1;
    for (int i = 1; i < pts_num - 1; i++) {
        dist = fabs((pts[i][0] - pts[0][0]) * nx + (pts[i][1] - pts[0][1]) * ny);
        if (dist > max_dist) {
            max_dist = dist;
            idx = i;
        }
    }
    if (max_dist >= epsilon) {
        int num1 = *lines_num;
        approx_lines(pts, idx + 1, epsilon, lines, &num1);
        int num2 = *lines_num - num1 - 1;
        approx_lines(pts + idx, pts_num - idx, epsilon, lines + num1 - 1, &num2);
        *lines_num = num1 + num2 - 1;
    } else {
        lines[0][0] = pts[0][0];
        lines[0][1] = pts[0][1];
        lines[1][0] = pts[pts_num - 1][0];
        lines[1][1] = pts[pts_num - 1][1];
        *lines_num = 2;
    }
}

// float类型的折线段近似
void approx_lines_f(float pts[][2], int pts_num, float epsilon, float lines[][2], int *lines_num) {
    // assert(pts);
    // assert(epsilon > 0);

    int dx = pts[pts_num - 1][0] - pts[0][0];
    int dy = pts[pts_num - 1][1] - pts[0][1];
    float nx = -dy / sqrtf(dx * dx + dy * dy);
    float ny = dx / sqrtf(dx * dx + dy * dy);
    float max_dist = 0, dist;
    int idx = -1;
    for (int i = 1; i < pts_num - 1; i++) {
        dist = fabs((pts[i][0] - pts[0][0]) * nx + (pts[i][1] - pts[0][1]) * ny);
        if (dist > max_dist) {
            max_dist = dist;
            idx = i;
        }
    }
    if (max_dist >= epsilon && *lines_num > 2) {
        int num1 = *lines_num;
        approx_lines_f(pts, idx + 1, epsilon, lines, &num1);
        int num2 = *lines_num - num1 - 1;
        approx_lines_f(pts + idx, pts_num - idx, epsilon, lines + num1 - 1, &num2);
        *lines_num = num1 + num2 - 1;
    } else {
        lines[0][0] = pts[0][0];
        lines[0][1] = pts[0][1];
        lines[1][0] = pts[pts_num - 1][0];
        lines[1][1] = pts[pts_num - 1][1];
        *lines_num = 2;
    }
}

void draw_line(image_t *img, int pt0[2], int pt1[2], uint8_t value) {
    int dx = pt1[0] - pt0[0];
    int dy = pt1[1] - pt0[1];
    if (abs(dx) > abs(dy)) {
        for (int x = pt0[0]; x != pt1[0]; x += (dx > 0 ? 1 : -1)) {
            int y = pt0[1] + (x - pt0[0]) * dy / dx;
                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    } else {
        for (int y = pt0[1]; y != pt1[1]; y += (dy > 0 ? 1 : -1)) {
            int x = pt0[0] + (y - pt0[1]) * dx / dy;
                    AT(img, clip(x, 0, img->width - 1), clip(y, 0, img->height - 1)) = value;
        }
    }
}

// 计算大津阈值
uint16_t getOSTUThreshold(image_t *img, uint8_t MinThreshold, uint8_t MaxThreshold) {
    uint8_t Histogram[256];
    uint16_t OUSTThreshold = 0;
    uint32_t PixelAmount = 0, Value_Sum = 0;
    uint64_t sigma = 0, maxSigma = 0;
    float w1 = 0, w2 = 0;
    int32_t u1 = 0, u2 = 0;
    uint8_t MinValue = 0, MaxValue = 255;

    //各像素点个数
    uint8_t *ptr = img->data;
    uint8_t *ptrEnd = img->data + img->width * img->height;
    while (ptr != ptrEnd) {
        ++Histogram[*ptr++];
    }

    for (uint8_t m = 0; m < 100; m++) {

        Histogram[m] = 0;
    }

    for (MinValue = 0; Histogram[MinValue] == 0 && MinValue < 255; ++MinValue);
    for (MaxValue = 255; Histogram[MaxValue] == 0 && MaxValue > 0; --MaxValue);

    if (MaxValue == MinValue) return MaxValue;         // 只有一个颜色
    if (MinValue + 1 == MaxValue) return MinValue;        // 只有二个颜色

    if (MinValue < MinThreshold) {
        MinValue = MinThreshold;
    }
    if (MaxValue > MaxThreshold) {
        MaxValue = MaxThreshold;
    }

    uint32_t Pixel_Integral[256] = {0};   //像素积分 
    uint32_t Value_Integral[256] = {0};    //灰度积分
    for (uint8_t i = MinValue; i <= MaxValue; ++i) {
        PixelAmount += Histogram[i];      //像素总数
        Value_Sum += Histogram[i] * i;     //灰度总和
        Pixel_Integral[i] = PixelAmount;
        Value_Integral[i] = Value_Sum;
    }
    for (uint8_t i = MinValue; i < MaxValue + 1; ++i) {
        w1 = (float) Pixel_Integral[i] / PixelAmount;  //前景像素点比例
        w2 = 1 - w1;                               //背景比例
        u1 = (int32_t) (Value_Integral[i] / w1);                   //前景平均灰度
        u2 = (int32_t) ((Value_Sum - Value_Integral[i]) / w2);      //背景平均灰度
        sigma = (uint64_t) (w1 * w2 * (u1 - u2) * (u1 - u2));
        if (sigma >= maxSigma) {
            maxSigma = sigma;
            OUSTThreshold = i;
        } else {
            break;
        }
    }
    return OUSTThreshold;
}

// 点集三角滤波
void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel) {
    // assert(kernel % 2 == 1);
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++) {
            pts_out[i][0] += pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
            pts_out[i][1] += pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}


// 点集等距采样2  使采样后点与点的距离为`dist`
// TODO: fix bug
void resample_points2(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist) {
    if (num1 < 0) {
        *num2 = 0;
        return;
    }
    pts_out[0][0] = pts_in[0][0];
    pts_out[0][1] = pts_in[0][1];
    int len = 1;
    for (int i = 0; i < num1 - 1 && len < *num2; i++) {
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float x1 = pts_in[i + 1][0];
        float y1 = pts_in[i + 1][1];

        do {
            float x = pts_out[len - 1][0];
            float y = pts_out[len - 1][1];

            float dx0 = x0 - x;
            float dy0 = y0 - y;
            float dx1 = x1 - x;
            float dy1 = y1 - y;

            float dist0 = sqrt(dx0 * dx0 + dy0 * dy0);
            float dist1 = sqrt(dx1 * dx1 + dy1 * dy1);

            float r0 = (dist1 - dist) / (dist1 - dist0);
            float r1 = 1 - r0;

            if (r0 < 0 || r1 < 0) break;
            x0 = x0 * r0 + x1 * r1;
            y0 = y0 * r0 + y1 * r1;
            pts_out[len][0] = x0;
            pts_out[len][1] = y0;
            len++;
        } while (len < *num2);

    }
    *num2 = len;
}


void draw_x(image_t *img, int x, int y, int len, uint8_t value) {
    for (int i = -len; i <= len; i++) {
                AT(img, clip(x + i, 0, img->width - 1), clip(y + i, 0, img->height - 1)) = value;
                AT(img, clip(x - i, 0, img->width - 1), clip(y + i, 0, img->height - 1)) = value;
    }
}

void draw_o(image_t *img, int x, int y, int radius, uint8_t value) {
    for (float i = -PI; i <= PI; i += PI / 10) {
                AT(img, clip(x + radius * cosf(i), 0, img->width - 1), clip(y + radius * sinf(i), 0, img->height - 1)) = value;
    }
}
