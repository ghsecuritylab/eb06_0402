
#ifndef	IRIS_CTL_MODE_TYPE_M
enum IRIS_CTL_MODE_TYPE
{
IRIS_CAM_IRIS,
IRIS_JIGUANG_ZOOM,//激光出光角度
IRIS_JIGUANG_CURRENT,//激光电流
IRIS_CAM_OSD_ENTER
};
#define	IRIS_CTL_MODE_TYPE_M
#endif

extern enum IRIS_CTL_MODE_TYPE iris_ctl_mode;


extern void dome_func_control(uchar action,uchar prePoint);
extern void cam_filt_cutter_set(u8 mode);
extern u8 RS485_SendBytes(u16 bytes_size,u8 *send_buff);

void extern_io_output(u8 mode);
void extern_io2_output(u8 mode);
void motor_lens_voltage_set(u8 mode);
void save_system_para(u8 mode);
void load_system_para(void);
void extern_io3_output(u8 mode);
void key_long_osd_emu(void);
void set_camera_uart_baudrate(u8 cbaudrate );




