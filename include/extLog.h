#ifndef	__EXT_LOG_H__
#define	__EXT_LOG_H__

/*
* debug header, ready to be ported and used to different project
*/

/** Debug level: ALL messages*/
#define	EXT_DBG_LEVEL_ALL				0x00
#define	EXT_DBG_LEVEL_INFO			0x01
#define	EXT_DBG_LEVEL_WARN			0x02
#define	EXT_DBG_LEVEL_ERROR			0x03
#define	EXT_DBG_LEVEL_FATAL			0x04


#define	EXT_DBG_ON					0x80U
#define	EXT_DBG_OFF					0x00U


#define	EXT_DBG_TRACE					0x40U
#define	EXT_DBG_STATE					0x20U
#define	EXT_DBG_FRESH					0x10U
#define	EXT_DBG_HALT					0x08U

#define	EXT_DBG_MASK_LEVEL			0x04
#define	EXT_DBG_LEVEL_OFF				EXT_DBG_LEVEL_ALL
#define	EXT_DBG_TYPES_ON				EXT_DBG_ON
#define	EXT_DBG_MIN_LEVEL				EXT_DBG_LEVEL_ALL


#define	EXT_NEW_LINE							"\r\n"


/*
* ESC (27, 0x1b) charactor is '\e' or '\x1b'
*/

#define ANSI_COLOR_RED				"\x1b[31m"	/* ESC[31m : red */
#define ANSI_COLOR_GREEN			"\x1b[32m"
#define ANSI_COLOR_YELLOW			"\x1b[33m"
#define ANSI_COLOR_BLUE			"\x1b[34m"
#define ANSI_COLOR_MAGENTA		"\x1b[35m"
#define ANSI_COLOR_CYAN			"\x1b[36m"
#define ANSI_COLOR_RESET			"\x1b[0m"	/* for all colors, other than red, this must be used. April,15,2018. JL*/


#define	ERROR_TEXT_BEGIN			"\t\e[31m ERR:"
#define	ERROR_TEXT_END			"\e[0m"


#define	WARN_TEXT_BEGIN			""ANSI_COLOR_MAGENTA"WARN:"

#define	INFO_TEXT_BEGIN			""ANSI_COLOR_BLUE"INFO:"


#ifndef __EXT_RELEASE__
	#define	EXT_PRINT(x)						{printf x ;}
	
//	#define	EXT_DEBUG(fmt, args...)	{printf("[%s-%u] DEBUG: " fmt EXT_NEW_LINE, __FILE__, __LINE__, ## args);}
	#define	EXT_DEBUG(debug, message...)		do { \
                               if ( \
                                   ((debug) & EXT_DBG_ON) && \
                                   ((debug) & EXT_DBG_TYPES_ON) && \
                                   ((int16_t)((debug) & EXT_DBG_MASK_LEVEL) >= EXT_DBG_MIN_LEVEL)) { \
                                 _TRACE_OUT(message);printf(EXT_NEW_LINE); \
                                 if ((debug) & EXT_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)

                             
	#define	EXT_INFO(message...)		{printf(ANSI_COLOR_CYAN "%s:[%s-%u]:", sysTaskName(), __FILE__, __LINE__);EXT_PRINT((message));printf((ANSI_COLOR_RESET EXT_NEW_LINE));}
	
	#define	EXT_ERROR(message...)		{printf(ERROR_TEXT_BEGIN "%s: ERROR:[%s-%u]:", sysTaskName(), __FILE__, __LINE__);EXT_PRINT((message)); printf((ERROR_TEXT_END  EXT_NEW_LINE));}

//	#define	EXT_ASSERT(x)				{printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); while(1);}
	#define	EXT_ASSERT(x, msg...)			{if((x)==0) {printf(ERROR_TEXT_BEGIN"%s: ASSERT: [%s-%u]:",  sysTaskName(), __FILE__, __LINE__ );printf (msg) ;printf((ERROR_TEXT_END EXT_NEW_LINE)); while(0){};}}
	#define	EXT_ABORT(fmt, args... )		printf("%s: ABORT in [" __FILE__ "-%u]:" fmt EXT_NEW_LINE, sysTaskName(), __LINE__, ##args );while(1){}
#else
	#define	EXT_PRINT(x)						{;}

	#define	EXT_DEBUG(debug, message...)		{}

	#define	EXT_INFO(message...)				{printf (message) ;printf(EXT_NEW_LINE);}

	#define	EXT_ERROR(message...)				{printf (message) ; printf(EXT_NEW_LINE);}
	
//	#define	EXT_ASSERT(x)				{while (1);}
	#define	EXT_ASSERT(x, msg...)				{}
	#define	EXT_ABORT(fmt, args... )		{}
#endif

#define	_TRACE_OUT(message...)	\
			{EXT_PRINT(("%s: [%s-%u.%s()]: ",  sysTaskName(), __FILE__, __LINE__, __FUNCTION__) );EXT_PRINT( (message)); }

#define	TRACE()						_TRACE_OUT((EXT_NEW_LINE) )

/* check predefined marco in gcc with 'cpp -dM include/extLog.h '*/
#if linux
#include "pthread.h"
#include <string.h>
 #include <errno.h>
inline static char	*sysTaskName(void)
{
	static char threadName[16] = "Unknown";
	pthread_t             self = pthread_self();

	if(pthread_getname_np(self, threadName, sizeof(threadName)) != 0)
	{
		EXT_ERROR("Fail in get thread name: %s", strerror(errno) );
		return "Unknown";
	}

	return threadName;
}
#else
#define	sysTaskName()		""
#endif

#endif

