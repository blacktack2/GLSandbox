#pragma once

unsigned int glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
