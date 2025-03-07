#include "pch.h"
#include "screenshots_common.h"

namespace screenshots
{

// DSA params 
u8 const p_number[crypto::xr_dsa::public_key_length] = {
    0xe0, 0xc7, 0x56, 0xaa, 0x69, 0xc8, 0x34, 0x75, 
    0xe2, 0x41, 0x5d, 0x33, 0xd2, 0x4e, 0x91, 0x09, 
    0xd9, 0x65, 0xbb, 0x4c, 0x1d, 0xb9, 0xb8, 0x07, 
    0x9b, 0x05, 0x32, 0xa3, 0x72, 0x6e, 0x76, 0x24, 
    0x04, 0xb1, 0x4f, 0x3b, 0x58, 0xfa, 0x05, 0x4c, 
    0x54, 0x95, 0x5d, 0xc3, 0x2b, 0xdd, 0x13, 0xe9, 
    0xd4, 0xe0, 0xa8, 0x8c, 0xd1, 0x9d, 0x11, 0xa1, 
    0x31, 0x1b, 0x00, 0xa2, 0x82, 0xb7, 0xc9, 0x46, 
    0x01, 0x22, 0x9d, 0xa7, 0xb0, 0xe1, 0x4a, 0x10, 
    0x39, 0xd5, 0x92, 0xd4, 0xcd, 0x02, 0x37, 0xa0, 
    0xda, 0x6a, 0xfe, 0x02, 0x82, 0x54, 0x9b, 0x5d, 
    0xcd, 0x71, 0xbf, 0x69, 0xc8, 0x77, 0x4c, 0x7e, 
    0x5e, 0x4e, 0x7a, 0xff, 0x0f, 0x12, 0x62, 0xd5, 
    0xf5, 0xca, 0x0a, 0x76, 0x26, 0x6d, 0x16, 0x0b, 
    0x4f, 0x09, 0x3b, 0x41, 0x09, 0x80, 0xff, 0x65, 
    0x15, 0xce, 0x29, 0x0f, 0x04, 0x45, 0xb0, 0xb1, 
};//p_number
u8 const q_number[crypto::xr_dsa::private_key_length] = {
    0xfb, 0xe4, 0x9a, 0x95, 0x31, 0x49, 0xcb, 0xda, 
    0xf1, 0x91, 0x36, 0x82, 0xa8, 0x80, 0xfd, 0xcb, 
    0x4e, 0x38, 0x85, 0x45, 
};//q_number
u8 const g_number[crypto::xr_dsa::public_key_length] = {
    0xdc, 0x38, 0x96, 0xb4, 0x81, 0xf2, 0x2d, 0x60, 
    0x20, 0x7e, 0x9a, 0xe5, 0x74, 0x45, 0x7f, 0xa4, 
    0x9d, 0x71, 0xb8, 0x94, 0x0a, 0xc5, 0xfb, 0x50, 
    0x71, 0xd9, 0xae, 0xb4, 0x74, 0xeb, 0x09, 0x93, 
    0x41, 0xd3, 0x70, 0x00, 0x60, 0x4b, 0x0e, 0x95, 
    0xb8, 0x31, 0x3d, 0x50, 0x84, 0x60, 0x6e, 0x16, 
    0xba, 0x7b, 0xc4, 0x8e, 0x25, 0xe0, 0x39, 0x10, 
    0x7e, 0xb9, 0x2a, 0xaf, 0x4b, 0x40, 0xd7, 0xc4, 
    0xa0, 0x2a, 0x19, 0xfe, 0x1f, 0x77, 0x04, 0x8f, 
    0x73, 0xe1, 0x6e, 0xfe, 0x8f, 0xbc, 0xcc, 0x8b, 
    0xdd, 0x12, 0x21, 0x86, 0xa6, 0x4d, 0x90, 0x9a, 
    0xbd, 0xfe, 0x8a, 0x05, 0x39, 0xad, 0xb4, 0x45, 
    0xf6, 0xf9, 0x0f, 0xe5, 0x91, 0x5c, 0x2c, 0x19, 
    0xa0, 0xf1, 0x2e, 0x06, 0x02, 0x92, 0x68, 0xaa, 
    0xbc, 0x7c, 0xf4, 0x4c, 0x85, 0x8b, 0x8c, 0x0b, 
    0x09, 0x83, 0xe6, 0x15, 0x24, 0xe3, 0x10, 0xa9, 
};//g_number
u8 const public_key[crypto::xr_dsa::public_key_length] = {
    0x0a, 0x0b, 0xe5, 0x02, 0x1c, 0xc6, 0x6c, 0xeb, 
    0xbd, 0xa2, 0xe6, 0xcd, 0x6d, 0x96, 0x64, 0xa9, 
    0x19, 0xe7, 0x81, 0x94, 0x0c, 0xb2, 0x98, 0x5c, 
    0x7c, 0x3c, 0x68, 0xb6, 0xb6, 0x20, 0xa9, 0xab, 
    0x0f, 0xc0, 0xb9, 0x5a, 0xe5, 0xe7, 0x35, 0x2e, 
    0xfd, 0xc2, 0x52, 0x64, 0x15, 0x98, 0x44, 0x88, 
    0x21, 0x4e, 0x8d, 0xf1, 0x04, 0x4f, 0xa5, 0x7a, 
    0x54, 0x7f, 0xc5, 0xeb, 0x9b, 0x39, 0x4a, 0x36, 
    0x3a, 0x79, 0xf1, 0xce, 0xd2, 0xaf, 0x47, 0xe5, 
    0x26, 0x0f, 0x56, 0xe2, 0x29, 0xd2, 0x60, 0x3d, 
    0x68, 0x23, 0xf1, 0x34, 0xea, 0xbf, 0x4e, 0x38, 
    0x66, 0x72, 0x0d, 0xf1, 0xc7, 0x33, 0x35, 0xf7, 
    0xc5, 0xe0, 0x59, 0x57, 0x42, 0x36, 0x83, 0x20, 
    0xff, 0x14, 0x03, 0xea, 0x2f, 0x0b, 0x0c, 0xa6, 
    0x98, 0x11, 0xd5, 0xb2, 0x96, 0x51, 0x6b, 0x08, 
    0x38, 0x98, 0x3a, 0xb6, 0x96, 0xa7, 0xad, 0xe3, 
};//public_key

} //namespace screenshots