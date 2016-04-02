

#if 0
#define     OSD_IDLE    0
#define     OSD_LEFT    1
#define     OSD_RIGHT    2
#define     OSD_UP      4
#define     OSD_DOWN    8
#define     OSD_ENTER    0x10
#define     OSD_ENTER_LONG    0x82
#endif

#ifndef	CAM_CTRL_ENUM_M
enum osd_key_type
{
OSD_IDLE,
OSD_LEFT,
OSD_RIGHT,
OSD_UP = 4,
OSD_DOWN = 8,
OSD_ENTER=0x10,
OSD_ENTER_LONG = 0x90,

};


enum picture_reverse_type
{
PIC_NORMAL,
PIC_MIRROR,
PIC_VFLIP,
PIC_ALL_FLIP

};

enum framerate_type
{
    CAM_FRAMERATE_720P25,
    CAM_FRAMERATE_720P30,
    CAM_FRAMERATE_720P50,
    CAM_FRAMERATE_720P60,
	CAM_FRAMERATE_1080P25,
	CAM_FRAMERATE_1080P30,
	CAM_FRAMERATE_1080I50,
	CAM_FRAMERATE_1080I60,

	CAM_CVBS_PAL,
	CAM_CVBS_NTSC,
};

#define	CAM_CTRL_ENUM_M
#endif

extern void camera_osd_switch(u8 mode);
extern void camera_osd_key_emulate(enum osd_key_type type);
extern u8 cam_request_response(void);
extern u8 cam_get_ID(u8 val);
extern	void cam_init(void);
extern void cam_zoom_data_set(u16 val); 

extern void cam_picture_reverse_set(enum picture_reverse_type val);
extern void cam_freeze_set(enum ON_OFF_TYPE val);
extern void cam_frog_set(u8 val);

extern u8 cam_get_parameters_oneByte(u8 blockNO,u16 offset,u8*get_para);
extern void cam_cross_line_mode_set(enum ON_OFF_TYPE mode);
extern void cam_atw_level_set(u8 val);
extern void cam_brightness_set(u8 val,u8 mode);
extern void cam_zoom_mode_set(enum ON_OFF_TYPE mode);
extern u8 cam_brightness_mode_get(void);

extern void cam_framerate_set(enum framerate_type val);
extern u8 cam_brightness_get(u8 mode,u8 *reval);
extern void cam_color_mode_set(enum ON_OFF_TYPE mode);
extern u8 cam_atw_level_get(u8 val);
extern void cam_brightness_mode_set(u8 mode);
extern void cam_zoom_disp_set(enum ON_OFF_TYPE mode);
extern u8 cam_zoom_data_get(u16 *val);
extern void cam_zoom_menu_set(enum ON_OFF_TYPE mode);

void get_cam_para_all(void);
void set_cam_para_all(void);
u8 cam_cross_line_mode_get(u8 *buf);
u8 cam_freeze_get(u8 *buf);
void cam_comm_menu_set(u8 val);// 关闭或者开启 通讯设置在菜单中的显示
void cam_cross_line_mode_set(enum ON_OFF_TYPE mode);
void cam_color_bar_mode_set(enum ON_OFF_TYPE mode);
void cam_nega_posi_mode_set(enum ON_OFF_TYPE mode);
u8 cam_agc_level_get(u16 *val);

int cam_uart1_init(void);
u8 cam_iris_value_data_get(u16* val);
u8 cam_shutter_value_get(u16 *val);

#define CAMERA_BRIGHTNESS_VAL_MAX       8

