/*
Copyright (C) 2016  Michael Wallace, Mario Garcia.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <CommProto/headerpacket.h>

#include <cstring>

uint32_t header_t::serialize(uint8_t* buffer, uint32_t offset) {
  memcpy(buffer + (offset++), &destID, 1);
  memcpy(buffer + (offset++), &sourceID, 1);
  memcpy(buffer + offset, &messageLength, 2);
  offset += 2;
  memcpy(buffer + offset, &messageID, 2);
  offset += 2;
  for (int x = 0; x < KEY_LENGTH; x++){
    memcpy(buffer + (offset++), &IV[x], 1);
  }

  return offset;
}


uint32_t header_t::deserialize(uint8_t* buffer, uint32_t offset) {
  memcpy(&destID, buffer + (offset++), 1);
  memcpy(&sourceID, buffer + (offset++), 1);
  memcpy(&messageLength, buffer + offset, 2);
  offset += 2;
  memcpy(&messageID, buffer + offset, 2);
  offset += 2;
  
  for (int x = 0; x < KEY_LENGTH; x++) {
      memcpy(&IV[x], buffer + (offset++), 1);
  }

  return offset;  
}