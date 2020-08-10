#ifndef CONFIG_H
#define CONFIG_H

#define OTA_ENABLED
#define DBG_TCP_ENABLED
#define DBG_SERIAL_ENABLED


/* PCB DESIGN   */
//#define PCB_DESIGN_1
//#define PCB_DESIGN_2 
#define PCB_DESIGN_3 


/* SPA MODEL    */
#define SSP_H       // SSP-H-20-1/SSP-H-20-1C/SSP-H20-2
//#define SJB_HS    // SJB-HS-20-1G/SJB-HS-30-1G/SJB-HS-22-1G/SJB-HS-33-1G

/* PROTOCOL    */
#define HOMEKIT


#define DBG_TCP_PORT        8888
#define SERIAL_DEBUG_SPEED  115200  

#ifdef PCB_DESIGN_1

	#define DATA_PIN    12    /*!< white wire   */
	#define CLCK_PIN    14    /*!< yellow wire  */
	#define HOLD_PIN    13    /*!< black wire   */

    /* connected to CD4051 */
    #define S0          4
    #define S1          5
    #define S2          15  

    #define E_          16    /* enable at 0 CD4051 output */

#endif 

#ifdef PCB_DESIGN_2
    /*
        !! PCB V2 - DUAL CD4051 !!
    */

	#define DATA_PIN    12    /*!< white wire   */
	#define CLCK_PIN    14    /*!< yellow wire  */
	#define HOLD_PIN    13    /*!< black wire   */

    /* connected to U4 & U5 */
    #define S0  5
    #define S1  4
    #define S2  15

    #define U4_E_  16   /* enable at 0 U4 output */
    #define U5_E_   0

#endif 

#ifdef PCB_DESIGN_3
    /*
        !! PCB V3 - D1 mini, no PCB
    */
	#define DATA_PIN    D5    /*!< white wire   */
	#define CLCK_PIN    D7    /*!< yellow wire  */
	#define HOLD_PIN    D6    /*!< black wire   */
	#define OUT_PIN     D0    /*!< 1kOhm to white wire to simulate button press */
	#define BEEP_PIN    D2    /*!< connected to buzzer circuitry */

#endif 

/* temperature sensor */
#define ADC_PIN     A0
#define NTC_R25     9000.0
#define NTC_T25     25.0
#define NTC_BETA    3950.0

#define WIFI_STA_HOSTNAME   "diyscip"
#define WIFI_AP_NAME        "DYISCIP_Setup"



/******   DO NOT MODIFY FROM HERE    *******/
#if !((defined(PCB_DESIGN_1) && !defined(PCB_DESIGN_2) && !defined(PCB_DESIGN_3)) \
  || (!defined(PCB_DESIGN_1) &&  defined(PCB_DESIGN_2) && !defined(PCB_DESIGN_3)) \
  || (!defined(PCB_DESIGN_1) && !defined(PCB_DESIGN_2) &&  defined(PCB_DESIGN_3)) )
#error "YOU MUST DEFINE EXACTY ONE PCB DESIGN"
#endif


#if (defined(SSP_H) && defined(SJB_HS)) || (!defined(SSP_H) && !defined(SJB_HS))
#error "YOU MUST DEFINE ONE AND ONE SPA MODEL"
#endif


#endif // CONFIG_H
