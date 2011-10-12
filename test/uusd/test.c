#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <iconv.h>
#include <malloc.h>


char *unicodebig_to_utf8(const char *src_str);

int main(int argc, char **argv) {
	const char *src_str = "4E2D56FD79FB52A8624B673A94F6884C5361000A900962E963A5542C621656DE590D540E952E5165529F80FD4EE37801000A0031002E0020621176845E106237000A0032002E00205145503C7F348D39000A0033002E00205F6979684E2D5FC3000A0034002E00208BDD8D39554657CE000A0035002E00204F7F75285E2E52A9";
	//const char *src_str = "4E2D56FD79FB52A8624B673A94F6884C5361000A900962E963A5542C621656DE590D540E952E5165529F80FD4EE37801000A0031002E0020621176845E106237000A0032002E00205145503C7F348D39000A0033002E00205F6979684E2D5FC3000A003";
	printf("ret = %s\n", unicodebig_to_utf8(src_str));
	return 0;
}


char *unicodebig_to_utf8(const char *src_str) {

	//char dst_str[2048];
	//char utf8_str[2048];
	//memset(&dst_str, sizeof(dst_str), 0);
	//memset(&utf8_str, sizeof(utf8_str), 0);
	char *dst_str = malloc(2048);
	char *utf8_str = malloc(2048);
	printf("%0x, %0x\n", dst_str, utf8_str);
	memset(&dst_str, 2048, 0);
	memset(&utf8_str, 2048, 0);
	printf("%0x, %0x\n", dst_str, utf8_str);

	int i=0;
	int j=0;
	int len = strlen(src_str);
	for (; i<len; i=i+2) {
	  unsigned char tmp = 0;
	  if (src_str[i] <= '9') {
	    tmp |= (src_str[i] - 48) << 4;
	  } else {
	    tmp |= (src_str[i] - 65 + 10) << 4;
	  }

	  if (src_str[i+1] <= '9') {
	    tmp |= (src_str[i+1] - 48);
	  } else {
	    tmp |= (src_str[i+1] - 65 + 10);
	  }

	  dst_str[j++] = tmp;
	}

	
   iconv_t cd=iconv_open("UTF-8", "UNICODEBIG");
   char *ins = dst_str;//待转文本的指针，用于iconv参数兼容。
   char *outs = utf8_str;//空间的指针
 
   size_t il,ol;

   il = j;
   ol = 2048;
 
   int res = iconv(cd, &ins, &il, &outs, &ol);

   if (res == -1) {
    perror("iconv");
    return NULL;
   } 

   iconv_close(cd);

   printf("utf8_str = %s\n", utf8_str);

   char *ret_str = (char*)malloc(2048 - ol + 1);
   strcpy(ret_str, utf8_str);
   return ret_str;

}
