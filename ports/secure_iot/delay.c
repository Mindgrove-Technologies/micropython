
#include<stdint.h>
#include"delay.h"
/***************************************************************************************************
                         void DELAY_us(uint16_t us_count)
****************************************************************************************************
 * I/P Arguments: uint16_t.
 * Return value	: none

 * description :
         This function is used generate delay in us.
         It generates a delay of approximate 10us for each count,
         if 5000 is passed as the argument then it generates a delay of 50ms.		  

***************************************************************************************************/
void DELAY_us(uint16_t us_count)
 {	
    while(us_count!=0)
      {
		 us_count--;
	   }
  }



/***************************************************************************************************
                         void DELAY_ms(uint16_t ms_count)
****************************************************************************************************
 * I/P Arguments: uint16_t.
 * Return value	: none

 * description:
     This function is used generate delay in ms.
     It generates a delay of 1ms for each count,
     if 1000 is passed as the argument then it generates delay of 1000ms(1sec)
***************************************************************************************************/
void DELAY_ms(uint16_t ms_count)
 {
        while(ms_count!=0)
		 {
	        DELAY_us(C_CountForOneMsDelay_U16);	 //DELAY_us is called to generate 1ms delay
			 ms_count--;
			}

   }






/***************************************************************************************************
                        void DELAY_sec(uint16_t sec_count)
****************************************************************************************************
  * I/P Arguments: uint16_t.
  * Return value	: none

  * description:
      This function is used generate delay in sec .
      It generates a approximate delay of 1sec for each count,
      if 10 is passed as the argument then it generates delay of 10sec
***************************************************************************************************/
void DELAY_sec(uint16_t sec_count)
 {


	 while(sec_count!=0)
	  {
	     DELAY_ms(1000);	//DELAY_ms is called to generate 1sec delay
		 sec_count--;
		}
  }