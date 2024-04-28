#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "common.h"

// 生成uuid,通过返回值或参数将结果返回
char *generateUUID()
{
	static char uuid[0x100];
	const char *c = "89ab";   // UUID 大都是变体 1，取值是 8、9、a、b 中的一个。
	char *p = uuid;
	int n;

	for (n = 0; n < 16; ++n)
	{
		int b = rand() % 255;  // 生成16个16进制的值

		switch (n)
		{
		case 6:
			sprintf(
				p,
				"4%x",
				b % 15);   // 版本 4：UUID 使用随机性或伪随机性生成
			break;
		case 8:
			sprintf(
				p,
				"%c%x",
				c[rand() % strlen(c)],    // UUID 变体
				b % 15);
			break;
		default:
			sprintf(
				p,
				"%02x",
				b);
			break;
		}

		p += 2;

		switch (n)
		{
		case 3:
		case 5:
		case 7:
		case 9:
			*p++ = '_';
			break;
		}
	}
	*p = 0;
	return uuid;
}




uint32_t align(uint32_t value){
	return ((value + alignment_size - 1) & (~(alignment_size - 1)));
}