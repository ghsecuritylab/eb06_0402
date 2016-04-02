#ifndef	__OSD_MENU_H_
#define	__OSD_MENU_H_


#define	OSD_VAL_START_ADDR_X		40//24
#define	OSD_VAL_START_ADDR_Y		0

extern u8 cam_para_mode;
extern u8 iris_mode;
extern u8 cam_filter_mode;

extern void osd_val_disp(u8 *data,u8 cnt);
extern void osd_clear_x_y(u8 x_start,u8 x_end,u8 y,u8 sizef);
extern void osd_opt_message_disp(u8 type,u16 wait_s);
extern void osd_iris_mode_disp(u8 type);
extern void osd_line2_disp(u8 x);
extern void osd_line2_val_disp_clear(void);
extern void osd_line3_disp(u8 select);
extern void osd_line_little_4_disp_item_clear(u8 select);
extern void osd_line_little_4_disp(u8 chn);
extern void OLED_Clear_line(u8 x,u8 y,u8 sizef);
extern void osd_para_mode_disp(u8 mode);
extern void osd_iris_mode_disp(u8 mode);
extern void osd_filter_mode_disp(u8 mode);
extern void osd_line1_val_disp_clear(void);
extern void osd_line_2_disp_item_clear(u8 select);
extern void osd_set_para_disp_line_1(u8 item);
extern void osd_para_mode_disp_xy(u8 mode,u8 x,u8 y);
extern void osd_iris_mode_disp_xy(u8 mode,u8 x,u8 y);
extern void osd_filter_mode_disp_xy(u8 mode,u8 x,u8 y);

extern void osd_sd_hd_mode_disp_clear(u8 data,u8 x,u8 y);
extern void osd_sd_hd_mode_disp(u8 data,u8 x,u8 y);
extern void filter_mode_disp_clear(u8 x,u8 y,u8 mode);
extern void osd_battery_mode_disp(u8 data,u8 x,u8 y);
extern void osd_line_1to4_all_disp(void);

#define	LINE3_SPACE		8
#define	LINE3_FONT_WIDTH	8

#define	OSD_LINE3_Y_POS		4
#define	OSD_LINE2_Y_POS		2


#define	OSD_PARA_MODE_X_START		0
#define	OSD_PARA_MODE_X_END		40

#define	OSD_PARA_MODE_Y		OSD_LINE2_Y_POS


#define	OSD_IRIS_MODE_X_START		(OSD_PARA_MODE_X_END+6)
#define	OSD_IRIS_MODE_X_END		(OSD_IRIS_MODE_X_START+8*4)

#define	OSD_IRIS_MODE_Y			OSD_LINE2_Y_POS

#define	OSD_FILTER_MODE_X_START		(OSD_IRIS_MODE_X_END+6)
#define	OSD_FILTER_MODE_Y		OSD_LINE2_Y_POS


#define	OSD_LINE2_Y_16_POS		2
#define	OSD_LINE2_X_16_POS		0


#endif
