#pragma once

void hexdump(const uint8_t *buf, size_t len, size_t width);
uint32_t hash_djb2(const uint8_t *data, size_t len);
void hashdump(const uint8_t *data, size_t len, size_t block_size, size_t width);
