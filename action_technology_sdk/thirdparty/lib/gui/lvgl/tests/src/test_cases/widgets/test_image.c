#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

LV_IMAGE_DECLARE(test_img_lvgl_logo_png);
LV_IMAGE_DECLARE(test_arc_bg);

void setUp(void)
{

}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * img_create(void)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_img_lvgl_logo_png);
    lv_obj_set_style_bg_opa(img, LV_OPA_20, 0);
    lv_obj_set_style_bg_color(img, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_width(img, 10, 0);
    lv_obj_set_style_shadow_color(img, lv_color_hex(0xff0000), 0);
    return img;
}

void test_image_rotated_pivot_center(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 100 + (i % 4) * 160, 150 + (i / 4) * 150);
        lv_image_set_rotation(img, i * 450);
        /*The default pivot should be the center*/
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_rotate_pivot_center.png");
}

void test_image_rotated_pivot_top_left(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 120 + (i % 4) * 180, 100 + (i / 4) * 300);
        lv_image_set_rotation(img, i * 450);
        lv_image_set_pivot(img, 0, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_rotate_pivot_top_left.png");
}

void test_image_scale_pivot_center(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 40 + (i % 4) * 200, 150 + (i / 4) * 150);
        lv_image_set_scale(img, 64 + i * 64);
        /*The default pivot should be the center*/
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_scale_pivot_center.png");
}

void test_image_scale_pivot_top_left(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 10 + (i % 4) * 190, 150 + (i / 4) * 150);
        lv_image_set_scale(img, 64 + i * 64);
        lv_image_set_pivot(img, 0, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_scale_pivot_top_left.png");
}

void test_image_scale_x_pivot_center(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 40 + (i % 4) * 200, 150 + (i / 4) * 150);
        lv_image_set_scale_x(img, 64 + i * 64);
        /*The default pivot should be the center*/
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_scale_x_pivot_center.png");
}

void test_image_scale_x_pivot_top_left(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 10 + (i % 4) * 190, 150 + (i / 4) * 150);
        lv_image_set_scale_x(img, 64 + i * 64);
        lv_image_set_pivot(img, 0, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_scale_x_pivot_top_left.png");
}

void test_image_scale_y_pivot_center(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 40 + (i % 4) * 200, 150 + (i / 4) * 150);
        lv_image_set_scale_y(img, 64 + i * 64);
        /*The default pivot should be the center*/
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_scale_y_pivot_center.png");
}

void test_image_scale_y_pivot_top_left(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 10 + (i % 4) * 190, 150 + (i / 4) * 150);
        lv_image_set_scale_y(img, 64 + i * 64);
        lv_image_set_pivot(img, 0, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_scale_y_pivot_top_left.png");
}

void test_image_rotate_and_scale_pivot_center(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 40 + (i % 4) * 200, 150 + (i / 4) * 150);
        lv_image_set_scale_x(img, 64 + i * 64);
        lv_image_set_scale_y(img, 32 + i * 96);
        lv_image_set_rotation(img, 200 + i * 333);
        /*The default pivot should be the center*/
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_rotate_and_scale_pivot_center.png");
}

