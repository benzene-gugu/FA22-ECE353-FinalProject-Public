/*
 * image.c
 *
 *      Author: Yunzhao Liu
 */
#ifndef IMAGE_H_
#define IMAGE_H_

// indices of the image
typedef enum {
	IMG_WAIT_1,
	IMG_VICTORY_1,
	IMG_VICTORY_2,
	IMG_ACCELERATE_1,
	IMG_ACCELERATE_2,
	IMG_ACCELERATE_3,
	IMG_RIGHT_1,
	IMG_RIGHT_2,
	IMG_RIGHT_3,
	IMG_RIGHT_4,
	IMG_RIGHT_5,
	IMG_KICK_1,
	IMG_KICK_2,
	IMG_PUNCH_1,
	IMG_PUNCH_2,
	IMG_HIT_1,
	IMG_TREE,
	IMG_FALL_1,
	IMG_FALL_2,
	IMG_FALL_3,
	IMG_FALL_4,
} ImageLabel;

typedef struct {
    uint16_t width, height;
    uint16_t xoffset, yoffset;
    const uint16_t* data;
} ImageInfo;

// image informations
extern const ImageInfo imageInfos[];
extern const uint16_t img_background[];

#endif /* IMAGE_H_ */
