
typedef	unsigned long		EEPROM_ADDR_TYPE;
typedef	unsigned int	EEPROM_PAGE_BYTES_TYPE;
#define	EEPROM_PAGE_SIZE	128//256	//at24c1024页大小
#define	EEPROM_PAGE_NUM	(512*2)//
#define	EEPROM_TOAL_SIZE	131071 //based on 0,即 0-131071 共131072 BYTES
#define MAX_PREPOINT_NUM           PREPOINT_TOTAL_256	 //	the maximun prepoints is 255 : 0-255 = total:256
#define MAX_TOUR_NUM              6      //最大自学习条数
#define	MAX_VECTORSCAN_NUM		8
#define	PREPOINT_NUM_TOTALS		(PREPOINT_TOTAL_256)

#define MAX_VECTOR_NUM            8     //actual 9, but the 7th is track


extern void eeprom_byte_write(EEPROM_ADDR_TYPE address, uchar data);
uchar eeprom_byte_read(EEPROM_ADDR_TYPE address);
void I2C_AT24C512_Init(void);


