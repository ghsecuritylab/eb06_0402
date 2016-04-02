#ifndef _ROTOCOL_H_
#define _ROTOCOL_H_

extern void System_data_init(void);
extern void ReturnPanTiltPos(u8 InquiryStepType);
extern void Keyboard_camera_porotocol_2(void);
extern uchar command_analysis(void);


#define CAMERA_LINK_CMD_HEAD    0xb1
#define CAMERA_LINK_CMD_TAIL    0xee

extern uchar keyboard_data_buffer[20];
extern u8   Pelco_fir_data;                       //当前协议的命令串的第一个字节
extern u8   Rec_byte_count;
extern u8   cmd_type;
extern u8   cmd_flag;   
extern u8   keyboard_data_history[2];
extern u8   x_his_speed;
extern u8   y_his_speed;
extern u8   Rec_byte_count_buff;
extern u8   Rec_spe_byte_count_buff;
extern u8   ultrak_erna_rec_tour_data_f;
extern u8   pan_direction;
extern u8   tilt_direction;
extern u8   pan_tilt_flag;


#endif  /* _ROTOCOL_H_ */

