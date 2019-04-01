#ifndef COMMAND_H
#define COMMAND_H

uint16_t wait_reply(int fd, uint32_t ev);
uint16_t send_command(int fd, uint32_t cmd);

static inline uint32_t unpack(uint8_t* data)
{
	return ((data[0] & 0x7f) << 21) | (data[1] << 14) | (data[2] << 7) | data[3];
}

ssize_t packet_out(int fd, uint32_t x);

#endif /* COMMAND_H */