void test_image_rotate_and_scale_pivot_top_left(void)
{
    lv_obj_t * img;
    uint32_t i;
    for(i = 0; i < 8; i++) {
        img = img_create();
        lv_obj_set_pos(img, 120 + (i % 4) * 180, 120 + (i / 4) * 220);
        lv_image_set_scale_x(img, 64 + i * 64);
        lv_image_set_scale_y(img, 32 + i * 96);
        lv_image_set_rotation(img, 200 + i * 333);
        lv_image_set_pivot(img, 0, 0);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_rotate_and_scale_pivot_top_left.png");
}

void test_image_normal_align(void)
{
    lv_obj_t * img;
    uint32_t i;
    lv_image_align_t aligns[] = {
        LV_IMAGE_ALIGN_TOP_LEFT, LV_IMAGE_ALIGN_TOP_MID, LV_IMAGE_ALIGN_TOP_RIGHT,
        LV_IMAGE_ALIGN_LEFT_MID, LV_IMAGE_ALIGN_CENTER, LV_IMAGE_ALIGN_RIGHT_MID,
        LV_IMAGE_ALIGN_BOTTOM_LEFT, LV_IMAGE_ALIGN_BOTTOM_MID, LV_IMAGE_ALIGN_BOTTOM_RIGHT,
    };

    for(i = 0; i < 9; i++) {
        img = img_create();
        lv_obj_set_size(img, 200, 120);
        lv_obj_set_pos(img, 30 + (i % 3) * 260, 40 + (i / 3) * 150);
        lv_image_set_inner_align(img, aligns[i]);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_normal_align.png");
}

void test_image_normal_align_offset(void)
{
    lv_obj_t * img;
    uint32_t i;
    lv_image_align_t aligns[] = {
        LV_IMAGE_ALIGN_TOP_LEFT, LV_IMAGE_ALIGN_TOP_MID, LV_IMAGE_ALIGN_TOP_RIGHT,
        LV_IMAGE_ALIGN_LEFT_MID, LV_IMAGE_ALIGN_CENTER, LV_IMAGE_ALIGN_RIGHT_MID,
        LV_IMAGE_ALIGN_BOTTOM_LEFT, LV_IMAGE_ALIGN_BOTTOM_MID, LV_IMAGE_ALIGN_BOTTOM_RIGHT,
    };

    for(i = 0; i < 9; i++) {
        img = img_create();
        lv_obj_set_size(img, 200, 120);
        lv_obj_set_pos(img, 30 + (i % 3) * 260, 40 + (i / 3) * 150);
        lv_image_set_inner_align(img, aligns[i]);
        lv_image_set_offset_x(img, 15);
        lv_image_set_offset_y(img, 20);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_normal_align_offset.png");
}

void test_image_transform_align(void)
{
    lv_obj_t * img;
    uint32_t i;
    lv_image_align_t aligns[] = {
        LV_IMAGE_ALIGN_TOP_LEFT, LV_IMAGE_ALIGN_TOP_MID, LV_IMAGE_ALIGN_TOP_RIGHT,
        LV_IMAGE_ALIGN_LEFT_MID, LV_IMAGE_ALIGN_CENTER, LV_IMAGE_ALIGN_RIGHT_MID,
        LV_IMAGE_ALIGN_BOTTOM_LEFT, LV_IMAGE_ALIGN_BOTTOM_MID, LV_IMAGE_ALIGN_BOTTOM_RIGHT,
    };

    for(i = 0; i < 9; i++) {
        img = img_create();
        lv_obj_set_size(img, 200, 120);
        lv_obj_set_pos(img, 30 + (i % 3) * 260, 40 + (i / 3) * 150);
        lv_image_set_scale_x(img, 300);
        lv_image_set_scale_y(img, 200);
        lv_image_set_rotation(img, 200);
        lv_image_set_inner_align(img, aligns[i]);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_transform_align.png");
}

void test_image_transform_align_offset(void)
{
    lv_obj_t * img;
    uint32_t i;
    lv_image_align_t aligns[] = {
        LV_IMAGE_ALIGN_TOP_LEFT, LV_IMAGE_ALIGN_TOP_MID, LV_IMAGE_ALIGN_TOP_RIGHT,
        LV_IMAGE_ALIGN_LEFT_MID, LV_IMAGE_ALIGN_CENTER, LV_IMAGE_ALIGN_RIGHT_MID,
        LV_IMAGE_ALIGN_BOTTOM_LEFT, LV_IMAGE_ALIGN_BOTTOM_MID, LV_IMAGE_ALIGN_BOTTOM_RIGHT,
    };

    for(i = 0; i < 9; i++) {
        img = img_create();
        lv_obj_set_size(img, 200, 120);
        lv_obj_set_pos(img, 30 + (i % 3) * 260, 40 + (i / 3) * 150);
        lv_image_set_inner_align(img, aligns[i]);
        lv_image_set_offset_x(img, 15);
        lv_image_set_offset_y(img, 20);
        lv_image_set_scale_x(img, 300);
        lv_image_set_scale_y(img, 200);
        lv_image_set_rotation(img, 200);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_transform_align_offset.png");
}

void test_image_stretch(void)
{
    lv_obj_t * img;
    uint32_t i;

    int32_t img_w = test_img_lvgl_logo_png.header.w;
    int32_t img_h = test_img_lvgl_logo_png.header.h;

    int32_t w_array[] = {img_w / 2, img_w, img_w * 2};
    int32_t h_array[] = {img_h / 2, img_h, img_h * 2};

    for(i = 0; i < 9; i++) {
        img = img_create();
        lv_obj_set_size(img, w_array[i / 3], h_array[i % 3]);
        lv_obj_set_pos(img, 30 + (i % 3) * 260, 40 + (i / 3) * 150);
        lv_image_set_inner_align(img, LV_IMAGE_ALIGN_STRETCH);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_stretch.png");
}

void test_image_tile(void)
{
    lv_obj_t * img;

    img = img_create();
    lv_obj_set_size(img, 350, LV_SIZE_CONTENT);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_pos(img, 20, 20);
    img = img_create();

    lv_obj_set_size(img, LV_SIZE_CONTENT, 150);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_pos(img, 420, 20);

    img = img_create();
    lv_obj_set_size(img, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_pos(img, 20, 220);
    lv_image_set_offset_x(img, -20);
    lv_image_set_offset_y(img, 20);

    img = img_create();
    lv_obj_set_size(img, 150, 150);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_pos(img, 220, 220);

    img = img_create();
    lv_obj_set_size(img, 150, 150);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
    lv_obj_set_pos(img, 420, 220);
    lv_image_set_offset_x(img, -2000);
    lv_image_set_offset_y(img, 2000);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_tile.png");
}

void test_image_ignore_transformation_settings_when_stretched(void)
{
    lv_obj_t * img = img_create();
    lv_obj_set_size(img, 200, 300);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_STRETCH);

    lv_image_set_rotation(img, 100);
    lv_image_set_pivot(img, 200, 300);
    TEST_ASSERT_EQUAL_INT(0, lv_image_get_rotation(img));

    lv_point_t pivot;
    lv_image_get_pivot(img, &pivot);
    TEST_ASSERT_EQUAL_INT(0, pivot.x);
    TEST_ASSERT_EQUAL_INT(0, pivot.y);

    int32_t scale_x_original = lv_image_get_scale_x(img);
    int32_t scale_y_original = lv_image_get_scale_y(img);
    lv_image_set_scale_x(img, 400);
    lv_image_set_scale_y(img, 500);
    TEST_ASSERT_EQUAL_INT(scale_x_original, lv_image_get_scale_x(img));
    TEST_ASSERT_EQUAL_INT(scale_y_original, lv_image_get_scale_y(img));
}

void test_image_ignore_transformation_settings_when_tiled(void)
{
    lv_obj_t * img = img_create();
    lv_obj_set_size(img, 200, 300);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);

    lv_image_set_rotation(img, 100);
    lv_image_set_pivot(img, 200, 300);
    lv_image_set_scale_x(img, 400);
    lv_image_set_scale_y(img, 500);

    lv_point_t pivot;
    lv_image_get_pivot(img, &pivot);
    TEST_ASSERT_EQUAL_INT(0, pivot.x);
    TEST_ASSERT_EQUAL_INT(0, pivot.y);
    TEST_ASSERT_EQUAL_INT(0, lv_image_get_rotation(img));
    TEST_ASSERT_EQUAL_INT(LV_SCALE_NONE, lv_image_get_scale_x(img));
    TEST_ASSERT_EQUAL_INT(LV_SCALE_NONE, lv_image_get_scale_y(img));
}

void test_image_clip_radius(void)
{
    lv_obj_t * par = lv_obj_create(lv_screen_active());
    lv_obj_set_scrollbar_mode(par, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(par, 0, 0);
    lv_obj_center(par);

    lv_obj_t * img = lv_img_create(par);
    lv_image_set_src(img, &test_arc_bg);
    lv_obj_center(img);

    lv_obj_set_style_radius(img, 10, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_clip_radius_10.png");

    lv_obj_set_style_radius(img, LV_RADIUS_CIRCLE, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_clip_radius_circle.png");

    /* parent cliped */
    lv_obj_set_pos(img, -50, -20);
    lv_image_set_scale(img, LV_SCALE_NONE * 2);
    lv_image_set_rotation(img, 450);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/image_clip_radius_circle_scaled_rotated.png");
}

void test_image_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_image_create(lv_screen_active());
    lv_property_t prop = { };

    prop.id = LV_PROPERTY_IMAGE_SRC;
    prop.ptr = &test_arc_bg;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_PTR(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_SRC).ptr, &test_arc_bg);
    TEST_ASSERT_EQUAL_PTR(lv_image_get_src(obj), &test_arc_bg);

    prop.id = LV_PROPERTY_IMAGE_OFFSET_X;
    prop.num = 10;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_OFFSET_X).num, 10);
    TEST_ASSERT_EQUAL_INT(lv_image_get_offset_x(obj), 10);

    prop.id = LV_PROPERTY_IMAGE_OFFSET_Y;
    prop.num = 20;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_OFFSET_Y).num, 20);
    TEST_ASSERT_EQUAL_INT(lv_image_get_offset_y(obj), 20);

    prop.id = LV_PROPERTY_IMAGE_ROTATION;
    prop.num = 30;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_ROTATION).num, 30);
    TEST_ASSERT_EQUAL_INT(lv_image_get_rotation(obj), 30);

    prop.id = LV_PROPERTY_IMAGE_PIVOT;
    prop.point.x = 40;
    prop.point.y = 50;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_PIVOT).point.x, 40);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_PIVOT).point.y, 50);
    lv_point_t pivot;
    lv_image_get_pivot(obj, &pivot);
    TEST_ASSERT_EQUAL_INT(pivot.x, 40);
    TEST_ASSERT_EQUAL_INT(pivot.y, 50);

    prop.id = LV_PROPERTY_IMAGE_SCALE;
    prop.num = 60;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_SCALE).num, 60);
    TEST_ASSERT_EQUAL_INT(lv_image_get_scale(obj), 60);

    prop.id = LV_PROPERTY_IMAGE_SCALE_X;
    prop.num = 70;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_SCALE_X).num, 70);
    TEST_ASSERT_EQUAL_INT(lv_image_get_scale_x(obj), 70);

    prop.id = LV_PROPERTY_IMAGE_SCALE_Y;
    prop.num = 80;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_SCALE_Y).num, 80);
    TEST_ASSERT_EQUAL_INT(lv_image_get_scale_y(obj), 80);

    prop.id = LV_PROPERTY_IMAGE_BLEND_MODE;
    prop.num = LV_BLEND_MODE_ADDITIVE;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_BLEND_MODE).num, LV_BLEND_MODE_ADDITIVE);
    TEST_ASSERT_EQUAL_INT(lv_image_get_blend_mode(obj), LV_BLEND_MODE_ADDITIVE);

    prop.id = LV_PROPERTY_IMAGE_ANTIALIAS;
    prop.num = 0;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_ANTIALIAS).num, 0);
    TEST_ASSERT_EQUAL_INT(lv_image_get_antialias(obj), 0);

    prop.id = LV_PROPERTY_IMAGE_INNER_ALIGN;
    prop.num = LV_IMAGE_ALIGN_TOP_MID;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(lv_obj_get_property(obj, LV_PROPERTY_IMAGE_INNER_ALIGN).num, LV_IMAGE_ALIGN_TOP_MID);
    TEST_ASSERT_EQUAL_INT(lv_image_get_inner_align(obj), LV_IMAGE_ALIGN_TOP_MID);
#endif
}

#endif
