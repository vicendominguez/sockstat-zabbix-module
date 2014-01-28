/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA  02110-1301, USA.
**/
 
#include "sysinc.h"
#include "module.h"
 
/* the variable keeps timeout setting for item processing */
static int    item_timeout = 0;
 
int    zbx_module_sockstat_info(AGENT_REQUEST *request, AGENT_RESULT *result);

static ZBX_METRIC keys[] =
/* KEY               FLAG           FUNCTION                TEST PARAMETERS */
{
    {"sockstat.info",   CF_HAVEPARAMS, zbx_module_sockstat_info,  "total"},
    {NULL}
};

/****************************
 * sockstat file parser     *
 ****************************/

int * get_values_sockstat(FILE * fd) {

  static int    buffer[5];
  char   line[128];

  if (!fd)
	return NULL;

  while (NULL != fgets(line, sizeof(line), fd)) {

        if (0 == strncmp(line,"sockets",7)) {
/*
                /proc/net/sockstat format: kernel 2.6.32-431.3.1.el6.x86_64 Centos 6.5
                sockets: used 290
*/
                sscanf (line, "%*s used %d", &buffer[0]);
        }
	if (0 == strncmp(line,"TCP",3)){
/*
                /proc/net/sockstat format: kernel 2.6.32-431.3.1.el6.x86_64 Centos 6.5
                 TCP: inuse 117 orphan 3 tw 669 alloc 121 mem 132
*/
                 sscanf(line,"%*s inuse %d orphan %d tw %d alloc %d",&buffer[1],&buffer[2],&buffer[3],&buffer[4]);
        }
  }
  return buffer;
}

/******************************************************************************
*                                                                            *
* Function: zbx_module_api_version                                           *
*                                                                            *
* Purpose: returns version number of the module interface                    *
*                                                                            *
* Return value: ZBX_MODULE_API_VERSION_ONE - the only version supported by   *
*               Zabbix currently                                             *
*                                                                            *
******************************************************************************/
int    zbx_module_api_version()
{
    return ZBX_MODULE_API_VERSION_ONE;
}
 
/******************************************************************************
*                                                                            *
* Function: zbx_module_item_timeout                                          *
*                                                                            *
* Purpose: set timeout value for processing of items                         *
*                                                                            *
* Parameters: timeout - timeout in seconds, 0 - no timeout set               *
*                                                                            *
******************************************************************************/
void    zbx_module_item_timeout(int timeout)
{
    item_timeout = timeout;
}
 
/******************************************************************************
*                                                                            *
* Function: zbx_module_item_list                                             *
*                                                                            *
* Purpose: returns list of item keys supported by the module                 *
*                                                                            *
* Return value: list of item keys                                            *
*                                                                            *
******************************************************************************/
ZBX_METRIC    *zbx_module_item_list()
{
    return keys;
}


/******************************************************************************
*                                                                            *
* Function: zbx_module_sockstat_info                                         *
*                                                                            *
* Purpose: returns values of sockstat proc file                              *
*                                                                            *
* Return value: list of values                                               *
*                                                                            *
******************************************************************************/
 
int    zbx_module_sockstat_info(AGENT_REQUEST *request, AGENT_RESULT *result)
{
    char    *param;

    char    P_PROC_SOCKSTAT[] = "/proc/net/sockstat";
    static  FILE *f_sockstat;
    int    *p_sockstat;

    int     j;	
    int     i_out;

    const   int  N_COMMANDS = 5;
    char    *s_commands[5] = {"total","tcp","orphan","timewait","allocated"}; 

    /* real start */

    if (1 != request->nparam)
    {
        /* set optional error message */
        SET_MSG_RESULT(result, strdup("Invalid number of parameters"));
        return SYSINFO_RET_FAIL;
    }
 
    param = get_rparam(request, 0);
    
    f_sockstat = fopen(P_PROC_SOCKSTAT, "r");
      if (0x0 == f_sockstat) {
	SET_MSG_RESULT(result, strdup("sockstat not found in /proc"));
	return SYSINFO_RET_FAIL;
    }

    p_sockstat = get_values_sockstat(f_sockstat);

    i_out = p_sockstat[0];
    for (j=0;j<=N_COMMANDS;j++){
	if (!strcmp(s_commands[j],param)) { 
		i_out = p_sockstat[j]; 
		break;
	}
    }

    fclose (f_sockstat);
    SET_UI64_RESULT(result, i_out);
    return SYSINFO_RET_OK;
}

 
/******************************************************************************
*                                                                            *
* Function: zbx_module_init                                                  *
*                                                                            *
* Purpose: the function is called on agent startup                           *
*          It should be used to call any initialization routines             *
*                                                                            *
* Return value: ZBX_MODULE_OK - success                                      *
*               ZBX_MODULE_FAIL - module initialization failed               *
*                                                                            *
* Comment: the module won't be loaded in case of ZBX_MODULE_FAIL             *
*                                                                            *
******************************************************************************/
int    zbx_module_init()
{
    srand(time(NULL));
    return ZBX_MODULE_OK;
}
 
/******************************************************************************
*                                                                            *
* Function: zbx_module_uninit                                                *
*                                                                            *
* Purpose: the function is called on agent shutdown                          *
*          It should be used to cleanup used resources if there are any      *
*                                                                            *
* Return value: ZBX_MODULE_OK - success                                      *
*               ZBX_MODULE_FAIL - function failed                            *
*                                                                            *
******************************************************************************/
int    zbx_module_uninit()
{
    return ZBX_MODULE_OK;
}
