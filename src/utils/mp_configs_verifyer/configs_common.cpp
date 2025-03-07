#include "pch.h"
#include "configs_common.h"

namespace mp_anticheat
{

// DSA params 
u8 const p_number[crypto::xr_dsa::public_key_length] = {
    0x98, 0xaf, 0x1d, 0x55, 0x86, 0xcb, 0xbf, 0x3e, 
    0x51, 0x00, 0x06, 0xab, 0x38, 0xca, 0xfe, 0x5f, 
    0xee, 0x69, 0xea, 0x8d, 0x33, 0xbc, 0x3e, 0x2c, 
    0x2b, 0x77, 0x74, 0xe9, 0xfb, 0x98, 0x1d, 0xf4, 
    0x5d, 0x87, 0x0a, 0xbe, 0x7e, 0x35, 0xef, 0x63, 
    0xce, 0xfd, 0x70, 0xc9, 0xe0, 0x95, 0x32, 0x68, 
    0x8c, 0x2c, 0x31, 0xac, 0xdb, 0xb7, 0xce, 0x0e, 
    0xf1, 0x95, 0xda, 0x0f, 0x57, 0x7c, 0x0d, 0x59, 
    0x34, 0xa3, 0xab, 0xb6, 0x97, 0x5b, 0xa8, 0x8e, 
    0x63, 0x3c, 0x03, 0xb6, 0xf6, 0x8d, 0x89, 0x0c, 
    0xf2, 0xeb, 0xed, 0x94, 0x92, 0x42, 0x27, 0x59, 
    0x6c, 0xdc, 0x19, 0x72, 0xa4, 0xd1, 0xc2, 0xe7, 
    0x49, 0x78, 0xa9, 0x31, 0xf6, 0x72, 0xa3, 0xde, 
    0x95, 0x97, 0x4c, 0xb3, 0x74, 0xa1, 0xb1, 0x43, 
    0x31, 0x67, 0x51, 0x54, 0xba, 0x03, 0xa4, 0x9d, 
    0x9c, 0x10, 0x26, 0xeb, 0x28, 0x42, 0x4e, 0x41, 
};//p_number
u8 const q_number[crypto::xr_dsa::private_key_length] = {
    0xfb, 0x9b, 0x74, 0x86, 0xb1, 0x44, 0x2f, 0xb3, 
    0x66, 0x73, 0xb8, 0x6b, 0x67, 0xe7, 0xd8, 0x6c, 
    0x0a, 0x16, 0x12, 0xd1, 
};//q_number
u8 const g_number[crypto::xr_dsa::public_key_length] = {
    0x1e, 0xd2, 0xb7, 0xd6, 0x6b, 0x3b, 0xbb, 0x7a, 
    0xb9, 0xca, 0xd7, 0x10, 0x39, 0x40, 0x85, 0xdb, 
    0x97, 0x20, 0xf2, 0xa4, 0xe6, 0x2f, 0x3d, 0x38, 
    0x95, 0x0a, 0x0b, 0x50, 0xe0, 0xa3, 0xa9, 0xa8, 
    0x6d, 0x4c, 0x63, 0x76, 0x82, 0x73, 0x87, 0xec, 
    0xfe, 0x82, 0xd9, 0x84, 0xe1, 0x7b, 0x6f, 0x80, 
    0x61, 0x39, 0x6b, 0x10, 0xcf, 0xda, 0x9e, 0x97, 
    0x0e, 0x7d, 0x3d, 0x2f, 0x1e, 0xd8, 0xfb, 0x19, 
    0x78, 0x8e, 0x62, 0xa7, 0x0e, 0xd9, 0x6c, 0x36, 
    0x34, 0x88, 0x41, 0x01, 0x20, 0xf2, 0x79, 0x05, 
    0x70, 0x60, 0x1e, 0x27, 0xa4, 0xda, 0xf7, 0x7d, 
    0xa0, 0x49, 0x02, 0xc0, 0xb4, 0x87, 0xd2, 0xa7, 
    0x7c, 0xad, 0x88, 0xec, 0x5f, 0xe8, 0x5e, 0xee, 
    0x61, 0x3d, 0x9e, 0x29, 0x72, 0xb4, 0x16, 0x32, 
    0x13, 0x9d, 0x83, 0x1e, 0x00, 0x2c, 0xb0, 0x89, 
    0x30, 0xe3, 0xe6, 0xd4, 0x13, 0x05, 0x7e, 0xf4, 
};//g_number
u8 const public_key[crypto::xr_dsa::public_key_length] = {
    0x34, 0x7a, 0xe8, 0x4b, 0x69, 0x50, 0xcb, 0x66, 
    0xc7, 0xd3, 0xf7, 0x43, 0xc8, 0x6c, 0xd2, 0xc7, 
    0xdb, 0x0e, 0x4e, 0x45, 0xaf, 0xbf, 0xee, 0x5e, 
    0x98, 0x09, 0x62, 0x11, 0xde, 0xc6, 0x14, 0xf4, 
    0xc9, 0x4d, 0x83, 0x07, 0xa3, 0x50, 0xa5, 0x11, 
    0x19, 0x26, 0xcd, 0x79, 0x9c, 0x39, 0xfb, 0x11, 
    0x86, 0x85, 0x7f, 0x69, 0x9d, 0x63, 0xc1, 0xd7, 
    0xf9, 0x22, 0x5d, 0xc7, 0xd9, 0x9e, 0x67, 0x0e, 
    0x01, 0x21, 0x96, 0x4d, 0x19, 0xcc, 0x50, 0xac, 
    0xd1, 0x74, 0x9a, 0x99, 0x79, 0x6c, 0x5b, 0xb7, 
    0x03, 0xf1, 0xae, 0x5b, 0x2b, 0xea, 0x79, 0xc3, 
    0x2a, 0x91, 0xeb, 0xed, 0x2c, 0x66, 0x6f, 0x46, 
    0x58, 0xb2, 0x02, 0x28, 0x06, 0x63, 0x75, 0xf8, 
    0x61, 0x70, 0xdf, 0x5f, 0xf8, 0x47, 0xc5, 0x08, 
    0x1a, 0x38, 0xf6, 0x1d, 0x8d, 0xee, 0x3f, 0xd9, 
    0x42, 0x54, 0xb6, 0x64, 0x50, 0xb7, 0x04, 0x73, 
};//public_key


} //namespace mp_anticheat
