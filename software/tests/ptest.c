#include <stdio.h>
#include <stdint.h>

static inline uint32_t unpack(uint8_t* out)
{
	return ((out[0] & 0x7f) << 21) | (out[1] << 14) | (out[2] << 7) | out[3];
}

void pack(uint8_t* data, uint32_t x)
{
	data[0] = (((x >> 21) & 0x7f) | 0x80);
	data[1] = ( (x >> 14) & 0x7f);
	data[2] = ( (x >>  7) & 0x7f);
	data[3] = (  x        & 0x7f);

}

int main(int argc, char** argv)
{
	for (int i = 0; i < 100000000; i++)
	{
		uint8_t d[4];

		pack(d, i);
		int y = unpack(d);

		if (y != i)
		{
			printf("%d != %d\n", i, y);
			return 1;
		}
	}

	return 0;
}
