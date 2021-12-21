#pragma once

// using을 통해 치환하면 사용하기 편하기 때문에 Type을 정의
// x64을 통해 더 넓은 메모리 공간으로 사용
using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;