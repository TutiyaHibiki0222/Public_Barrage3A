#pragma once
/*
	Project.h
*/
// プロジェクト固有のヘッダー
// 標準ライブラリのヘッダー.
#include <iostream>			// c++.
#include <vector>           // 拡張配列.
#include <array>            // 固定配列.
#include <fstream>			// ファイル読み込み.
#include <functional>		
#include <map>              // map.
#include <string>           // string型.
#include <tuple>            // tuple
#include <sstream>          // stringstream
#include <utility>          // utility (std::pair, std::move など)
#include <cmath>            // <cmath>
#include <chrono>			// 時間制御に必要.
#include <memory>
#include <unordered_set>
#include <limits>
#include <type_traits>

#ifdef _DEBUG
#include <cstdlib>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "DxLib.h"          // Dxライブラリに必要.
