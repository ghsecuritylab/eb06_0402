

extern void jiguang_io_init(void);
extern void jiguang_zoom_set(u8 mode,u8 val);
extern 	void jiguang_current_set(u8 mode);
extern 	void jiguang_on_off_set(enum ON_OFF_TYPE mode);
extern 	void jiguang_motor_reset(void);
extern void jiguang_comm_init(void);



extern	s16 jiguang_zoom_steps;
extern void jiguang_zoom_handle(u8 mode);
extern u8 jiguang_zoom_state;
  
