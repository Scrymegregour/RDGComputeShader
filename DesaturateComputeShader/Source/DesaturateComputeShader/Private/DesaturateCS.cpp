#include "DesaturateCS.h"

IMPLEMENT_GLOBAL_SHADER(
	FDesaturateCS,
	"/CustomShaders1/Private/DesaturateShader.usf",
	"MainCS",
	SF_Compute
);