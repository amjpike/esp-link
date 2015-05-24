/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


#include <esp8266.h>
#include "cgi.h"
#include "io.h"


//cause I can't be bothered to write an ioGetLed()
static char currLedState=0;

//Cgi that turns the LED on or off according to the 'led' param in the POST data
int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
	int len;
	char buff[1024];

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len=httpdFindArg(connData->post->buff, "led", buff, sizeof(buff));
	if (len!=0) {
		currLedState=atoi(buff);
		ioLed(currLedState);
	}

	httpdRedirect(connData, "led.tpl");
	return HTTPD_CGI_DONE;
}



//Template code for the led page.
int ICACHE_FLASH_ATTR tplLed(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "ledstate")==0) {
		if (currLedState) {
			os_strcpy(buff, "on");
		} else {
			os_strcpy(buff, "off");
		}
	} else if (os_strcmp(token, "topnav")==0) {
		printNav(buff);
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

static long hitCounter=0;

//Template code for the counter on the index page.
int ICACHE_FLASH_ATTR tplCounter(HttpdConnData *connData, char *token, void **arg) {
	char buff[256];
	if (token==NULL) return HTTPD_CGI_DONE;

	if (os_strcmp(token, "topnav")==0) {
		printNav(buff);
	} else if (os_strcmp(token, "counter")==0) {
		hitCounter++;
		os_sprintf(buff, "%ld", hitCounter);
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

static char *navLinks[][2] = {
	{ "Home", "/index.tpl" }, { "Wifi", "/wifi/wifi.tpl" }, { "Serial", "/index.tpl" },
	{ "Esp log", "/console.tpl" }, { "Help", "/help.tpl" },
	{ 0, 0 },
};

// Print the navigation links into the buffer and return the length of what got added
int ICACHE_FLASH_ATTR printNav(char *buff) {
	int len = 0;
	for (uint8_t i=0; navLinks[i][0] != NULL; i++) {
		if (i > 0) buff[len++] = '|';
		//os_printf("nav %d: %s -> %s\n", i, navLinks[i][0], navLinks[i][1]);
		len += os_sprintf(buff+len, " <a href=\"%s\">%s</a> ", navLinks[i][1], navLinks[i][0]);
	}
	//os_printf("nav: %s\n", buff);
	return len;
}
