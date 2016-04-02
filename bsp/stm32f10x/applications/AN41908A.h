#ifndef __LENSDRV_H__
#define __LENSDRV_H__


typedef volatile union{
	unsigned char byte;
	struct {
		unsigned char b0:1;
		unsigned char b1:1;
		unsigned char b2:1;
		unsigned char b3:1;
		unsigned char b4:1;
		unsigned char b5:1;
		unsigned char b6:1;
		unsigned char b7:1;
	} flag;
} BITF_8;

typedef volatile union{
	unsigned int word;
	struct {
		unsigned int b0:1;
		unsigned int b1:1;
		unsigned int b2:1;
		unsigned int b3:1;
		unsigned int b4:1;
		unsigned int b5:1;
		unsigned int b6:1;
		unsigned int b7:1;
		unsigned int b8:1;
		unsigned int b9:1;
		unsigned int b10:1;
		unsigned int b11:1;
		unsigned int b12:1;
		unsigned int b13:1;
		unsigned int b14:1;
		unsigned int b15:1;
	} flag;
} BITF_16;

void LensDrvFocusZoomInit(void);
void LensDrvIrisInit(void);

extern void LensDrvInit(void);
extern uint LensDrvSPIReadUint( uchar addr );
extern void LensDrvSPIWriteUint( uchar addr, uint wt_data );
extern void LensdrvDelay_us(unsigned int x);
extern void LensDrvIrisMove(uint IrsTGT);
extern void LensDrvFocusMove( u8 dir, uint FcStep);
extern void LenDrvZoomMove( u8 dir, uint ZoomStep);
void an41908a_spi_pin_init(void);
void an41908_led_mode_set(u8 mode);
void an41908_check_work_state(void);
void an41908_brake_set(u8 mode);

#endif	/*__LENSDRV_H__*/

