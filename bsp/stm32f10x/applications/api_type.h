#ifndef _API_TYPE_H_
#define _API_TYPE_H_


#define uchar unsigned char

#define uint32 unsigned int

/**************************************CONFIGURE STANDARD DATA TYPES*****************************************************/

typedef  unsigned  char       CHAR;                         /*  8-bit character                                     */
typedef  unsigned  char       BOOLEAN;                      /*  8-bit boolean or logical                            */
typedef  unsigned  char       INT08U;                       /*  8-bit unsigned integer                              */
typedef    signed  char       INT08S;                       /*  8-bit   signed integer                              */
typedef  unsigned  short      INT16U;                       /* 16-bit unsigned integer                              */
typedef    signed  short      INT16S;                       /* 16-bit   signed integer                              */
typedef  unsigned  int        INT32U;                       /* 32-bit unsigned integer                              */
typedef    signed  int        INT32S;                       /* 32-bit   signed integer                              */
typedef  unsigned  long long  INT64U;                       /* 64-bit unsigned integer                              */
typedef    signed  long long  INT64S;                       /* 64-bit   signed integer                              */

typedef            float      FP32;                         /* 32-bit floating point                                */
typedef            double     FP64;                         /* 64-bit floating point                                */


typedef		unsigned short	OSD_CHAR_BUF_TYPE;		//
typedef 	unsigned short	MENU_ITEM_NUM_TYPE;


#ifndef	 uchar
#define uchar unsigned char
#endif

#ifndef	 uint
#define uint  unsigned short 
#endif

#define	MCU_LITTLE_ENDIAN	1

#if MCU_LITTLE_ENDIAN

  typedef	union __Union_time {
    struct
       {
       uchar second;
       uchar minute; 
       uchar hour;
	   uchar week;
	   uchar day;
	   uchar month;
       uchar year;
       }data;
	uchar array[7];
   }Union_time; 
#else
typedef	union __Union_time {
    struct
       {
        uchar year;
		uchar month;
		uchar day;
		uchar week;
		uchar hour;
		uchar minute;
		uchar second;
       }data;
	uchar array[7];
   }Union_time;

#endif

#endif

